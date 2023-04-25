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

#ifndef RT_PINT_H
#define RT_PINT_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_RT_PINT "RT_PINT"
OBJECT_DECLARE_SIMPLE_TYPE(RTPINTState, RT_PINT)

struct RTPINTState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;
   
    uint32_t ISEL;   /* 0x0 */
    uint32_t IENR;   /* 0x4 */
    uint32_t SIENR;  /* 0x8 */
    uint32_t CIENR;  /* 0xc */
    uint32_t IENF;   /* 0x10 */
    uint32_t SIENF;  /* 0x14 */
    uint32_t CIENF;  /* 0x18 */
    uint32_t RISE;   /* 0x1c */
    uint32_t FALL;   /* 0x20 */
    uint32_t IST;    /* 0x24 */
    uint32_t PMCTRL; /* 0x28 */
    uint32_t PMSRC;  /* 0x2c */
    uint32_t PMCFG;  /* 0x30 */

    qemu_irq irq[8];

};

#endif /* RT_PINT_H */
