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
#include "qemu/timer.h"
#include "hw/irq.h"
#include "hw/ptimer.h"
#include "hw/qdev-properties.h"
#include "qemu/module.h"
#include "qemu/log.h"
#include "qom/object.h"
#include "sysemu/runstate.h"
#include "sysemu/runstate-action.h"
#include "sysemu/sysemu.h"

#include "hw/timer/rt_ostimer.h"

/* TODO Capture feature is not implement */

static void rt_ostimer_update(RTOSTIMERState *s)
{
    if (s->EVTIMERH == s->MATCH_H && s->EVTIMERL == s->MATCH_L)
    {
        s->OSEVENT_CTRL |= OSTIMER_INTRFLAG_MASK;
    }
    /* Update interrupts.  */
    if ((s->OSEVENT_CTRL & OSTIMER_INTENA_MASK) == OSTIMER_INTENA_MASK) {
        if ((s->OSEVENT_CTRL & OSTIMER_INTRFLAG_MASK) == OSTIMER_INTRFLAG_MASK) {
            qemu_irq_raise(s->irq);
            qemu_system_wakeup_request(QEMU_WAKEUP_REASON_OTHER, NULL);
            //qemu_log("ostimer raise irq 0x%x\n", s->OSEVENT_CTRL);
        }
    } else {
        qemu_irq_lower(s->irq);
    }
}

/* Check all active timers, and schedule the next timer interrupt.  */

static uint64_t rt_ostimer_read(void *opaque, hwaddr offset, unsigned size)
{
    RTOSTIMERState *s = RT_OSTIMER(opaque);
    uint32_t v = 0;

    switch (offset) {
    case 0x0:
        v = s->EVTIMERL;
        break;
    case 0x4:
        v = s->EVTIMERH;
        break;
    case 0x8:
        v = s->CAPTURE_L; 
        break;
    case 0xc:
        v = s->CAPTURE_H;
        break;
    case 0x10:
        v = s->MATCH_L;
        break;
    case 0x14:
        v = s->MATCH_H;
        break;
    case 0x1C:
        v = s->OSEVENT_CTRL;
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset %x\n", __func__, (int)offset);
        break;
    }

    //printf("[RT_OSTIMER]: read 0x%lx = 0x%x\n", offset, v);
    return v;
}


static void rt_ostimer_write(void *opaque, hwaddr offset,
                            uint64_t value, unsigned size)
{
    RTOSTIMERState *s = RT_OSTIMER(opaque);

    //printf("[RT_OSTIMER]: write 0x%lx = 0x%lx\n", offset, value);

    switch (offset) {
    case 0x0:
        /* RO */
        break;
    case 0x4:
        /* RO */
        break;
    case 0x8:
        /* RO */
        break;
    case 0xc:
        /* RO */
        break;
    case 0x10:
        s->MATCH_L = value;
        break;
    case 0x14:
        s->MATCH_H = value;
        break;
    case 0x1C:
        s->OSEVENT_CTRL = value;
        if ((value & OSTIMER_INTRFLAG_MASK) == OSTIMER_INTRFLAG_MASK)
        {
            s->OSEVENT_CTRL &= (~OSTIMER_INTRFLAG_MASK);
        }
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset %x\n", __func__, (int)offset);
    }
}


static const MemoryRegionOps rt_ostimer_ops = {
    .read  = rt_ostimer_read,
    .write = rt_ostimer_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_rt_ostimer = {
    .name = TYPE_RT_OSTIMER,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(EVTIMERL, RTOSTIMERState),
        VMSTATE_UINT32(EVTIMERH, RTOSTIMERState),
        VMSTATE_UINT32(CAPTURE_L, RTOSTIMERState),
        VMSTATE_UINT32(CAPTURE_H, RTOSTIMERState),
        VMSTATE_UINT32(MATCH_L, RTOSTIMERState),
        VMSTATE_UINT32(MATCH_H, RTOSTIMERState),
        VMSTATE_UINT32(OSEVENT_CTRL, RTOSTIMERState),
        VMSTATE_PTIMER(timer, RTOSTIMERState),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_ostimer_tick(void *opaque)
{
    static uint64_t ticks = 0;
    uint64_t r = 0;
    RTOSTIMERState *s = RT_OSTIMER(opaque);

    ticks += 1;
    //printf("[OSTIMER] r = 0x%lx\n", ticks);
    /*convert to gray code*/
    r = ticks^(ticks>>1);

    s->EVTIMERH = (uint32_t)(r>>32);
    s->EVTIMERL = (uint32_t)(r&0xFFFFFFFF);
    //printf("[OSTIMER] s->EVTIMERL = 0x%x\n", s->EVTIMERL);
    //printf("[OSTIMER] s->EVTIMERH = 0x%x\n", s->EVTIMERH);
    rt_ostimer_update(s);
}

static void rt_ostimer_realize(DeviceState *dev, Error **errp)
{
    RTOSTIMERState *s = RT_OSTIMER(dev);


    s->timer = ptimer_init(rt_ostimer_tick, s, PTIMER_POLICY_CONTINUOUS_TRIGGER | PTIMER_POLICY_WRAP_AFTER_ONE_PERIOD); 
    ptimer_transaction_begin(s->timer);
    ptimer_set_freq(s->timer, 600000000);
    ptimer_run(s->timer, 0);
    ptimer_transaction_commit(s->timer);
}


static void rt_ostimer_unrealize(DeviceState *dev)
{
    RTOSTIMERState *s = RT_OSTIMER(dev);

    if(s->timer) {
        ptimer_transaction_begin(s->timer);
        ptimer_stop(s->timer);
        ptimer_transaction_commit(s->timer);
        ptimer_free(s->timer);
        s->timer = NULL;
    }
}

static void rt_ostimer_init(Object *obj)
{
    RTOSTIMERState *s = RT_OSTIMER(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->iomem, obj, &rt_ostimer_ops, s,
                          TYPE_RT_OSTIMER, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
}

static void rt_ostimer_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_ostimer_realize;
    dc->unrealize = rt_ostimer_unrealize;
    dc->vmsd = &vmstate_rt_ostimer;
}

static const TypeInfo rt_ostimer_info = {
    .name          = TYPE_RT_OSTIMER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTOSTIMERState),
    .instance_init = rt_ostimer_init,
    .class_init    = rt_ostimer_class_init,
};

static void rt_ostimer_register_types(void)
{
    type_register_static(&rt_ostimer_info);
}

type_init(rt_ostimer_register_types)
