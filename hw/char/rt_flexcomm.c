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
#include "chardev/char-fe.h"
#include "chardev/char-serial.h"
#include "hw/char/rt_flexcomm.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "qemu/fifo8.h"
#include "sysemu/sysemu.h"

static void rt_flexcomm_update(RTFLEXCOMMState *s);

static void uart_update_parameters(RTFLEXCOMMState *s)
{
    QEMUSerialSetParams ssp;

    /* FIXME hardcode to 115200 8N1 */
    ssp.data_bits = 8;
    ssp.parity = 'N';
    ssp.stop_bits = 1;
    ssp.speed = 115200;
    qemu_chr_fe_ioctl(&s->chr, CHR_IOCTL_SERIAL_SET_PARAMS, &ssp);
}

static int rt_flexcomm_post_load(void *opaque, int version_id)
{
    RTFLEXCOMMState *s = (RTFLEXCOMMState *)opaque;

    uart_update_parameters(s);
    return 0;
}

static const VMStateDescription vmstate_rt_flexcomm = {
    .name = TYPE_RT_FLEXCOMM,
    .version_id = 1,
    .minimum_version_id = 0,
    .post_load = rt_flexcomm_post_load,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(USART_CFG, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_CTL, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_STAT, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_INTENSET, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_INTENCLR, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_BRG, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_INTSTAT, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_OSR, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_ADDR, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOCFG, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOSTAT, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOTRIG, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOINTENSET, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOINTENCLR, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOINTSTAT, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOWR, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFORD, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFORDNOPOP, RTFLEXCOMMState),
        VMSTATE_UINT32(USART_FIFOSIZE, RTFLEXCOMMState),
        VMSTATE_UINT32(PSELID, RTFLEXCOMMState),
        VMSTATE_UINT32(ID, RTFLEXCOMMState),
        VMSTATE_UINT32(FIFO_SIZE, RTFLEXCOMMState),
        VMSTATE_END_OF_LIST()
    },
};

/* Try to send tx data, and arrange to be called back later if
 * we can't (ie the char backend is busy/blocking).
 */
static gboolean uart_transmit(void *do_not_use, GIOCondition cond, void *opaque)
{
    RTFLEXCOMMState *s = RT_FLEXCOMM(opaque);
    int ret;
    uint32_t num = fifo8_num_used(s->TXFIFO);

    if (num == 0) {
        return FALSE;
    }
    const uint8_t * txbuf = fifo8_pop_buf(s->TXFIFO, num, &num);
    ret = qemu_chr_fe_write(&s->chr, txbuf, num);
    if (ret <= 0) {
        printf("fw_write error %d\n", ret);
        fifo8_push_all(s->TXFIFO, txbuf, num);
        guint _r = qemu_chr_fe_add_watch(&s->chr, G_IO_OUT | G_IO_HUP,
                                             uart_transmit, s);
        if (!_r) {
            printf("schedule fw_write error %d\n", _r);
            /* Most common reason to be here is "no chardev backend":
             * just insta-drain the buffer, so the serial output
             * goes into a void, rather than blocking the guest.
             */
            goto buffer_drained;
        }
        return FALSE;
    }
    rt_flexcomm_update(s);

buffer_drained:
    /* Going from TXFULL set to clear triggers the tx interrupt */
    return FALSE;
}

static void rt_flexcomm_update(RTFLEXCOMMState *s)
{
    uint32_t psel = s->PSELID & PERSEL_MASK;
    int raise_irq = 0;

    switch (psel) {
        case 0:
            break;
        case 1:
            s->PSELID |= USARTPRESE_MASK;
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
    }

    if(fifo8_is_full(s->TXFIFO))
    {
        s->USART_FIFOSTAT &= ~USART_STAT_TXNOTFULL_MASK; 
    }

    if(fifo8_is_full(s->RXFIFO))
    {
        s->USART_FIFOSTAT |= USART_STAT_RXFULL_MASK;
    }

    if(fifo8_is_empty(s->RXFIFO))
    {
        s->USART_FIFOSTAT &= ~USART_STAT_RXNOTEMPTY_MASK;
    } else {
        s->USART_FIFOSTAT |= USART_STAT_RXNOTEMPTY_MASK;
    }

    if(fifo8_is_empty(s->TXFIFO))
    {
        s->USART_FIFOSTAT |= USART_STAT_TXNOTFULL_MASK;
    } else {
        s->USART_FIFOSTAT &= ~USART_STAT_TXNOTFULL_MASK;
        uart_transmit(NULL, G_IO_OUT, s);
    }

    if(fifo8_num_used(s->RXFIFO) >= FIFOTRIG_RXLVL(s))
    {
        s->USART_FIFOINTSTAT |= USART_FIFOINTSTAT_RXLVL;
    } else {
        s->USART_FIFOINTSTAT &= ~USART_FIFOINTSTAT_RXLVL;
    }
 
    if(fifo8_num_used(s->TXFIFO) >= FIFOTRIG_TXLVL(s))
    {
        s->USART_FIFOINTSTAT |= USART_FIFOINTSTAT_TXLVL;
    } else {
        s->USART_FIFOINTSTAT &= ~USART_FIFOINTSTAT_TXLVL;
    }

    if((s->USART_FIFOINTENSET & USART_FIFOINTSTAT_RXLVL) == USART_FIFOINTSTAT_RXLVL)
    {
	if ((s->USART_FIFOINTSTAT & USART_FIFOINTSTAT_RXLVL) == USART_FIFOINTSTAT_RXLVL)
        {
          raise_irq = 1;
        }
    }

    if((s->USART_FIFOINTENSET & USART_FIFOINTSTAT_TXLVL) == USART_FIFOINTSTAT_TXLVL)
    {
	if ((s->USART_FIFOINTSTAT & USART_FIFOINTSTAT_TXLVL) == USART_FIFOINTSTAT_TXLVL)
        {
          raise_irq = 1;
        }
    }
    qemu_set_irq(s->irq, raise_irq);
}

