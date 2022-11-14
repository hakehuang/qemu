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
#include "hw/misc/rt_rstctl.h"
#include "migration/vmstate.h"


#define RT_RSTCTL0_NAME "reset controllor0 for NXP RTxxx series"
#define RT_RSTCTL1_NAME "reset controllor1 for NXP RTxxx series"

static uint64_t rt_rstctl0_read(void *opaque, hwaddr offset, unsigned size)
{
    RTRSTCTL0State *s = RT_RSTCTL0(opaque);
    uint32_t value;
     
    switch(offset) {
        case 0x10: /* PRSTCTL0 */
            value = s->PRSTCTL0;
            break;
        case 0x14: /* PRSTCTL1 */
            value = s->PRSTCTL1;
            break;
        case 0x18: /* PRSTCTL2 */
            value = s->PRSTCTL2;
            break;
        default:
            break;
    }
    printf("[RSTCTL0]: read 0x%lx = 0x%x\n", offset, value);

    return value;
}

static void rt_rstctl0_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTRSTCTL0State *s = RT_RSTCTL0(opaque);

    switch(offset)
    {
        case 0x0:
            s->SYSRSTSTAT &= ~value; 
            break;
        case 0x10:
            s->PRSTCTL0 = value;
            break;
        case 0x14:
            s->PRSTCTL1 = value;
            break;
        case 0x18:
            s->PRSTCTL2 = value;
            break;
        case 0x40:
            s->PRSTCTL0 |= value;
            break;
        case 0x44:
             s->PRSTCTL1 |= value;
            break;
        case 0x48:
             s->PRSTCTL2 |= value;
            break;
        case 0x70:
            s->PRSTCTL0 &= ~value;
            break;
        case 0x74:
            s->PRSTCTL1 &= ~value;
            break;
        case 0x78:
            s->PRSTCTL2 &= ~value;
            break;
        default:
            return;
    } 
}

static const MemoryRegionOps rt_rstctl0_ops = {
    .read  = rt_rstctl0_read,
    .write = rt_rstctl0_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_rstctl0_realize(DeviceState *dev, Error **errp)
{
    RTRSTCTL0State *s = RT_RSTCTL0(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_rstctl0_ops, s,
                          TYPE_RT_RSTCTL0, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static void rt_rstctl0_reset_at_boot(DeviceState *dev)
{
    RTRSTCTL0State *s = RT_RSTCTL0(dev);

    s->SYSRSTSTAT = 0x1;
    s->PRSTCTL0 = 0x7DF51F0A;
    s->PRSTCTL1 = 0x0101800C;
    s->PRSTCTL2 = 0x1C000001;
    s->PRSTCTL0_SET = 0x0;
    s->PRSTCTL1_SET = 0x0;
    s->PRSTCTL2_SET = 0x0;
    s->PRSTCTL0_CLR = 0x0;
    s->PRSTCTL1_CLR = 0x0;
    s->PRSTCTL2_CLR = 0x0;
}

static const VMStateDescription vmstate_rt_rstctl0 = {
    .name = RT_RSTCTL0_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(SYSRSTSTAT, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL0, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL1, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL2, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL0_SET, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL1_SET, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL2_SET, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL0_CLR, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL1_CLR, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL2_CLR, RTRSTCTL0State),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_rstctl0_init(Object *obj)
{
    RTRSTCTL0State *s = RT_RSTCTL0(obj);

    memory_region_init_io(&s->iomem, obj, &rt_rstctl0_ops, s, TYPE_RT_RSTCTL0, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_rstctl0_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_rstctl0_realize;
    dc->vmsd = &vmstate_rt_rstctl0;
    dc->reset = rt_rstctl0_reset_at_boot;
}

static const TypeInfo rt_rstctl0_info = {
    .name          = TYPE_RT_RSTCTL0,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTRSTCTL0State),
    .instance_init = rt_rstctl0_init,
    .class_init    = rt_rstctl0_class_init,
};

static void rt_rstctl0_register_types(void)
{
    type_register_static(&rt_rstctl0_info);
}

type_init(rt_rstctl0_register_types)

static uint64_t rt_rstctl1_read(void *opaque, hwaddr offset, unsigned size)
{
    RTRSTCTL1State *s = RT_RSTCTL1(opaque);
    uint32_t value;

    switch(offset) {
        case 0x10: /* PRSTCTL0 */
            value = s->PRSTCTL0;
            break;
        case 0x14: /* PRSTCTL1 */
            value = s->PRSTCTL1;
            break;
        case 0x18: /* PRSTCTL2 */
            value = s->PRSTCTL2;
            break;
        default:
            break;
    }
    printf("[RSTCTL1]: read 0x%lx = 0x%x\n", offset, value);

    return value;
}

static void rt_rstctl1_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTRSTCTL1State *s = RT_RSTCTL1(opaque);

    switch(offset)
    {
        case 0x0:
            s->SYSRSTSTAT &= ~value; 
            break;
        case 0x10:
            s->PRSTCTL0 = value;
            break;
        case 0x14:
            s->PRSTCTL1 = value;
            break;
        case 0x18:
            s->PRSTCTL2 = value;
            break;
        case 0x40:
            s->PRSTCTL0 |= value;
            break;
        case 0x44:
             s->PRSTCTL1 |= value;
            break;
        case 0x48:
             s->PRSTCTL2 |= value;
            break;
        case 0x70:
            s->PRSTCTL0 &= ~value;
            break;
        case 0x74:
            s->PRSTCTL1 &= ~value;
            break;
        case 0x78:
            s->PRSTCTL2 &= ~value;
            break;
        default:
            return;
    } 
}

static const MemoryRegionOps rt_rstctl1_ops = {
    .read  = rt_rstctl1_read,
    .write = rt_rstctl1_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_rstctl1_realize(DeviceState *dev, Error **errp)
{
    RTRSTCTL1State *s = RT_RSTCTL1(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_rstctl1_ops, s,
                          TYPE_RT_RSTCTL1, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_rstctl1 = {
    .name = RT_RSTCTL1_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(SYSRSTSTAT, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL0, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL1, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL2, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL0_SET, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL1_SET, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL2_SET, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL0_CLR, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL1_CLR, RTRSTCTL0State),
        VMSTATE_UINT32(PRSTCTL2_CLR, RTRSTCTL0State),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_rstctl1_init(Object *obj)
{
    RTRSTCTL1State *s = RT_RSTCTL1(obj);

    memory_region_init_io(&s->iomem, obj, &rt_rstctl1_ops, s, TYPE_RT_RSTCTL1, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_rstctl1_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_rstctl1_realize;
    dc->vmsd = &vmstate_rt_rstctl1;
}

static const TypeInfo rt_rstctl1_info = {
    .name          = TYPE_RT_RSTCTL1,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTRSTCTL1State),
    .instance_init = rt_rstctl1_init,
    .class_init    = rt_rstctl1_class_init,
};

static void rt_rstctl1_register_types(void)
{
    type_register_static(&rt_rstctl1_info);
}

type_init(rt_rstctl1_register_types)

