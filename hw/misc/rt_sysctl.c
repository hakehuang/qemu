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
#include "qemu/main-loop.h"
#include "qemu/cutils.h"
#include "qemu/module.h"
#include "qemu/guest-random.h"
#include "hw/irq.h"
#include "hw/misc/rt_sysctl.h"
#include "migration/vmstate.h"


#define RT_SYSCTL0_NAME "system controllor0 for NXP RTxxx series"
#define RT_SYSCTL1_NAME "system controllor1 for NXP RTxxx series"

static uint64_t rt_sysctl0_read(void *opaque, hwaddr offset, unsigned size)
{
    RTSYSCTL0State *s = RT_SYSCTL0(opaque);
     
    switch(offset) {
        default:
            break;
    }    

    return s->reg[offset / 4];
}

static void rt_sysctl0_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTSYSCTL0State *s = RT_SYSCTL0(opaque);

    s->reg[offset / 4] = value;
}

static const MemoryRegionOps rt_sysctl0_ops = {
    .read  = rt_sysctl0_read,
    .write = rt_sysctl0_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_sysctl0_realize(DeviceState *dev, Error **errp)
{
    RTSYSCTL0State *s = RT_SYSCTL0(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_sysctl0_ops, s,
                          TYPE_RT_SYSCTL0, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_sysctl0 = {
    .name = RT_SYSCTL0_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32_ARRAY(reg, RTSYSCTL0State, 512),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_sysctl0_init(Object *obj)
{
    RTSYSCTL0State *s = RT_SYSCTL0(obj);

    memory_region_init_io(&s->iomem, obj, &rt_sysctl0_ops, s, TYPE_RT_SYSCTL0, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_sysctl0_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_sysctl0_realize;
    dc->vmsd = &vmstate_rt_sysctl0;
}

static const TypeInfo rt_sysctl0_info = {
    .name          = TYPE_RT_SYSCTL0,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTSYSCTL0State),
    .instance_init = rt_sysctl0_init,
    .class_init    = rt_sysctl0_class_init,
};

static void rt_sysctl0_register_types(void)
{
    type_register_static(&rt_sysctl0_info);
}

type_init(rt_sysctl0_register_types)

static uint64_t rt_sysctl1_read(void *opaque, hwaddr offset, unsigned size)
{
    RTSYSCTL1State *s = RT_SYSCTL1(opaque);
     
    switch(offset) {
        default:
            break;
    }    

    return s->reg[offset / 4];
}

static void rt_sysctl1_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTSYSCTL1State *s = RT_SYSCTL1(opaque);

    s->reg[offset / 4] = value;
}

static const MemoryRegionOps rt_sysctl1_ops = {
    .read  = rt_sysctl1_read,
    .write = rt_sysctl1_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_sysctl1_realize(DeviceState *dev, Error **errp)
{
    RTSYSCTL1State *s = RT_SYSCTL1(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_sysctl1_ops, s,
                          TYPE_RT_SYSCTL1, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_sysctl1 = {
    .name = RT_SYSCTL1_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32_ARRAY(reg, RTSYSCTL1State, 512),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_sysctl1_init(Object *obj)
{
    RTSYSCTL1State *s = RT_SYSCTL1(obj);

    memory_region_init_io(&s->iomem, obj, &rt_sysctl1_ops, s, TYPE_RT_SYSCTL1, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_sysctl1_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_sysctl1_realize;
    dc->vmsd = &vmstate_rt_sysctl1;
}

static const TypeInfo rt_sysctl1_info = {
    .name          = TYPE_RT_SYSCTL1,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTSYSCTL1State),
    .instance_init = rt_sysctl1_init,
    .class_init    = rt_sysctl1_class_init,
};

static void rt_sysctl1_register_types(void)
{
    type_register_static(&rt_sysctl1_info);
}

type_init(rt_sysctl1_register_types)

