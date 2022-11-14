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

#ifndef RT_SYSCTL_H
#define RT_SYSCTL_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_RT_SYSCTL0 "rt_sysctl0"
OBJECT_DECLARE_SIMPLE_TYPE(RTSYSCTL0State, RT_SYSCTL0)

struct RTSYSCTL0State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t reg[512];
};

#define TYPE_RT_SYSCTL1 "rt_sysctl1"
OBJECT_DECLARE_SIMPLE_TYPE(RTSYSCTL1State, RT_SYSCTL1)

struct RTSYSCTL1State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t reg[512];
};

#endif /* RT_SYSCTL_H */
