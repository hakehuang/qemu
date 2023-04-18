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

#ifndef RT_PMC_H
#define RT_PMC_H

#include "hw/sysbus.h"
#include "qom/object.h"
#include "qemu/timer.h"

#define TYPE_RT_PMC "rt_pmc"
OBJECT_DECLARE_SIMPLE_TYPE(RTPMCState, RT_PMC)

#define FLAG_AUTOWKF_MASK (1U<<28)
#define AUTOWKUP_AUTOWKTIME 0xFFFF
#define CTRL_AUTOWKF_MASK (1U<<28)

struct RTPMCState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t STATUS;
    uint32_t FLAGS;
    uint32_t CTRL;
    uint32_t RUNCTRL;
    uint32_t SLEEPCTRL;
    uint32_t LVDCORECTRL;
    uint32_t AUTOWKUP;
    uint32_t PMICCFG;
    uint32_t PADVRANGE;
    uint32_t MEMSEQCTRL;
    uint32_t TSENSOR;

    uint32_t timer_interval;
    QEMUTimer *timer;
    uint32_t state;

    uint32_t count;

    qemu_irq irq;
};

typedef enum { 
    ePMC_Active = 11000,
    ePMC_Sleep  = 12000,
    ePMC_Deep_Sleep = 13000,
    ePMC_Deep_Power_Down = 14000,
    ePMC_Full_Deep_Power_Down = 15000,
} ePMState;

#endif /* RT_PMC_H */
