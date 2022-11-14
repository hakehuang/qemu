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

#ifndef RT_FLEXCOMM_I2C_H
#define RT_FLEXCOMM_I2C_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"
#include "qemu/bitops.h"
#include "hw/i2c/i2c.h"

#define TYPE_RT_FLEXCOMMI2C "rt.flexcomm.i2c"
OBJECT_DECLARE_SIMPLE_TYPE(RTFLEXCOMMI2CState, RT_FLEXCOMMI2C)

enum {
    MST_IDLE = 0,
    MST_RCV_RD,
    MST_TRS_RD,
    MST_NAK_ADDR,
    MST_NAK_DATA,
};

#define PERSEL_MASK 0x7
#define I2CPRESENT_MASK (1U<<6)

/* I2C_CFG */
#define SLVEN_OFFSET 1U
#define SLVEN_LENGTH 1U
#define SLVEN_MASK   (((1<<SLVEN_LENGTH) - 1)<<SLVEN_OFFSET)

#define MSTEN_OFFSET 0U
#define MSTEN_LENGTH 1U
#define MSTEN_MASK   (((1<<MSTEN_LENGTH) - 1)<<MSTEN_OFFSET)


/* I2C_STAT */
#define MSTSTATE_OFFSET 1U
#define MSTSTATE_LENGTH 2U
#define MSTSTATE_MASK (((1<<MSTSTATE_LENGTH) - 1)<<MSTSTATE_OFFSET)

#define MSTARBLOSS_OFFSET 4U
#define MSTARBLOSS_LENGTH 1U
#define MSTARBLOSS_MASK (((1<<MSTARBLOSS_LENGTH) - 1)<<MSTARBLOSS_OFFSET)

#define MSTSTSTPERR_OFFSET 6U
#define MSTSTSTPERR_LENGTH 1U
#define MSTSTSTPERR_MASK (((1<<MSTSTSTPERR_LENGTH) - 1)<<MSTSTSTPERR_OFFSET)

#define SLVPENDING_MASK 0x100
#define MSTPENDING_MASK 0x1

/* I2C_MSTCTL */
#define MSTSTOP_MASK 0x4
#define MSTSTART_MASK 0x2
#define MSTCONTINUE_MASK 0x1


struct RTFLEXCOMMI2CState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion iomem;
    uint32_t ID;

    uint32_t I2C_CFG;
    uint32_t I2C_STAT;
    uint32_t I2C_INTENSET;
    uint32_t I2C_INTENCLR;
    uint32_t I2C_TIMEOUT;
    uint32_t I2C_CLKDIV;
    uint32_t I2C_INTSTAT;
    uint32_t I2C_MSTCTL;
    uint32_t I2C_MSTTIME;
    uint32_t I2C_MSTDAT;
    uint32_t I2C_SLVCTL;
    uint32_t I2C_SLVDAT;
    uint32_t I2C_SLVADR0;
    uint32_t I2C_SLVADR1;
    uint32_t I2C_SLVADR2;
    uint32_t I2C_SLVADR3;
    uint32_t I2C_SLVQUAL0;
    uint32_t I2C_MONRXDAT;
    uint32_t PSELID;

    I2CBus *bus;

    qemu_irq irq;
};

#endif
