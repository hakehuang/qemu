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

#ifndef FXOS8700_H
#define FXOS8700_H

#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/i2c/i2c.h"

#define TYPE_FXOS8700 "FXOS8700"
OBJECT_DECLARE_SIMPLE_TYPE(FXOS8700State, FXOS8700)

#define FXOS8700_REG_SIZE 128U

struct FXOS8700State {
    I2CSlave parent_obj;
    uint32_t reg_idx;
    uint8_t reg[FXOS8700_REG_SIZE];
    bool addr_byte;
};

#endif /* FXOS8700_H */
