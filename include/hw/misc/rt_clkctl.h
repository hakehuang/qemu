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

#ifndef RT_CLKCTL_H
#define RT_CLKCTL_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_RT_CLKCTL0 "rt_clkctl0"
OBJECT_DECLARE_SIMPLE_TYPE(RTCLKCTL0State, RT_CLKCTL0)

struct RTCLKCTL0State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;
   
    uint32_t PSCCTL0; /* 0x10 */
    uint32_t PSCCTL1; /* 0x14 */
    uint32_t PSCCTL2; /* 0x18 */
    uint32_t PSCCTL0_SET; /* 0x40 */
    uint32_t PSCCTL1_SET; /* 0x44 */
    uint32_t PSCCTL2_SET; /* 0x48 */
    uint32_t PSCCTL0_CLR; /* 0x70 */
    uint32_t PSCCTL1_CLR; /* 0x74 */
    uint32_t PSCCTL2_CLR; /* 0x78 */
    uint32_t FRO_CONTROL; /* 0x80 */
    uint32_t FRO_CAPVAL; /* 0x84 */
    uint32_t FRO_RDTRIM; /* 0x8C */
    uint32_t FRO_SCTRIM; /* 0x90 */
    uint32_t FRODIVSEL; /* 0x108 */
    uint32_t FROCLKSTATUS; /* 0x10C */
    uint32_t FRODIVOEN; /* 0x110 */
    uint32_t LOWFREQCLKDIV; /* 0x130 */
    uint32_t SYSOSCCTL0; /* 0x160 */
    uint32_t SYSOSCBYPASS; /* 0x168 */
    uint32_t LPOSCCTL0; /* 0x190 */
    uint32_t OSC32KHZCTL0; /* 0x1C0 */
    uint32_t SYSPLL0CLKSEL; /* 0x200 */
    uint32_t SYSPLL0CTL0; /* 0x204 */
    uint32_t SYSPLL0LOCKTIMEDIV2; /* 0x20C */
    uint32_t SYSPLL0NUM; /* 0x210 */
    uint32_t SYSPLL0DENOM; /* 0x214 */
    uint32_t SYSPLL0PFD; /* 0x218 */
    uint32_t MAINPLLCLKDIV; /* 0x240 */
    uint32_t DSPPLLCLKDIV; /* 0x244 */
    uint32_t AUX0PLLCLKDIV; /* 0x248 */
    uint32_t AUX1PLLCLKDIV; /* 0x24C */
    uint32_t SYSCPUAHBCLKDIV; /* 0x400 */
    uint32_t MAINCLKSELA; /* 0x430 */
    uint32_t MAINCLKSELB; /* 0x434 */
    uint32_t PFC0DIV; /* 0x500 */
    uint32_t PFC1DIV; /* 0x504 */
    uint32_t FLEXSPI0FCLKSEL; /* 0x620 */
    uint32_t FLEXSPI0FCLKDIV; /* 0x624 */
    uint32_t FLEXSPI1FCLKSEL; /* 0x630 */
    uint32_t FLEXSPI1FCLKDIV; /* 0x634 */
    uint32_t SCTFCLKSEL; /* 0x640 */
    uint32_t SCTIN7CLKDIV; /* 0x644 */
    uint32_t USBHSFCLKSEL; /* 0x660 */
    uint32_t USBHSFCLKDIV; /* 0x664 */
    uint32_t SDIO0FCLKSEL; /* 0x680 */
    uint32_t SDIO0FCLKDIV; /* 0x684 */
    uint32_t SDIO1FCLKSEL; /* 0x690 */
    uint32_t SDIO1FCLKDIV; /* 0x694 */
    uint32_t ADC0FCLKSEL0; /* 0x6D0 */
    uint32_t ADC0FCLKSEL1; /* 0x6D4 */
    uint32_t ADC0FCLKDIV; /* 0x6D8 */
    uint32_t UTICKFCLKSEL; /* 0x700 */
    uint32_t WDT0FCLKSEL; /* 0x720 */
    uint32_t A32KHZWAKECLKSEL; /* 0x730 */
    uint32_t A32KHZWAKECLKDIV; /* 0x734 */
    uint32_t SYSTICKFCLKSEL; /* 0x760 */
    uint32_t SYSTICKFCLKDIV; /* 0x764 */
    uint32_t DPHYCLKSEL; /* 0x770 */
    uint32_t DPHYCLKDIV; /* 0x774 */
    uint32_t DPHYESCCLKSEL; /* 0x778 */
    uint32_t DPHYESCRXCLKDIV; /* 0x77C */
    uint32_t DPHYESCTXCLKDIV; /* 0x780 */
    uint32_t GPUCLKSEL; /* 0x790 */
    uint32_t GPUCLKDIV; /* 0x794 */
    uint32_t DCPIXELCLKSEL; /* 0x7A0 */
    uint32_t DCPIXELCLKDIV; /* 0x7A4 */

};

