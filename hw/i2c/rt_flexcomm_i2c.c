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
#include "hw/i2c/i2c.h"
#include "hw/i2c/rt_flexcomm_i2c.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"

/*
  TODO I2C slave mode is not implement
*/

static const VMStateDescription vmstate_rt_flexcomm = {
    .name = TYPE_RT_FLEXCOMMI2C,
    .version_id = 1,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(ID, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_CFG, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_STAT, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_INTENSET, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_INTENCLR, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_TIMEOUT, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_CLKDIV, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_INTSTAT, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_MSTCTL, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_MSTTIME, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_MSTDAT, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_SLVCTL, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_SLVDAT, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_SLVADR0, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_SLVADR1, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_SLVADR2, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_SLVADR3, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_SLVQUAL0, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(I2C_MONRXDAT, RTFLEXCOMMI2CState),
        VMSTATE_UINT32(PSELID, RTFLEXCOMMI2CState),
        VMSTATE_END_OF_LIST()
    },
};

static void rt_flexcomm_update(RTFLEXCOMMI2CState *s)
{

    uint32_t psel = s->PSELID & PERSEL_MASK;

    switch (psel) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            s->PSELID |= I2CPRESENT_MASK;
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
    }

    if (((s->I2C_CFG & 0x7) != 0) && s->I2C_INTENSET)
    {
        if ((s->I2C_STAT & s->I2C_INTENSET) != 0) {
            qemu_irq_raise(s->irq);
            qemu_log("flexcomm raise irq 0x%x\n", s->I2C_STAT);
        } else {
            qemu_irq_lower(s->irq);
        }
    } else {
        qemu_irq_lower(s->irq);
    }

}

static void rt_flexcomm_reset(RTFLEXCOMMI2CState *s)
{

    s->ID = 0x0FFC0000;
    s->I2C_CFG = 0;
    s->I2C_STAT = 0x801;
    s->I2C_INTENSET = 0x0;
    s->I2C_INTENCLR = 0x0;
    s->I2C_TIMEOUT  = 0xFFFF;
    s->I2C_CLKDIV   = 0x0;
    s->I2C_INTSTAT  = 0x801;
    s->I2C_MSTCTL   = 0x0;
    s->I2C_MSTTIME  = 0x77;
    s->I2C_MSTDAT   = 0x0;
    s->I2C_SLVCTL   = 0x0;
    s->I2C_SLVDAT   = 0x0;
    s->I2C_SLVADR0  = 0x1;
    s->I2C_SLVADR1  = 0x1;
    s->I2C_SLVADR2  = 0x1;
    s->I2C_SLVADR3  = 0x1;
    s->I2C_SLVQUAL0 = 0x0;
    s->I2C_MONRXDAT = 0x0;
    s->PSELID = 0x1000;
    
}

static void rt_flexcomm_reset_at_boot(DeviceState *dev)
{
    RTFLEXCOMMI2CState *s = RT_FLEXCOMMI2C(dev);

    rt_flexcomm_reset(s);
}

static uint64_t rt_flexcomm_i2c_read(void *opaque, hwaddr offset,
                                unsigned size)
{
    RTFLEXCOMMI2CState *s = (RTFLEXCOMMI2CState *)opaque;
    uint32_t v = 0;

    switch (offset) {
        case 0x800:
            v = s->I2C_CFG;
            break;
        case 0x804:
            v = s->I2C_STAT;
            break;
        case 0x808:
            v = s->I2C_INTENSET;
            break;
        case 0x80C:
            /* Write only */
            break;
        case 0x810: 
            v = s->I2C_TIMEOUT;
            break;
        case 0x814:
            v = s->I2C_CLKDIV;
            break;
        case 0x818:
            v = s->I2C_INTSTAT;
            break;
        case 0x820:
            v = s->I2C_MSTCTL;
            break;
        case 0x824:
            v = s->I2C_MSTTIME;
            break;
        case 0x828:
            v = s->I2C_MSTDAT;
            break;
        case 0x840:
            v = s->I2C_SLVCTL;
            break;
        case 0x844:
            v = s->I2C_SLVDAT;
            break;
        case 0x848:
            v = s->I2C_SLVADR0;
            break;
        case 0x84C:
            v = s->I2C_SLVADR1;
            break;
        case 0x850:
            v = s->I2C_SLVADR2;
            break;
        case 0x854:
            v = s->I2C_SLVADR3;
            break;
        case 0x858:
            v = s->I2C_SLVQUAL0;
            break;
        case 0x880: 
            v = s->I2C_MONRXDAT;
            break;
        case 0xFF8: 
            v = s->PSELID;
            break;
        default:
            break;
    }

    //printf("[FLEXCOMM_I2C]: read 0x%lx = 0x%x\n", offset, v);
    return v;
}

static uint64_t rt_flexcomm_read(void *opaque, hwaddr offset,
                                unsigned size)
{
    return rt_flexcomm_i2c_read(opaque, offset, size);
}

