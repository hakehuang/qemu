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

#ifndef PCA9420_H
#define PCA9420_H

#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/i2c/i2c.h"

#define TYPE_PCA9420 "PCA9420"
OBJECT_DECLARE_SIMPLE_TYPE(PCA9420State, PCA9420)

#define PCA9420_REG_SIZE 0x32U

struct PCA9420State {
    I2CSlave parent_obj;
    uint32_t reg_idx;
    uint8_t reg[PCA9420_REG_SIZE];
    bool addr_byte;
};

#endif /* PCA9420_H */
