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

#ifndef RT_CLKCTL_H
#define RT_CLKCTL_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_RT6XX_CLKCTL0 "rt6xx_clkctl0"
OBJECT_DECLARE_SIMPLE_TYPE(RT6XXCLKCTL0State, RT6XX_CLKCTL0)

struct RT6XXCLKCTL0State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t PSCCTL0 /* RW 0x10*/;
    uint32_t PSCCTL1 /* RW 0x14*/;
    uint32_t PSCCTL2 /* RW 0x18*/;
    uint32_t PSCCTL0_SET /* W 0x40*/;
    uint32_t PSCCTL1_SET /* W 0x44*/;
    uint32_t PSCCTL2_SET /* W 0x48*/;
    uint32_t PSCCTL0_CLR /* W 0x70*/;
    uint32_t PSCCTL1_CLR /* W 0x74*/;
    uint32_t PSCCTL2_CLR /* W 0x78*/;
    uint32_t FFROCTL0 /* RW 0x100*/;
    uint32_t FFROCTL1 /* RW 0x104*/;
    uint32_t SYSOSCCTL0 /* RW 0x160*/;
    uint32_t SYSOSCBYPASS /* RW 0x168*/;
    uint32_t LPOSCCTL0 /* RW 0x190*/;
    uint32_t OSC32KHZCTL0 /* RW 0x1C0*/;
    uint32_t SYSPLL0CLKSEL /* RW 0x200*/;
    uint32_t SYSPLL0CTL0 /* RW 0x204*/;
    uint32_t SYSPLL0LOCKTIMEDIV2 /* RW 0x20C*/;
    uint32_t SYSPLL0NUM /* RW 0x210*/;
    uint32_t SYSPLL0DENOM /* RW 0x214*/;
    uint32_t SYSPLL0PFD /* RW, W1 0x218*/;
    uint32_t MAINPLLCLKDIV /* RW 0x240*/;
    uint32_t DSPPLLCLKDIV /* RW 0x244*/;
    uint32_t AUX0PLLCLKDIV /* RW 0x248*/;
    uint32_t AUX1PLLCLKDIV /* RW 0x24C*/;
    uint32_t SYSCPUAHBCLKDIV /* RW 0x400*/;
    uint32_t MAINCLKSELA /* RW 0x430*/;
    uint32_t MAINCLKSELB /* RW 0x434*/;
    uint32_t PFC0DIV /* RW 0x500*/;
    uint32_t PFC1DIV /* RW 0x504*/;
    uint32_t FLEXSPIFCLKSEL /* RW 0x620*/;
    uint32_t FLEXSPIFCLKDIV /* RW 0x624*/;
    uint32_t SCTFCLKSEL /* RW 0x640*/;
    uint32_t SCTFCLKDIV /* RW 0x644*/;
    uint32_t USBHSFCLKSEL /* RW 0x660*/;
    uint32_t USBHSFCLKDIV /* RW 0x664*/;
    uint32_t SDIO0FCLKSEL /* RW 0x680*/;
    uint32_t SDIO0FCLKDIV /* RW 0x684*/;
    uint32_t SDIO1FCLKSEL /* RW 0x690*/;
    uint32_t SDIO1FCLKDIV /* RW 0x694*/;
    uint32_t ADC0FCLKSEL0 /* RW 0x6D0*/;
    uint32_t ADC0FCLKSEL1 /* RW 0x6D4*/;
    uint32_t ADC0FCLKDIV /* RW 0x6D8*/;
    uint32_t UTICKFCLKSEL /* RW 0x700*/;
    uint32_t WDT0FCLKSEL /* RW 0x720*/;
    uint32_t WAKECLK32KHZSEL /* RW 0x730*/;
    uint32_t WAKECLK32KHZDIV /* RW 0x734*/;
    uint32_t SYSTICKFCLKSEL /* RW 0x760*/;
    uint32_t SYSTICKFCLKDIV /* RW 0x764*/;

};

#define TYPE_RT6XX_CLKCTL1 "rt6xx_clkctl1"
OBJECT_DECLARE_SIMPLE_TYPE(RT6XXCLKCTL1State, RT6XX_CLKCTL1)