static void rt_flexcomm_i2c_write(void *opaque, hwaddr offset,
                             uint64_t value, unsigned size)
{
    RTFLEXCOMMI2CState *s = (RTFLEXCOMMI2CState *)opaque;
    uint32_t v32 = (uint32_t)value;

    //printf("[FLEXCOMM_I2C]: write 0x%lx = 0x%lx\n", offset, value);
    switch (offset) {
        case 0x800:
            s->I2C_CFG = v32;
            if ((v32 & MSTEN_MASK) == MSTEN_MASK){
                /* set the pending bit */
                s->I2C_STAT |= MSTPENDING_MASK;
            } else if((v32 & SLVEN_MASK) == SLVEN_MASK) {
                /* set the pending bit */
                s->I2C_STAT |= SLVPENDING_MASK;
            }
            break;
        case 0x804:
            s->I2C_STAT &= ~v32;
            break;
        case 0x808:
            s->I2C_INTENSET = v32;
            break;
        case 0x80C:
            s->I2C_INTENSET &= ~v32; /* WOC */
            break;
        case 0x810: 
            s->I2C_TIMEOUT = v32;
            break;
        case 0x814:
            s->I2C_CLKDIV = v32;
            break;
        case 0x818:
            break;
        case 0x820:
            s->I2C_MSTCTL = v32 & 0x80;
            if ((v32 & MSTSTART_MASK) == MSTSTART_MASK)
            {
                /* clear the pending status */
                s->I2C_STAT &= ~MSTPENDING_MASK;
                if (i2c_start_transfer(s->bus, s->I2C_MSTDAT, s->I2C_MSTDAT & 0x1))
                {
                    /* invalide address */
                    uint32_t status = s->I2C_STAT;

                    s->I2C_STAT = deposit32(status, MSTSTATE_OFFSET, MSTSTATE_LENGTH, MST_NAK_ADDR);
                    i2c_end_transfer(s->bus);

                } else {
                    if ((s->I2C_MSTDAT & 0x1) == 0x0) {
                        /* master recv*/
                        s->I2C_MSTDAT = i2c_recv(s->bus);
                    }
                }
                /* set the pending bit */
                s->I2C_STAT |= MSTPENDING_MASK;
            } else if ((v32 & MSTCONTINUE_MASK) == MSTCONTINUE_MASK) {
                /* clear the pending status */
                s->I2C_STAT &= ~MSTPENDING_MASK;
                s->I2C_STAT &= ~(MSTARBLOSS_MASK|MSTSTSTPERR_MASK);
                if (i2c_send(s->bus, s->I2C_MSTDAT))
                {
                    /* no ack from slave */
                    uint32_t status = s->I2C_STAT;

                    s->I2C_STAT = deposit32(status, MSTSTATE_OFFSET, MSTSTATE_LENGTH, MST_NAK_DATA);
                    i2c_end_transfer(s->bus);                 
                }
                /* set the pending bit */
                s->I2C_STAT |= MSTPENDING_MASK;
            } else if ((v32 & MSTSTOP_MASK) == MSTSTOP_MASK){
                i2c_end_transfer(s->bus);
            }
            break;
        case 0x824:
            s->I2C_MSTTIME = v32;
            break;
        case 0x828:
            s->I2C_MSTDAT = v32;
            break;
        case 0x840:
            s->I2C_SLVCTL = v32;

            break;
        case 0x844:
            s->I2C_SLVDAT = v32;
            break;
        case 0x848:
            s->I2C_SLVADR0 = v32;
            break;
        case 0x84C:
            s->I2C_SLVADR1 = v32;
            break;
        case 0x850:
            s->I2C_SLVADR2 = v32;
            break;
        case 0x854:
            s->I2C_SLVADR3 = v32;
            break;
        case 0x858:
            s->I2C_SLVQUAL0 = v32;
            break;
        case 0x880:
            break;
        case 0xFF8:
            s->PSELID = value;
            break;
        default:
            return;
    }
}

static void rt_flexcomm_write(void *opaque, hwaddr offset,
                             uint64_t value, unsigned size)
{
    RTFLEXCOMMI2CState *s = (RTFLEXCOMMI2CState *)opaque;

    rt_flexcomm_i2c_write(opaque, offset, value, size);

    rt_flexcomm_update(s);
}

static const struct MemoryRegionOps rt_flexcomm_ops = {
    .read = rt_flexcomm_read,
    .write = rt_flexcomm_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_flexcomm_realize(DeviceState *dev, Error **errp)
{
    RTFLEXCOMMI2CState *s = RT_FLEXCOMMI2C(dev);

    s->bus = i2c_init_bus(dev, "i2c-bus");
}

static void rt_flexcomm_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RTFLEXCOMMI2CState *s = RT_FLEXCOMMI2C(obj);

    memory_region_init_io(&s->iomem, obj, &rt_flexcomm_ops, s,
                          TYPE_RT_FLEXCOMMI2C, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
}

static Property rt_flexcomm_i2c_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void rt_flexcomm_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_flexcomm_realize;
    dc->vmsd = &vmstate_rt_flexcomm;
    dc->reset = rt_flexcomm_reset_at_boot;
    set_bit(DEVICE_CATEGORY_INPUT, dc->categories);
    dc->desc = "RT FLEXCOMM I2C";
    device_class_set_props(dc, rt_flexcomm_i2c_properties);
}

static const TypeInfo rt_flexcomm_info = {
    .name           = TYPE_RT_FLEXCOMMI2C,
    .parent         = TYPE_SYS_BUS_DEVICE,
    .instance_size  = sizeof(RTFLEXCOMMI2CState),
    .instance_init  = rt_flexcomm_init,
    .class_init     = rt_flexcomm_class_init,
};

static void rt_flexcomm_register_types(void)
{
    type_register_static(&rt_flexcomm_info);
}

type_init(rt_flexcomm_register_types)