static void rt_flexcomm_reset(RTFLEXCOMMState *s)
{
    s->USART_CFG = 0x0;
    s->USART_CTL = 0x0;
    s->USART_STAT = 0xA;
    s->USART_INTENSET = 0x0;
    s->USART_INTENCLR = 0x0;
    s->USART_BRG = 0x0;
    s->USART_INTSTAT = 0x0;
    s->USART_OSR = 0xF;
    s->USART_ADDR = 0x0;
    s->USART_FIFOCFG = 0x0;
    s->USART_FIFOSTAT = 0x30;
    s->USART_FIFOTRIG = 0x0;
    s->USART_FIFOINTENSET = 0x0;
    s->USART_FIFOINTENCLR = 0x0;
    s->USART_FIFOINTSTAT = 0x0;
    s->USART_FIFOWR = 0x0;
    s->USART_FIFORD = 0x0;
    s->USART_FIFORDNOPOP = 0x0;
    s->USART_FIFOSIZE = 0x10;
    s->ID = 0x0FFC0000;
    s->PSELID = 0x1020f0;
}

static void rt_flexcomm_reset_at_boot(DeviceState *dev)
{
    RTFLEXCOMMState *s = RT_FLEXCOMM(dev);

    rt_flexcomm_reset(s);
}

static int uart_can_receive(void *opaque)
{
    RTFLEXCOMMState *s = RT_FLEXCOMM(opaque);

    /* We can take a char if RX is enabled and the buffer is empty */
    if ((s->USART_FIFOSTAT & USART_STAT_RXFULL_MASK) == 0x0) {
        return 1;
    }
    return 0;
}

static void uart_receive(void *opaque, const uint8_t *buf, int size)
{
    RTFLEXCOMMState *s = RT_FLEXCOMM(opaque);

    int i = 0;
    while(i < size) {
        fifo8_push(s->RXFIFO, buf[i]);
        rt_flexcomm_update(s);
        qemu_chr_fe_accept_input(&s->chr);
        i++;
    }
}

static uint64_t rt_flexcomm_read(void *opaque, hwaddr offset,
                                unsigned size)
{
    RTFLEXCOMMState *s = (RTFLEXCOMMState *)opaque;
    uint32_t v = 0;
 
    switch (offset) {
    case 0x0:
	v = s->USART_CFG;
        break;
    case 0x4:
        v =  s->USART_CTL;
        break;
    case 0x8:
        v = s->USART_STAT;
        break;
    case 0xC:
        v = s->USART_INTENSET;
        break;
    case 0x10:
        v = s->USART_INTENCLR;
        break;
    case 0x20:
        v = s->USART_BRG;
        break;
    case 0x24:
        v = s->USART_INTSTAT;
        break;
    case 0x28:
        v = s->USART_OSR;
        break;
    case 0x2C:
        v = s->USART_ADDR;
        break;
    case 0xE00:
        v = s->USART_FIFOCFG;
        break;
    case 0xE04:
        v = s->USART_FIFOSTAT;
        break;
    case 0xE08:
        v = s->USART_FIFOTRIG;
        break;
    case 0xE10:
        v = s->USART_FIFOINTENSET;
        break;
    case 0xE14:
        v = s->USART_FIFOINTENCLR;
        break;
    case 0xE18:
        v = s->USART_FIFOINTSTAT;
        break;
    case 0xE20:
        v = s->USART_FIFOWR;
        break;
    case 0xE30:
        if (fifo8_num_used(s->RXFIFO)) {
 	    s->USART_FIFORD = fifo8_pop(s->RXFIFO);
        }
        v = s->USART_FIFORD;
        break;
    case 0xE40:
        v = s->USART_FIFORDNOPOP;
        break;
    case 0xE48:
        v = s->USART_FIFOSIZE;
        break;
    case 0xFFC:
        v = s->ID;
        break;
    case 0xFF8: 
        v = s->PSELID;
        break;
    default:
        break;;
    }
    rt_flexcomm_update(s);
    return v;
}


