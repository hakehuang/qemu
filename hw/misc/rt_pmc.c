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
#include "hw/misc/rt_pmc.h"
#include "migration/vmstate.h"


#define RT_PMC_NAME "PMC for NXP RTxxx series"

static uint64_t rt_pmc_read(void *opaque, hwaddr offset, unsigned size)
{
    RTPMCState *s = RT_PMC(opaque);
    uint32_t value= 0xFEFEFEFE;

    switch(offset) {
        case 0x4:
            /* always ok to set*/
            value = 0;
            break;
        case 0x8:
           value = s->FLAGS;
            break;
        case 0xC:
           value = s->CTRL;
            break;
        case 0x10:
            value = s->RUNCTRL;
            break;
        case 0x14:
            value = s->SLEEPCTRL;
            break;
        case 0x18:
            value = s->LVDCORECTRL;
            break;
        case 0x24:
            value = s->AUTOWKUP;
            break;
        case 0x28:
            value = s->PMICCFG;
            break;
        case 0x2C:
            value = s->PADVRANGE;
            break;
        case 0x30:
            value = s->MEMSEQCTRL;
            break;
        case 0x60:
            value = s->TSENSOR;
            break;
        default:
    }

    return value;
}

static void rt_pmc_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTPMCState *s = RT_PMC(opaque);
    uint32_t v32 = (uint32_t)value;

    switch(offset) {
        case 0x4:
            /* RO */
            break;
        case 0x8:
            s->FLAGS = v32;
            break;
        case 0xC:
            s->CTRL = v32;
            break;
        case 0x10:
            s->RUNCTRL = v32;
            break;
        case 0x14:
            s->SLEEPCTRL = v32;
            break;
        case 0x18:
            s->LVDCORECTRL = v32;
            break;
        case 0x24:
            s->AUTOWKUP = v32;
            break;
        case 0x28:
            s->PMICCFG = v32;
            break;
        case 0x2C:
            s->PADVRANGE = v32;
            break;
        case 0x30:
            s->MEMSEQCTRL = v32;
            break;
        case 0x60:
            s->TSENSOR = v32;
            break;
        default:
    }

    return;
}

static void rt_pmc_reset_at_boot(DeviceState *dev)
{
    RTPMCState *s = RT_PMC(dev);

    s->STATUS = 0x0;
    s->FLAGS = 0x00170000;
    s->CTRL = 0x00200000;
    s->RUNCTRL = 0x00000026;
    s->SLEEPCTRL = 0x00000026;
    s->LVDCORECTRL = 0x0;
    s->AUTOWKUP = 0x0;
    s->PMICCFG = 0x00000073;
    s->PADVRANGE = 0x0;
    s->MEMSEQCTRL = 0x3F;
    s->TSENSOR = 0x0;
}

static const MemoryRegionOps rt_pmc_ops = {
    .read  = rt_pmc_read,
    .write = rt_pmc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_pmc_realize(DeviceState *dev, Error **errp)
{
    RTPMCState *s = RT_PMC(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_pmc_ops, s,
                          TYPE_RT_PMC, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_pmc = {
    .name = RT_PMC_NAME,
    .version_id = 1,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(STATUS, RTPMCState),
        VMSTATE_UINT32(FLAGS, RTPMCState),
        VMSTATE_UINT32(CTRL, RTPMCState),
        VMSTATE_UINT32(RUNCTRL, RTPMCState),
        VMSTATE_UINT32(SLEEPCTRL, RTPMCState),
        VMSTATE_UINT32(LVDCORECTRL, RTPMCState),
        VMSTATE_UINT32(AUTOWKUP, RTPMCState),
        VMSTATE_UINT32(PMICCFG, RTPMCState),
        VMSTATE_UINT32(PADVRANGE, RTPMCState),
        VMSTATE_UINT32(MEMSEQCTRL, RTPMCState),
        VMSTATE_UINT32(TSENSOR, RTPMCState),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_pmc_init(Object *obj)
{
    RTPMCState *s = RT_PMC(obj);

    memory_region_init_io(&s->iomem, obj, &rt_pmc_ops, s, TYPE_RT_PMC, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_pmc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_pmc_realize;
    dc->vmsd = &vmstate_rt_pmc;
    dc->reset = rt_pmc_reset_at_boot;
}

static const TypeInfo rt_pmc_info = {
    .name          = TYPE_RT_PMC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTPMCState),
    .instance_init = rt_pmc_init,
    .class_init    = rt_pmc_class_init,
};

static void rt_pmc_register_types(void)
{
    type_register_static(&rt_pmc_info);
}

type_init(rt_pmc_register_types)

