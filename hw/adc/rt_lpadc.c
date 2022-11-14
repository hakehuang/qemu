/*
 * Copyright (c) 2022, NXP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "migration/vmstate.h"
#include "hw/qdev-core.h"
#include "hw/qdev-properties.h"
#include "qemu/dma_notifier.h"
#include "qemu/fifo32.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qemu/timer.h"
#include "qemu/typedefs.h"
#include "hw/irq.h"
#include "hw/adc/rt_lpadc.h"

#ifndef RT_LPADC_ERR_DEBUG
#define RT_LPADC_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (RT_LPADC_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)


static void rt_lpadc_reset(DeviceState *dev)
{
    RTLPADCState *s = RT_LPADC(dev);
    int i = 0;

    s->VERID = 0x0200201A;
    s->PARAM = 0x0F041010;
    s->CTRL = 0x00000000;
    s->STAT = 0x00000000;
    s->IE = 0x00000000;
    s->DE = 0x00000000;
    s->CFG = 0x00800000;
    s->PAUSE = 0x00000000;
    s->SWTRIG = 0x00000000;
    s->TSTAT = 0x00000000;
    for (i = 0; i < 16; i++) {
        s->TCTRL[i] = 0x00000000;
    }
    s->FCTRL[0] = 0x00000000;
    s->FCTRL[1] = 0x00000000;
    for (i = 0; i < ADC_COMMAND_BUF_NUM; i++) {
        s->CMDL[i] = 0x00002000;
        s->CMDH[i] = 0x00000000;
    }
    for (i = 0; i < ADC_CMP_VALUE_NUM; i++) {
        s->CV[i] = 0x00000000;
    }
    fifo32_reset(&s->RESFIFO[0]);
    fifo32_reset(&s->RESFIFO[1]);
}

static void rt_lpadc_run_cmds(RTLPADCState *s, uint32_t ch)
{
    uint32_t status = s->STAT;
    uint32_t next = ch;

    status = (status & (~CMDACT_MASK)) | (ch<<CMDACT_OFFSET);
    s->STAT = status | ADC_ACTIVE_MASK;

    while(next != 0)
    {
        uint32_t fifo_sel = FROM_REG(s->TCTRL[next], FIFO_SEL_A);
        uint32_t cmdsrc = FROM_REG(s->TCTRL[next], TCMD);
        uint32_t data = 0x8000FFFF; /* faked value in low 16 bits*/

        data = TO_REG(data, CMDSRC, cmdsrc);
        data = TO_REG(data, TSRC, next);
        fifo32_push(&s->RESFIFO[fifo_sel], data);
        next = FROM_REG(s->CMDH[next], NEXT);
        if (s->PAUSE & PAUSEEN_MASK) {
            uint32_t delay = s->PAUSE & PAUSEDLY_MASK;
            if (delay > 0) {
                while(delay--);
            }
        }
    }

    s->STAT &= (~ADC_ACTIVE_MASK) & (~CMDACT_MASK);
}

static void rt_lpadc_update_state(void *opaque)
{
    RTLPADCState *s = RT_LPADC(opaque);

    if ((s->CTRL & ADCEN_MASK) != ADCEN_MASK) {
        s->state = eLPADC_Stop;
    } else {
        s->state = eLPADC_Run;
    }

    if ((s->CTRL & DOZEN_MASK) == DOZEN_MASK) {
        s->state = eLPADC_Dozen;
    }

    if (s->DE & FWMDE1_MASK) {
        uint32_t rdy = FROM_REG(s->STAT, RDY1);
        if (rdy){
            dma_trigger_notify(&s->dma_trigger_nr);
        }
    }
    if (s->DE & FWMDE0_MASK) {
        uint32_t rdy = FROM_REG(s->STAT, RDY0);

        if (rdy){
            dma_trigger_notify(&s->dma_trigger_nr);
        }
    }

}

static void rt_lpadc_timer_event_fsm_cb(void *opaque)
{
    RTLPADCState *s = RT_LPADC(opaque);

    switch(s->state) {
        case eLPADC_Run:
            break;
        case eLPADC_Stop:
            break;
        case eLPADC_Dozen:
            break;
        default:
            break;
    }

    rt_lpadc_update_state(s);
}


static void rt_lpadc_realize(DeviceState *dev, Error **errp)
{
    RTLPADCState *s = RT_LPADC(dev);

    fifo32_create(&s->RESFIFO[0], RESFIFO_SIZE);
    fifo32_create(&s->RESFIFO[1], RESFIFO_SIZE);

    s->timer_interval = 2; /* every 2 ms*/
    s->timer = timer_new_ms(QEMU_CLOCK_VIRTUAL, rt_lpadc_timer_event_fsm_cb, s);
    int64_t now = qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL);

    timer_mod(s->timer, now + s->timer_interval);

    s->state = eLPADC_Stop;
}