struct RT6XXCLKCTL1State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t PSCCTL0 /* RW 0x10*/;
    uint32_t PSCCTL1 /* RW 0x14*/;
    uint32_t PSCCTL2 /* RW 0x18*/;
    uint32_t PSCCTL0_SET /* W 0x40*/;
    uint32_t PSCCTL1_SET /* W 0x44*/;
    uint32_t PSCCTL2_SET /* W 0x48*/;
    uint32_t PSCCTL0_CLR /* W 0x70*/;
    uint32_t PSCCTL1_CLR /* W 0x74*/;
    uint32_t PSCCTL2_CLR /* W 0x78*/;
    uint32_t AUDIOPLL0CLKSEL /* RW 0x200*/;
    uint32_t AUDIOPLL0CTL0 /* RW 0x204*/;
    uint32_t AUDIOPLL0LOCKTIMEDIV2 /* RW 0x20C*/;
    uint32_t AUDIOPLL0NUM /* RW 0x210*/;
    uint32_t AUDIOPLL0DENOM /* RW 0x214*/;
    uint32_t AUDIOPLL0PFD /* RW, W1 0x218*/;
    uint32_t AUDIOPLLCLKDIV /* RW 0x240*/;
    uint32_t DSPCPUCLKDIV /* RW 0x400*/;
    uint32_t DSPMAINRAMCLKDIV /* RW 0x404*/;
    uint32_t DSPCPUCLKSELA /* RW 0x430*/;
    uint32_t DSPCPUCLKSELB /* RW 0x434*/;
    uint32_t OSEVENTFCLKSEL /* RW 0x480*/;
    uint32_t FRG0CLKSEL /* RW 0x500*/;
    uint32_t FRG0CTL /* RW 0x504*/;
    uint32_t FC0FCLKSEL /* RW 0x508*/;
    uint32_t FRG1CLKSEL /* RW 0x520*/;
    uint32_t FRG1CTL /* RW 0x524*/;
    uint32_t FC1FCLKSEL /* RW 0x528*/;
    uint32_t FRG2CLKSEL /* RW 0x540*/;
    uint32_t FRG2CTL /* RW 0x544*/;
    uint32_t FC2FCLKSEL /* RW 0x548*/;
    uint32_t FRG3CLKSEL /* RW 0x560*/;
    uint32_t FRG3CTL /* RW 0x564*/;
    uint32_t FC3FCLKSEL /* RW 0x568*/;
    uint32_t FRG4CLKSEL /* RW 0x580*/;
    uint32_t FRG4CTL /* RW 0x584*/;
    uint32_t FC4FCLKSEL /* RW 0x588*/;
    uint32_t FRG5CLKSEL /* RW 0x5A0*/;
    uint32_t FRG5CTL /* RW 0x5A4*/;
    uint32_t FC5FCLKSEL /* RW 0x5A8*/;
    uint32_t FRG6CLKSEL /* RW 0x5C0*/;
    uint32_t FRG6CTL /* RW 0x5C4*/;
    uint32_t FC6FCLKSEL /* RW 0x5C8*/;
    uint32_t FRG7CLKSEL /* RW 0x5E0*/;
    uint32_t FRG7CTL /* RW 0x5E4*/;
    uint32_t FC7FCLKSEL /* RW 0x5E8*/;
    uint32_t FRG14CLKSEL /* RW 0x6C0*/;
    uint32_t FRG14CTL /* RW 0x6C4*/;
    uint32_t FC14FCLKSEL /* RW 0x6C8*/;
    uint32_t FRG15CLKSEL /* RW 0x6E0*/;
    uint32_t FRG15CTL /* RW 0x6E4*/;
    uint32_t FC15FCLKSEL /* RW 0x6E8*/;
    uint32_t FRGPLLCLKDIV /* RW 0x6FC*/;
    uint32_t DMIC0FCLKSEL /* RW 0x700*/;
    uint32_t DMIC0FCLKDIV /* RW 0x704*/;
    uint32_t CT32BIT0FCLKSEL /* RW 0x720*/;
    uint32_t CT32BIT1FCLKSEL /* RW 0x724*/;
    uint32_t CT32BIT2FCLKSEL /* RW 0x728*/;
    uint32_t CT32BIT3FCLKSEL /* RW 0x72C*/;
    uint32_t CT32BIT4FCLKSEL /* RW 0x730*/;
    uint32_t AUDIOMCLKSEL /* RW 0x740*/;
    uint32_t AUDIOMCLKDIV /* RW 0x744*/;
    uint32_t CLKOUTSEL0 /* RW 0x760*/;
    uint32_t CLKOUTSEL1 /* RW 0x764*/;
    uint32_t CLKOUTDIV /* RW 0x768*/;
    uint32_t I3C0FCLKSEL /* RW 0x780*/;
    uint32_t I3C0FCLKSTCSEL /* RW 0x784*/;
    uint32_t I3C0FCLKSTCDIV /* RW 0x788*/;
    uint32_t I3C0FCLKSDIV /* RW 0x78C*/;
    uint32_t I3C0FCLKDIV /* RW 0x790*/;
    uint32_t WDT1FCLKSEL /* RW 0x7A0*/;
    uint32_t ACMP0FCLKSEL /* RW 0x7C0*/;
    uint32_t ACMP0FCLKDIV /* RW 0x7C4*/;

};

#endif /* RT_CLKCTL_H */
