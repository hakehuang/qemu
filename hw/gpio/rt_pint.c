/*
 * Copyright (c) 2023, NXP
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
#include "qemu/main-loop.h"
#include "qemu/cutils.h"
#include "qemu/module.h"
#include "qemu/guest-random.h"
#include "hw/irq.h"
#include "hw/gpio/rt_pint.h"
#include "migration/vmstate.h"


#define RT_PINT_NAME "Pin Interrupt and Pattern Match"

static uint64_t rt_pint_read(void *opaque, hwaddr offset, unsigned size)
{
    RTPINTState *s = RT_PINT(opaque);
    uint32_t value = 0;

    switch(offset) {
        case 0x0:
        value = s->ISEL;
        break;
        case 0x4:
        value = s->IENR;
        break;
        case 0x8:
        value = s->SIENR;
        break;
        case 0xc:
        value = s->CIENR;
        break;
        case 0x10:
        value = s->IENF;
        break;
        case 0x14:
        value = s->SIENF;
        break;
        case 0x18:
        value = s->CIENF;
        break;
        case 0x1c:
        value = s->RISE;
        break;
        case 0x20:
        value = s->FALL;
        break;
        case 0x24:
        value = s->IST;
        break;
        case 0x28:
        value = s->ISEL;
        break;
        case 0x2c:
        value = s->PMSRC;
        break;
        case  0x30:
        value = s->PMCFG;
        break;
    }

    return value;
}

static void rt_pint_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTPINTState *s = RT_PINT(opaque);

    switch(offset) {
        case 0x0:
        s->ISEL = value;
        break;
        case 0x4:
        s->IENR = value;
        break;
        case 0x8:
        s->SIENR = value;
        break;
        case 0xc:
        s->CIENR = value;
        break;
        case 0x10:
        s->IENF = value;
        break;
        case 0x14:
        s->SIENF = value;
        break;
        case 0x18:
        s->CIENF = value;
        break;
        case 0x1c:
        s->RISE = value;
        break;
        case 0x20:
        s->FALL = value;
        break;
        case 0x24:
        s->IST = value;
        break;
        case 0x28:
        s->ISEL = value;
        break;
        case 0x2c:
        s->PMSRC = value;
        break;
        case 0x30:
        s->PMCFG = value;
        break;
        default:
        break;
    }
}

static const MemoryRegionOps rt_pint_ops = {
    .read  = rt_pint_read,
    .write = rt_pint_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_pint_realize(DeviceState *dev, Error **errp)
{
    RTPINTState *s = RT_PINT(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_pint_ops, s,
                          TYPE_RT_PINT, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_pint = {
    .name = RT_PINT_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(ISEL, RTPINTState),
        VMSTATE_UINT32(IENR, RTPINTState),
        VMSTATE_UINT32(SIENR, RTPINTState),
        VMSTATE_UINT32(CIENR, RTPINTState),
        VMSTATE_UINT32(IENF, RTPINTState),
        VMSTATE_UINT32(SIENF, RTPINTState),
        VMSTATE_UINT32(CIENF, RTPINTState),
        VMSTATE_UINT32(RISE, RTPINTState),
        VMSTATE_UINT32(FALL, RTPINTState),
        VMSTATE_UINT32(IST, RTPINTState),
        VMSTATE_UINT32(PMCTRL, RTPINTState),
        VMSTATE_UINT32(PMSRC, RTPINTState),
        VMSTATE_UINT32(PMCFG, RTPINTState),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_pint_init(Object *obj)
{
    RTPINTState *s = RT_PINT(obj);

    qdev_init_gpio_out_named(DEVICE(SYS_BUS_DEVICE(obj)), s->irq, SYSBUS_DEVICE_GPIO_IRQ, 8);

    memory_region_init_io(&s->iomem, obj, &rt_pint_ops, s, TYPE_RT_PINT, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_pint_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_pint_realize;
    dc->vmsd = &vmstate_rt_pint;
}

static const TypeInfo rt_pint_info = {
    .name          = TYPE_RT_PINT,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTPINTState),
    .instance_init = rt_pint_init,
    .class_init    = rt_pint_class_init,
};

static void rt_pint_register_types(void)
{
    type_register_static(&rt_pint_info);
}

type_init(rt_pint_register_types)

