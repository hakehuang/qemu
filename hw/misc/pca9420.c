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
#include "hw/misc/pca9420.h"
#include "migration/vmstate.h"


static int pca9420_event(I2CSlave *i2c, enum i2c_event event)
{
    PCA9420State *s = PCA9420(i2c);

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

static uint8_t pca9420_recv(I2CSlave *i2c)
{
    PCA9420State *s = PCA9420(i2c);
    uint8_t res;

    res = s->reg[s->reg_idx];
    s->reg_idx++;

    return res;
}

static int pca9420_send(I2CSlave *i2c, uint8_t data)
{
    PCA9420State *s = PCA9420(i2c);

    if (s->addr_byte) {
        s->reg_idx = data;
        s->addr_byte = false;
        return 0;
    }

    s->reg[s->reg_idx] = data;
    s->reg_idx++;
    return 0;
}

static void pca9420_reset(DeviceState *dev)
{
    PCA9420State *s = PCA9420(dev);
    int i = 0;

    s->addr_byte = false;
    s->reg_idx = 0;
    for (i = 0; i < PCA9420_REG_SIZE; i++) {
        s->reg[i] = 0;
    }
    s->reg[3] = 0x3F;
    s->reg[5] = 0x7F;
    s->reg[7] = 0xFF;
    s->reg[9] = 0x41;

    s->reg[0xA] = 0x89;
    s->reg[0xB] = 0xAE;
    s->reg[0xC] = 0x1;
    s->reg[0x10] = 0x3;
    s->reg[0x11] = 0x8;
    s->reg[0x12] = 0x4;
    s->reg[0x13] = 0x8;
    s->reg[0x14] = 0x4;

    s->reg[0x15] = 0x1E;
    s->reg[0x16] = 0x95;
    s->reg[0x17] = 0x24;
    s->reg[0x18] = 0x10;

    s->reg[0x1A] = 0x78;
    s->reg[0x22] = 0x14;
    s->reg[0x23] = 0x0C;
    s->reg[0x24] = 0x4F;

    s->reg[0x25] = 0x39;
    s->reg[0x26] = 0x1C;
    s->reg[0x27] = 0x4C;
    s->reg[0x28] = 0x4F;
    s->reg[0x29] = 0x0C;
    s->reg[0x2A] = 0x1C;
    s->reg[0x2B] = 0x4C;
    s->reg[0x2C] = 0x4F;
    s->reg[0x2D] = 0x0C;
    s->reg[0x2E] = 0x1C;
    s->reg[0x2F] = 0x4C;
    s->reg[0x30] = 0x4F;
    s->reg[0x31] = 0x0C;
}

static const VMStateDescription vmstate_pca9420 = {
    .name = "pca9420",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_I2C_SLAVE(parent_obj, PCA9420State),
        VMSTATE_UINT8_ARRAY(reg, PCA9420State, PCA9420_REG_SIZE),
        VMSTATE_UINT32(reg_idx, PCA9420State),
        VMSTATE_BOOL(addr_byte, PCA9420State),
        VMSTATE_END_OF_LIST()
    }
};

static void pca9420_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);

    k->event = pca9420_event;
    k->recv = pca9420_recv;
    k->send = pca9420_send;
    dc->reset = pca9420_reset;
    dc->vmsd = &vmstate_pca9420;
}

static const TypeInfo pca9420_info = {
    .name          = TYPE_PCA9420,
    .parent        = TYPE_I2C_SLAVE,
    .instance_size = sizeof(PCA9420State),
    .class_init    = pca9420_class_init,
};

static void pca9420_register_types(void)
{
    type_register_static(&pca9420_info);
}

type_init(pca9420_register_types)