static void rt_lpadc_unrealize(DeviceState *dev)
{
    RTLPADCState *s = RT_LPADC(dev);

    fifo32_destroy(&s->RESFIFO[0]);
    fifo32_destroy(&s->RESFIFO[1]);

    if(s->timer) {
        timer_del(s->timer);
        s->timer = NULL;
    }
}

static uint64_t rt_lpadc_read(void *opaque, hwaddr offset,
                                     unsigned int size)
{
    RTLPADCState *s = opaque;
    uint32_t value = 0;

    DB_PRINT("Address: 0x%" HWADDR_PRIx "\n", offset);

    switch (offset) {
        case 0x0:
            value = s->VERID;
            break;
        case 0x4:
            value = s->PARAM;
            break;
        case 0x10:
            value = s->CTRL;
            break;
        case 0x14:
            break;
        case 0x18:
            value = s->IE;
            break;
        case 0x1C:
            value = s->DE;
            break;
        case 0x20:
            value = s->CFG;
            break;
        case 0x24:
            value = s->PAUSE;
            break;
        case 0x34:
            value = 0;
            break;
        case 0x38:
            break;
        case 0xA0 ... 0xDC:
            value = s->TCTRL[(offset - 0xA0) / 4];
            break;
        case 0xE0:
            value = s->FCTRL[0];
            break;
        case 0xE4:
            value = s->FCTRL[1];
            break;
        case 0x100 ... 0x174:
            {
                int reg_id = (offset - 0x100) / 4;
                if ((reg_id % 2) == 0) {
                    int idx = reg_id>>1;
                    value = s->CMDL[idx];
                } else {
                    int idx = (reg_id + 1)>>1;
                    value = s->CMDH[idx];
                }
            }
            break;
        case 0x200 ... 0x20C:
            value = s->CV[(offset - 0x200)/4];
            break;
        case 0x300 ... 0x304:
            int reg_id = (offset - 0x300)/4;
            if (!fifo32_is_empty(&s->RESFIFO[reg_id]))
            {
                value = fifo32_pop(&s->RESFIFO[reg_id]);
                s->FCTRL[reg_id] |= fifo32_num_used(&s->RESFIFO[reg_id]);
            }
            break;
    }

    return value;
}

static void rt_lpadc_ctrl_reg_cb(void *opaque)
{
    RTLPADCState *s = opaque;

    if (s->CTRL & RSTFIFO0_MASK) {
        fifo32_reset(&s->RESFIFO[0]);
    }

    if (s->CTRL & RSTFIFO1_MASK) {
        fifo32_reset(&s->RESFIFO[1]);
    }

    if (s->CTRL & RST_MASK) {
        uint32_t temp_r = s->CTRL;
        rt_lpadc_reset(DEVICE(s));
        s->CTRL |= temp_r;
    }

    if (s->CTRL & ADCEN_MASK) {
        s->state = eLPADC_Run;
    }
}

static void rt_lpadc_status_cb(void *opaque)
{
    RTLPADCState *s = opaque;
    int raise_irq = 0;
    uint32_t count = 0;


    if (fifo32_is_full(&s->RESFIFO[0])) {
        s->STAT |= FOF0_MASK;
        if (s->IE & FOF0_MASK)
        {
            raise_irq = 1;
        }
    }

    if (fifo32_is_full(&s->RESFIFO[1])) {
        s->STAT |= FOF1_MASK;
        if (s->IE & FOF1_MASK)
        {
            raise_irq = 1;
        }
    }

    count = fifo32_num_used(&s->RESFIFO[0]);
    if (count > (s->FCTRL[0]>>FWMARK_OFFSET))
    {
        if (s->IE & FWMIE0_MASK)
        {
            raise_irq = 1;
        }
    }

    count = fifo32_num_used(&s->RESFIFO[1]);
    if (count > (s->FCTRL[1]>>FWMARK_OFFSET))
    {
        if (s->IE & FWMIE1_MASK)
        {
            raise_irq = 1;
        }

    }

    qemu_set_irq(s->irq, raise_irq);   
}

static void lp_adc_swtrigger(void *opaque)
{
    RTLPADCState *s = opaque;
    uint32_t ids = s->SWTRIG;
    int i = 0;

    for (i = 0; i < TST_REG_NUM; i++) {
        if (ids && (ids&(1<<i))!=0)
        {
            uint32_t status = s->STAT;
            status = status & (~TRGACT_MASK);
            status = status | (i<<TRGACT_OFFSET);
            s->STAT = status;
            rt_lpadc_run_cmds(s, i);
            ids &= ~(1<<i);
            s->STAT &= (~TRGACT_MASK);
            s->STAT |= TCOMP_INT_MASK;
        }
    }
}

