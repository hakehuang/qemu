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

#ifndef RT_OSTIMER_H
#define RT_OSTIMER_H

#include "hw/sysbus.h"
#include "hw/ptimer.h"
#include "hw/misc/imx_ccm.h"
#include "qom/object.h"

#define TYPE_RT_OSTIMER "rt-ostimer"


#define MATCH_WR_RDY_MASK      (1U<<2)
#define OSTIMER_INTENA_MASK    (1U<<1)
#define OSTIMER_INTRFLAG_MASK  (1U)

typedef struct RTOSTIMERState RTOSTIMERState;
DECLARE_INSTANCE_CHECKER(RTOSTIMERState, RT_OSTIMER,
                         TYPE_RT_OSTIMER)

struct RTOSTIMERState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    ptimer_state *timer;
    MemoryRegion  iomem;

    uint32_t EVTIMERL;
    uint32_t EVTIMERH;
    uint32_t CAPTURE_L;
    uint32_t CAPTURE_H;
    uint32_t MATCH_L;
    uint32_t MATCH_H;
    uint32_t OSEVENT_CTRL;

    uint32_t next_timeout;
    uint32_t next_int;

    uint32_t freq;

    qemu_irq irq;
};

#endif /* RT_OSTIMER_H */