static void rt_flexcomm_write(void *opaque, hwaddr offset,
                             uint64_t value, unsigned size)
{
    RTFLEXCOMMState *s = (RTFLEXCOMMState *)opaque;

    switch (offset) {
    case 0x0:
	    s->USART_CFG = value;
        break;
    case 0x4:
        s->USART_CTL = value;
        break;
    case 0x8:
        s->USART_STAT = value;
        break;
    case 0xC:
        s->USART_INTENSET = value;
        break;
    case 0x10:
        s->USART_INTENCLR = value;
        break;
    case 0x20:
        s->USART_BRG = value;
        break;
    case 0x24:
        // s->INTSTAT read only
        break;
    case 0x28:
        s->USART_OSR = value;
        break;
    case 0x2C:
        s->USART_ADDR = value;
        break;
    case 0xE00:
        s->USART_FIFOCFG = value;
        break;
    case 0xE04:
        //s->FIFOSTAT read only
        break;
    case 0xE08:
        s->USART_FIFOTRIG = value;
        break;
    case 0xE10:
        s->USART_FIFOINTENSET = value;
        break;
    case 0xE14:
        s->USART_FIFOINTENCLR = value;
        break;
    case 0xE18:
        // s->FIFOINTSTAT read only
        break;
    case 0xE20:
        s->USART_FIFOWR = value;
        if (s->USART_CTL && USARTD_TXDIS_MASK)
        {
            break;
        }
        printf("%c", (char)value);
        if ((s->USART_FIFOSTAT & USART_STAT_TXNOTFULL_MASK) == USART_STAT_TXNOTFULL_MASK)
        {
            fifo8_push(s->TXFIFO, s->USART_FIFOWR);
        }
        break;
    case 0xE30:
        // s->FIFORD read only
        break;
    case 0xE40:
        // s->FIFORDNOPOP read only;
        break;
    case 0xE48:
        // s->FIFOSIZE read only
        break;
    case 0xFF8:
        s->PSELID = value;
        break;
    default:
        break;
    }

    rt_flexcomm_update(s);
}

static const struct MemoryRegionOps rt_flexcomm_ops = {
    .read = rt_flexcomm_read,
    .write = rt_flexcomm_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_flexcomm_realize(DeviceState *dev, Error **errp)
{
    RTFLEXCOMMState *s = RT_FLEXCOMM(dev);

    s->RXFIFO = (Fifo8 *)g_malloc0(sizeof(Fifo8));
    s->TXFIFO = (Fifo8 *)g_malloc0(sizeof(Fifo8));

    fifo8_create(s->RXFIFO, s->FIFO_SIZE);
    fifo8_create(s->TXFIFO, s->FIFO_SIZE);

    qemu_chr_fe_set_handlers(&s->chr, uart_can_receive, uart_receive,
                             NULL, NULL, s, NULL, true);
 
}

static void rt_flexcomm_unrealize(DeviceState *dev)
{
    RTFLEXCOMMState *s = RT_FLEXCOMM(dev);

    fifo8_destroy(s->RXFIFO);
    fifo8_destroy(s->TXFIFO);

    g_free(s->RXFIFO);
    g_free(s->TXFIFO);
}

static void rt_flexcomm_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RTFLEXCOMMState *s = (RTFLEXCOMMState *)obj;

    memory_region_init_io(&s->iomem, obj, &rt_flexcomm_ops, s,
                          TYPE_RT_FLEXCOMM, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
}

static Property rt_flexcomm_properties[] = {
    DEFINE_PROP_UINT32("FIFO_SIZE", RTFLEXCOMMState, FIFO_SIZE, 16),
    DEFINE_PROP_CHR("chardev", RTFLEXCOMMState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void rt_flexcomm_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_flexcomm_realize;
    dc->vmsd = &vmstate_rt_flexcomm;
    dc->reset = rt_flexcomm_reset_at_boot;
    set_bit(DEVICE_CATEGORY_INPUT, dc->categories);
    dc->desc = "RT FLEXCOMM";
    dc->unrealize = rt_flexcomm_unrealize;
    device_class_set_props(dc, rt_flexcomm_properties);
}

static const TypeInfo rt_flexcomm_info = {
    .name           = TYPE_RT_FLEXCOMM,
    .parent         = TYPE_SYS_BUS_DEVICE,
    .instance_size  = sizeof(RTFLEXCOMMState),
    .instance_init  = rt_flexcomm_init,
    .class_init     = rt_flexcomm_class_init,
};

static void rt_flexcomm_register_types(void)
{
    type_register_static(&rt_flexcomm_info);
}

type_init(rt_flexcomm_register_types)