static void rt_lpadc_write(void *opaque, hwaddr offset,
                       uint64_t val64, unsigned int size)
{
    RTLPADCState *s = opaque;
    uint32_t value = (uint32_t) val64;

    DB_PRINT("Address: 0x%" HWADDR_PRIx ", Value: 0x%x\n",
             offset, value);

    switch (offset) {
        case 0x0:
            /* RO */
            break;
        case 0x4:
            /* RO */
            break;
        case 0x10:
            s->CTRL = value;
            rt_lpadc_ctrl_reg_cb(s);
            break;
        case 0x14:
            s->STAT &= (~value);
            break;
        case 0x18:
            s->IE = value;
            break;
        case 0x1C:
            s->DE = value;
            break;
        case 0x20:
            s->CFG = value;
            break;
        case 0x24:
            s->PAUSE = value;
            break;
        case 0x34:
            s->SWTRIG = value;
            lp_adc_swtrigger(s);
            break;
        case 0x38:
            s->TSTAT &= (~value);
            break;
        case 0xA0 ... 0xDC:
            s->TCTRL[(offset - 0xa0)/4] = value;
            break;
        case 0xE0:
            s->FCTRL[0] = value & FWMARK_MASK;
            break;
        case 0xE4:
            s->FCTRL[1] = value & FWMARK_MASK;
            break;
        case 0x100 ... 0x174:
            {
                int reg_id = (offset - 0x100) / 4;
                if ((reg_id % 2) == 0) {
                    int idx = reg_id>>1;
                    s->CMDL[idx] = value;
                } else {
                    int idx = (reg_id + 1)>>1;
                    s->CMDH[idx] = value;
                }
            }
            break;
        case 0x200 ... 0x20C:
            s->CV[(offset - 0x200)/4] = value;
            break;
        case 0x300 ... 0x304:
            /* RO */
            break;
        default:
            break;
    }
    rt_lpadc_status_cb(s);
}

static Property rt_lpadc_properties[] = {
    DEFINE_PROP_UINT32("dma_hw_trigger_nr", RTLPADCState, dma_trigger_nr, 24),
    DEFINE_PROP_END_OF_LIST(),
};

static const MemoryRegionOps rt_lpadc_ops = {
    .read = rt_lpadc_read,
    .write = rt_lpadc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_rt_lpadc = {
    .name = TYPE_RT_LPADC,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(VERID, RTLPADCState),
        VMSTATE_UINT32(PARAM, RTLPADCState),
        VMSTATE_UINT32(CTRL, RTLPADCState),
        VMSTATE_UINT32(STAT, RTLPADCState),
        VMSTATE_UINT32(IE, RTLPADCState),
        VMSTATE_UINT32(DE, RTLPADCState),
        VMSTATE_UINT32(CFG, RTLPADCState),
        VMSTATE_UINT32(PAUSE, RTLPADCState),
        VMSTATE_UINT32(SWTRIG, RTLPADCState),
        VMSTATE_UINT32(TSTAT, RTLPADCState),
        VMSTATE_UINT32_ARRAY(TCTRL, RTLPADCState, TST_REG_NUM),
        VMSTATE_UINT32_ARRAY(FCTRL, RTLPADCState, FIFO_CTL_NUM),
        VMSTATE_UINT32_ARRAY(CMDL, RTLPADCState, ADC_COMMAND_BUF_NUM),
        VMSTATE_UINT32_ARRAY(CMDH, RTLPADCState, ADC_COMMAND_BUF_NUM),
        VMSTATE_UINT32_ARRAY(CV, RTLPADCState, ADC_CMP_VALUE_NUM),
        VMSTATE_STRUCT_ARRAY(RESFIFO, RTLPADCState, FIFO_CTL_NUM, 1, vmstate_fifo8, Fifo32),
        VMSTATE_UINT32(timer_interval, RTLPADCState),
        VMSTATE_TIMER_PTR(timer, RTLPADCState),
        VMSTATE_UINT32(state, RTLPADCState),
        VMSTATE_UINT32(dma_trigger_nr, RTLPADCState),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_lpadc_init(Object *obj)
{
    RTLPADCState *s = RT_LPADC(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &rt_lpadc_ops, s,
                          TYPE_RT_LPADC, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void rt_lpadc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = rt_lpadc_reset;
    dc->vmsd = &vmstate_rt_lpadc;
    dc->realize = rt_lpadc_realize;
    dc->unrealize = rt_lpadc_unrealize;
    dc->user_creatable = true;
    dc->hotpluggable = true;

    device_class_set_props(dc, rt_lpadc_properties);
}

static const TypeInfo rt_lpadc_info = {
    .name          = TYPE_RT_LPADC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTLPADCState),
    .instance_init = rt_lpadc_init,
    .class_init    = rt_lpadc_class_init,
};

static void rt_lpadc_register_types(void)
{
    type_register_static(&rt_lpadc_info);
}

type_init(rt_lpadc_register_types)
