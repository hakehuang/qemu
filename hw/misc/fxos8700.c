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
#include "hw/misc/fxos8700.h"
#include "migration/vmstate.h"


static int fxos8700_event(I2CSlave *i2c, enum i2c_event event)
{
    FXOS8700State *s = FXOS8700(i2c);

    switch (event) {
    case I2C_START_RECV:
        break;
    case I2C_START_SEND:
        s->addr_byte = true;
        break;
    default:
        break;
    }

    return 0;
}

static uint8_t fxos8700_recv(I2CSlave *i2c)
{
    FXOS8700State *s = FXOS8700(i2c);
    uint8_t res;

    res = s->reg[s->reg_idx];
    s->reg_idx++;

    return res;
}

static int fxos8700_send(I2CSlave *i2c, uint8_t data)
{
    FXOS8700State *s = FXOS8700(i2c);

    if (s->addr_byte) {
        s->reg_idx = data;
        s->addr_byte = false;
        return 0;
    }

    s->reg[s->reg_idx] = data;
    s->reg_idx++;
    return 0;
}

static void fxos8700_reset(DeviceState *dev)
{
    FXOS8700State *s = FXOS8700(dev);
    int i = 0;

    s->reg_idx = 0;
    for (i = 0; i < FXOS8700_REG_SIZE; i++) {
        s->reg[i] = 0;
    }
    /* STATUS register */
    s->reg[0] = 0xFF; /* data is always ready */

    /* FAKE DATA */
    s->reg[1] = 0x5F; /* 8 MSBs of 14-bit sample */
    s->reg[2] = 0x1F; /* 6 LSBs of 14-bit real-time sample */
    s->reg[3] = 0x5F; /* 8 MSBs */
    s->reg[4] = 0x1F; /* 6 LSBS */
    s->reg[5] = 0x5F; /* 8 MSBs */
    s->reg[6] = 0x1F; /* 6 LSBs */
    s->reg[0x0D] = 0xC7; /* WHO_AMI_I */
    s->reg[0x11] = 0x80; /* Landscape/portrait configuration */
    s->reg[0x13] = 0x84; /* Back/front trip angle threshold */
    s->reg[0x14] = 0x44; /* Portrait to landscape trip threshold angle and hysteresis settings */
    /* magnetic data */
    s->reg[0x33] = 0x55;
    s->reg[0x34] = 0x55;
    s->reg[0x35] = 0x55;
    s->reg[0x36] = 0x55;
    s->reg[0x37] = 0x55;
    s->reg[0x38] = 0x55;
    /* accerleration data */
    s->reg[0x39] = 0x55;
    s->reg[0x3A] = 0x55;
    s->reg[0x3B] = 0x55;
    s->reg[0x3C] = 0x55;
    s->reg[0x3D] = 0x55;
    s->reg[0x3E] = 0x55;
    /* Magnetometer */
    s->reg[0x45] = 0x55;
    s->reg[0x46] = 0x55;
    s->reg[0x47] = 0x55;
    s->reg[0x48] = 0x55;
    s->reg[0x49] = 0x55;
    s->reg[0x4A] = 0x55;
    s->reg[0x4B] = 0x55;
    s->reg[0x4C] = 0x55;
    s->reg[0x4D] = 0x55;
    s->reg[0x4E] = 0x55;
    s->reg[0x4F] = 0x55;
    s->reg[0x50] = 0x55;

}

static const VMStateDescription vmstate_fxos8700 = {
    .name = "fxos8700",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_I2C_SLAVE(parent_obj, FXOS8700State),
        VMSTATE_UINT8_ARRAY(reg, FXOS8700State, FXOS8700_REG_SIZE),
        VMSTATE_UINT32(reg_idx, FXOS8700State),
        VMSTATE_BOOL(addr_byte, FXOS8700State),
        VMSTATE_END_OF_LIST()
    }
};

static void fxos8700_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);

    k->event = fxos8700_event;
    k->recv = fxos8700_recv;
    k->send = fxos8700_send;
    dc->reset = fxos8700_reset;
    dc->vmsd = &vmstate_fxos8700;
}

static const TypeInfo fxos8700_info = {
    .name          = TYPE_FXOS8700,
    .parent        = TYPE_I2C_SLAVE,
    .instance_size = sizeof(FXOS8700State),
    .class_init    = fxos8700_class_init,
};

static void fxos8700_register_types(void)
{
    type_register_static(&fxos8700_info);
}

type_init(fxos8700_register_types)