#define TYPE_RT_CLKCTL1 "rt_clkctl1"
OBJECT_DECLARE_SIMPLE_TYPE(RTCLKCTL1State, RT_CLKCTL1)

struct RTCLKCTL1State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t PSCCTL0; /*  0x10  */
    uint32_t PSCCTL1; /*  0x14  */
    uint32_t PSCCTL2; /*  0x18  */
    uint32_t PSCCTL0_SET; /*  0x40  */
    uint32_t PSCCTL1_SET; /*  0x44  */
    uint32_t PSCCTL2_SET; /*  0x48  */
    uint32_t PSCCTL0_CLR; /*  0x70  */
    uint32_t PSCCTL1_CLR; /*  0x74  */
    uint32_t PSCCTL2_CLR; /*  0x78  */
    uint32_t AUDIOPLL0CLKSEL; /*  0x200 */
    uint32_t AUDIOPLL0CTL0; /*  0x204 */
    uint32_t AUDIOPLL0LOCKTIMEDIV2; /*  0x20C */
    uint32_t AUDIOPLL0NUM; /*  0x210 */
    uint32_t AUDIOPLL0DENOM;  /*  0x214 */
    uint32_t AUDIOPLL0PFD; /*  0x218 */
    uint32_t AUDIOPLLCLKDIV;  /*  0x240 */
    uint32_t DSPCPUCLKDIV; /*  0x400 */
    uint32_t DSPCPUCLKSELA; /*  0x430 */
    uint32_t DSPCPUCLKSELB; /*  0x434 */
    uint32_t OSEVENTTFCLKSEL; /*  0x480 */
    uint32_t FRG0CLKSEL;  /*  0x500 */
    uint32_t FRG0CTL; /*  0x504 */
    uint32_t FC0FCLKSEL;  /*  0x508 */
    uint32_t FRG1CLKSEL;  /*  0x520 */
    uint32_t FRG1CTL; /*  0x524 */
    uint32_t FC1FCLKSEL;  /*  0x528 */
    uint32_t FRG2CLKSEL;  /*  0x540 */
    uint32_t FRG2CTL; /*  0x544 */
    uint32_t FC2FCLKSEL;  /*  0x548 */
    uint32_t FRG3CLKSEL;  /*  0x560 */
    uint32_t FRG3CTL; /*  0x564 */
    uint32_t FC3FCLKSEL;  /*  0x568 */
    uint32_t FRG4CLKSEL;  /*  0x580 */
    uint32_t FRG4CTL; /*  0x584 */
    uint32_t FC4FCLKSEL;  /*  0x588 */
    uint32_t FRG5CLKSEL;  /*  0x5A0 */
    uint32_t FRG5CTL; /*  0x5A4 */
    uint32_t FC5FCLKSEL;  /*  0x5A8 */
    uint32_t FRG6CLKSEL;  /*  0x5C0 */
    uint32_t FRG6CTL; /*  0x5C4 */
    uint32_t FC6FCLKSEL;  /*  0x5C8 */
    uint32_t FRG7CLKSEL;  /*  0x5E0 */
    uint32_t FRG7CTL; /*  0x5E4 */
    uint32_t FC7FCLKSEL;  /*  0x5E8 */
    uint32_t FRG8CLKSEL;  /*  0x600 */
    uint32_t FRG8CTL; /*  0x604 */
    uint32_t FC8FCLKSEL;  /*  0x608 */
    uint32_t FRG9CLKSEL;  /*  0x620 */
    uint32_t FRG9CTL; /*  0x624 */
    uint32_t FC9FCLKSEL;  /*  0x628 */
    uint32_t FRG10CLKSEL; /*  0x640 */
    uint32_t FRG10CTL; /*  0x644 */
    uint32_t FC10FCLKSEL; /*  0x648 */
    uint32_t FRG11CLKSEL; /*  0x660 */
    uint32_t FRG11CTL; /*  0x664 */
    uint32_t FC11FCLKSEL; /*  0x668 */
    uint32_t FRG12CLKSEL; /*  0x680 */
    uint32_t FRG12CTL; /*  0x684 */
    uint32_t FC12FCLKSEL; /*  0x688 */
    uint32_t FRG13CLKSEL; /*  0x6A0 */
    uint32_t FRG13CTL; /*  0x6A4 */
    uint32_t FC13FCLKSEL; /*  0x6A8 */
    uint32_t FRG14CLKSEL; /*  0x6C0 */
    uint32_t FRG14CTL; /*  0x6C4 */
    uint32_t FC14FCLKSEL; /*  0x6C8 */
    uint32_t FRG15CLKSEL; /*  0x6E0 */
    uint32_t FRG15CTL; /*  0x6E4 */
    uint32_t FC15FCLKSEL; /*  0x6E8 */
    uint32_t FRG16CLKSEL; /*  0x700 */
    uint32_t FRG16CTL; /*  0x704 */
    uint32_t FC16FCLKSEL; /*  0x708 */
    uint32_t FRG17CLKSEL; /*  0x720 */
    uint32_t FRG17CTL; /*  0x724 */
    uint32_t FLEXIOCLKSEL; /*  0x728 */
    uint32_t FLEXIOCLKDIV; /*  0x740 */
    uint32_t FRGPLLCLKDIV; /*  0x760 */
    uint32_t DMIC0FCLKSEL; /*  0x780 */
    uint32_t DMIC0FCLKDIV; /*  0x784 */
    uint32_t CT32BIT0FCLKSEL; /*  0x7A0 */
    uint32_t CT32BIT1FCLKSEL; /*  0x7A4 */
    uint32_t CT32BIT2FCLKSEL; /*  0x7A8 */
    uint32_t CT32BIT3FCLKSEL; /*  0x7AC */
    uint32_t CT32BIT4FCLKSEL; /*  0x7B0 */
    uint32_t AUDIOMCLKSEL; /*  0x7C0 */
    uint32_t AUDIOMCLKDIV; /*  0x7C4 */
    uint32_t CLKOUTSEL0;  /*  0x7E0 */
    uint32_t CLKOUTSEL1;  /*  0x7E4 */
    uint32_t CLKOUTFCLKDIV; /*  0x7E8 */
    uint32_t I3C01FCLKSEL; /*  0x800 */
    uint32_t I3C01FCLKSTCSEL; /*  0x804 */
    uint32_t I3C01FCLKSTCDIV; /*  0x808 */
    uint32_t I3C01FCLKSDIV; /*  0x80C */
    uint32_t I3C01FCLKDIV; /*  0x810 */
    uint32_t I3C01FCLKSTSTCLKSEL; /*  0x814 */
    uint32_t WDT1FCLKSEL; /*  0x820 */
    uint32_t ACMP0FCLKSEL; /*  0x830 */
    uint32_t ACMP0FCLKDIV; /*  0x834 */

};

#endif /* RT_CLKCTL_H */
