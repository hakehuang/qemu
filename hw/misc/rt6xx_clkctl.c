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

#include "qemu/osdep.h"
#include "qemu/main-loop.h"
#include "qemu/cutils.h"
#include "qemu/module.h"
#include "qemu/guest-random.h"
#include "hw/irq.h"
#include "hw/misc/rt6xx_clkctl.h"
#include "migration/vmstate.h"


#define RT6XX_CLKCTL0_NAME "clock controllor0 for NXP RT6xx series"
#define RT6XX_CLKCTL1_NAME "clock controllor1 for NXP RT6xx series"

static uint64_t rt_clkctl0_read(void *opaque, hwaddr offset, unsigned size)
{
    RT6XXCLKCTL0State *s = RT6XX_CLKCTL0(opaque);
    uint32_t value = 0xFEFEFEFE;
     
    switch(offset) {
        case 0x10:
            value = s->PSCCTL0;
            break;
        case 0x14:
            value = s->PSCCTL1;
            break;
        case 0x18:
            value = s->PSCCTL2;
            break;
        case 0x40: /* WO */
            break;
        case 0x44:
            break;
        case 0x48:
            break;
        case 0x70:
            break;
        case 0x74:
            break;
        case 0x78:
            break;
        case 0x100:
            value -> s->FFROCTL0;
            break;
        case 0x104:
            value -> s->FFROCTL1;
            break;
        case 0x160:
            value = s->SYSOSCCTL0;
            break;
        case 0x168:
            value = s->SYSOSCBYPASS;
            break;
        case 0x190:
            value = s->LPOSCCTL0 | 0x80000000;
            break;
        case 0x1C0:
            value = s->OSC32KHZCTL0;
            break;
        case 0x200:
            value = s->SYSPLL0CLKSEL;
            break;
        case 0x204:
            /* bit1 is reset status  */
            value = s->SYSPLL0CTL0 & (~0x2);
            break;
        case 0x20C:
            value = s->SYSPLL0LOCKTIMEDIV2;
            break;
        case 0x210:
            value = s->SYSPLL0NUM;
            break;
        case 0x214:
            value = s->SYSPLL0DENOM;
            break;
        case 0x218:
            value = s->SYSPLL0PFD | 0x40404040;
            break;
        case 0x240:
            value = s->MAINPLLCLKDIV & (~0xE0000000);
            break;
        case 0x244:
            value = s->DSPPLLCLKDIV & (~0xE0000000);
            break;
        case 0x248:
            value = s->AUX0PLLCLKDIV & (~0xE0000000);
            break;
        case 0x24C:
            value = s->AUX1PLLCLKDIV & (~0xE0000000);
            break;
        case 0x400:
            value = s->SYSCPUAHBCLKDIV & (~0x80000000);
            break;
        case 0x430:
            value = s->MAINCLKSELA;
            break;
        case 0x434:
            value = s->MAINCLKSELB;
            break;
        case 0x500:
            value = s->PFC0DIV & (~0xE0000000);
            break;
        case 0x504:
            value = s->PFC1DIV & (~0xE0000000);
            break;
        case 0x620:
            value = s->FLEXSPI0FCLKSEL;
            break;
        case 0x624:
            value = s->FLEXSPI0FCLKDIV & (~0xE0000000);
            break;
        case 0x640:
            value = s->SCTFCLKSEL;
            break;
        case 0x644:
            value = s->SCTIN7CLKDIV & (~0xE0000000);
            break;
        case 0x660:
            value = s->USBHSFCLKSEL;
            break;
        case 0x664:
            value = s->USBHSFCLKDIV & (~0xE0000000);
            break;
        case 0x680:
            value = s->SDIO0FCLKSEL;
            break;
        case 0x684:
            value = s->SDIO0FCLKDIV & (~0xE0000000);
            break;
        case 0x690:
            value = s->SDIO1FCLKSEL;
            break;
        case 0x694:
            value = s->SDIO1FCLKDIV  & (~0xE0000000);
            break;
        case 0x6D0:
            value = s->ADC0FCLKSEL0;
            break;
        case 0x6D4:
            value = s->ADC0FCLKSEL1;
            break;
        case 0x6D8:
            value = s->ADC0FCLKDIV & (~0xE0000000);
            break;
        case 0x700:
            value = s->UTICKFCLKSEL;
            break;
        case 0x720:
            value = s->WDT0FCLKSEL;
            break;
        case 0x730:
            value = s->A32KHZWAKECLKSEL;
            break;
        case 0x734:
            value = s->A32KHZWAKECLKDIV & (~0xE0000000);
            break;
        case 0x760:
            value = s->SYSTICKFCLKSEL;
            break;
        case 0x764:
            value = s->SYSTICKFCLKDIV & (~0xE0000000);
            break;
        default:
            break;
    }    

    return value;
}

static void rt_clkctl0_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RT6XXCLKCTL0State *s = RT6XX_CLKCTL0(opaque);
    uint32_t v32 = (uint32_t)value;

    switch(offset) {
        case 0x10:
            s->PSCCTL0 = v32;
            break;
        case 0x14:
            s->PSCCTL1 = v32;
            break;
        case 0x18:
            s->PSCCTL2 = v32;
            break;
        case 0x40: /* WO */
            s->PSCCTL0 |= v32;              
            break;
        case 0x44:
            s->PSCCTL1 |= v32;              
            break;
        case 0x48:
            s->PSCCTL2 |= v32;              
            break;
        case 0x70:
            s->PSCCTL0 &= ~v32;
            break;
        case 0x74:
            s->PSCCTL1 &= ~v32;
            break;
        case 0x78:
            s->PSCCTL2 &= ~v32;
            break;
        case 0x100:
            s->FFROCTL0 = v32;
            break;
        case 0x104:
            s->FFROCTL1 = v32;
            break;
        case 0x160:
            s->SYSOSCCTL0 = v32;
            break;
        case 0x168:
            s->SYSOSCBYPASS = v32;
            break;
        case 0x190:
            s->LPOSCCTL0 = v32;
            break;
        case 0x1C0:
            s->OSC32KHZCTL0 = v32;
            break;
        case 0x200:
            s->SYSPLL0CLKSEL = v32;
            break;
        case 0x204:
            /* bit1 is reset status  */
            s->SYSPLL0CTL0 = v32;
            break;
        case 0x20C:
            s->SYSPLL0LOCKTIMEDIV2 = v32 & (~0xE0000000);
            break;
        case 0x210:
            s->SYSPLL0NUM = v32;
            break;
        case 0x214:
            s->SYSPLL0DENOM = v32;
            break;
        case 0x218:
            s->SYSPLL0PFD = v32;
            break;
        case 0x240:
            s->MAINPLLCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x244:
            s->DSPPLLCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x248:
            s->AUX0PLLCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x24C:
            s->AUX1PLLCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x400:
            s->SYSCPUAHBCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x430:
            s->MAINCLKSELA = v32;
            break;
        case 0x434:
            s->MAINCLKSELB = v32;
            break;
        case 0x500:
            s->PFC0DIV = v32 & (~0xE0000000);
            break;
        case 0x504:
            s->PFC1DIV = v32 & (~0xE0000000);
            break;
        case 0x620:
            s->FLEXSPI0FCLKSEL = v32;
            break;
        case 0x624:
            s->FLEXSPI0FCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x640:
            s->SCTFCLKSEL = v32;
            break;
        case 0x644:
            s->SCTIN7CLKDIV = v32 & (~0xE0000000);
            break;
        case 0x660:
            s->USBHSFCLKSEL = v32;
            break;
        case 0x664:
            s->USBHSFCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x680:
            s->SDIO0FCLKSEL = v32;
            break;
        case 0x684:
            s->SDIO0FCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x690:
            s->SDIO1FCLKSEL = v32;
            break;
        case 0x694:
            s->SDIO1FCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x6D0:
            s->ADC0FCLKSEL0 = v32;
            break;
        case 0x6D4:
            s->ADC0FCLKSEL1 = v32;
            break;
        case 0x6D8:
            s->ADC0FCLKDIV = v32 & (~0xE0000000);
            break;
        case 0x700:
            s->UTICKFCLKSEL = v32;
            break;
        case 0x720:
            s->WDT0FCLKSEL = v32;
            break;
        case 0x730:
            s->A32KHZWAKECLKSEL = v32;
            break;
        case 0x734:
            s->A32KHZWAKECLKDIV = v32 & (~0xE0000000);
            break;
        case 0x760:
            s->SYSTICKFCLKSEL = v32;
            break;
        case 0x764:
            s->SYSTICKFCLKDIV = v32 & (~0xE0000000);
            break;
        default:
            break;
    }    

    return;
}

static void rt_clkctl0_reset_at_boot(DeviceState *dev)
{
    RT6XXCLKCTL0State *s = RT6XX_CLKCTL0(dev);

    s->PSCCTL0 = 0x5;
    s->PSCCTL1 = 0;
    s->PSCCTL2 = 0;
    s->PSCCTL0_SET = 0;
    s->PSCCTL1_SET = 0;
    s->PSCCTL2_SET = 0;
    s->PSCCTL0_CLR = 0;
    s->PSCCTL1_CLR = 0;
    s->PSCCTL2_CLR = 0;
    s->FFROCTL0 = 0x20410;
    s->FFROCTL1 = 0;
    s->SYSOSCCTL0 = 0;
    s->SYSOSCBYPASS = 0;
    s->LPOSCCTL0 = 0x807BC4D4;
    s->OSC32KHZCTL0 = 0;
    s->SYSPLL0CLKSEL = 0x7;
    s->SYSPLL0CTL0 = 0x16002;
    s->SYSPLL0LOCKTIMEDIV2 = 0xCAFE;
    s->SYSPLL0NUM = 0x4DD2F15;
    s->SYSPLL0DENOM = 0x1FFFFFDB;
    s->SYSPLL0PFD = 0x80808080;
    s->MAINPLLCLKDIV = 0;
    s->DSPPLLCLKDIV = 0;
    s->AUX0PLLCLKDIV = 0;
    s->AUX1PLLCLKDIV = 0;
    s->SYSCPUAHBCLKDIV = 0;
    s->MAINCLKSELA = 0;
    s->MAINCLKSELB = 0;
    s->PFC0DIV = 0x40000000;
    s->PFC1DIV = 0x40000000;
    s->FLEXSPIFCLKSEL = 0x7;
    s->FLEXSPIFCLKDIV = 0x40000000;
    s->SCTFCLKSEL = 0x7;
    s->SCTFCLKDIV = 0x40000000;
    s->USBHSFCLKSEL = 0x7;
    s->USBHSFCLKDIV = 0x40000000;
    s->SDIO0FCLKSEL = 0x7;
    s->SDIO0FCLKDIV = 0x40000000;
    s->SDIO1FCLKSEL = 0x7;
    s->SDIO1FCLKDIV = 0x40000000;
    s->ADC0FCLKSEL0 = 0x7;
    s->ADC0FCLKSEL1 = 0x7;
    s->ADC0FCLKDIV = 0x40000000;
    s->UTICKFCLKSEL = 0x7;
    s->WDT0FCLKSEL = 0;
    s->WAKECLK32KHZSEL = 0x1;
    s->WAKECLK32KHZDIV = 0x1F;
    s->SYSTICKFCLKSEL = 0x7;
    s->SYSTICKFCLKDIV = 0x40000000;

}

static const MemoryRegionOps rt_clkctl0_ops = {
    .read  = rt_clkctl0_read,
    .write = rt_clkctl0_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_clkctl0_realize(DeviceState *dev, Error **errp)
{
    RT6XXCLKCTL0State *s = RT6XX_CLKCTL0(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_clkctl0_ops, s,
                          TYPE_RT6XX_CLKCTL0, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_clkctl0 = {
    .name = RT6XX_CLKCTL0_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(PSCCTL0, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL1, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL2, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL0_SET, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL1_SET, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL2_SET, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL0_CLR, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL1_CLR, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PSCCTL2_CLR, RT6XXCLKCTL0State),
        VMSTATE_UINT32(FFROCTL0, RT6XXCLKCTL0State),
        VMSTATE_UINT32(FFROCTL1, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSOSCCTL0, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSOSCBYPASS, RT6XXCLKCTL0State),
        VMSTATE_UINT32(LPOSCCTL0, RT6XXCLKCTL0State),
        VMSTATE_UINT32(OSC32KHZCTL0, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0CLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0CTL0, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0LOCKTIMEDIV2, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0NUM, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0DENOM, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0PFD, RT6XXCLKCTL0State),
        VMSTATE_UINT32(MAINPLLCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(DSPPLLCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(AUX0PLLCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(AUX1PLLCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSCPUAHBCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(MAINCLKSELA, RT6XXCLKCTL0State),
        VMSTATE_UINT32(MAINCLKSELB, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PFC0DIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(PFC1DIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(FLEXSPIFCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(FLEXSPIFCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SCTFCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SCTFCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(USBHSFCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(USBHSFCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SDIO0FCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SDIO0FCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SDIO1FCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SDIO1FCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(ADC0FCLKSEL0, RT6XXCLKCTL0State),
        VMSTATE_UINT32(ADC0FCLKSEL1, RT6XXCLKCTL0State),
        VMSTATE_UINT32(ADC0FCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(UTICKFCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(WDT0FCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(WAKECLK32KHZSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(WAKECLK32KHZDIV, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSTICKFCLKSEL, RT6XXCLKCTL0State),
        VMSTATE_UINT32(SYSTICKFCLKDIV, RT6XXCLKCTL0State),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_clkctl0_init(Object *obj)
{
    RT6XXCLKCTL0State *s = RT6XX_CLKCTL0(obj);

    memory_region_init_io(&s->iomem, obj, &rt_clkctl0_ops, s, TYPE_RT6XX_CLKCTL0, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_clkctl0_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_clkctl0_realize;
    dc->vmsd = &vmstate_rt_clkctl0;
    dc->reset = rt_clkctl0_reset_at_boot;
}

static const TypeInfo rt_clkctl0_info = {
    .name          = TYPE_RT6XX_CLKCTL0,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RT6XXCLKCTL0State),
    .instance_init = rt_clkctl0_init,
    .class_init    = rt_clkctl0_class_init,
};

static void rt_clkctl0_register_types(void)
{
    type_register_static(&rt_clkctl0_info);
}

type_init(rt_clkctl0_register_types)

static uint64_t rt_clkctl1_read(void *opaque, hwaddr offset, unsigned size)
{
    RT6XXCLKCTL1State *s = RT6XX_CLKCTL1(opaque);
    uint32_t value = 0xFEFEFEFE;
     
    switch(offset) {
        case 0x10:
            value = s->PSCCTL0;
            break;
        case 0x14:
            value = s->PSCCTL1;
            break;
        case 0x18:
            value = s->PSCCTL2;
            break;
        case 0x40: /* WO */
            break;
        case 0x44:
            break;
        case 0x48:
            break;
        case 0x70:
            break;
        case 0x74:
            break;
        case 0x78:
            break;
        case 0x200: /* Enable Tuning */
            /* in qemu turning does not take time  */
            value = s->AUDIOPLL0CLKSEL;
            break;
        case 0x204:
            /* apll reset is removed  */
            value = s->AUDIOPLL0CTL0 & (~0x2);
            break;
        case 0x20C:
            value = s->AUDIOPLL0LOCKTIMEDIV2;
            break;
        case 0x210:
            value = s->AUDIOPLL0NUM;
            break;
        case 0x214:
            value = s->AUDIOPLL0DENOM;
            break;
        case 0x218:
            /* always locked  */
            value = s->AUDIOPLL0PFD | 0x40404040; 
            break;
        case 0x240:
            value = s->AUDIOPLLCLKDIV & (~0xE0000000);
            break;
        case 0x400:
            value = s->DSPCPUCLKDIV & (~0xE0000000);
            break;
        case 0x400:
            value = s->DSPCPUCLKDIV & (~0xE0000000);
            break;
        case 0x404:
            value = s->DSPMAINRAMCLKDIV;
        case 0x430:
            value = s->DSPCPUCLKSELA;
            break;
        case 0x434:
            value = s->DSPCPUCLKSELB;
            break;
        case 0x480:
            value = s->OSEVENTTFCLKSEL;
            break;
        case 0x500:
            value = s->FRG0CLKSEL;
            break;
        case 0x504:
            value = s->FRG0CTL;
            break;
        case 0x508:
            value = s->FC0FCLKSEL;
            break;
        case 0x520:
            value = s->FRG1CLKSEL;
            break;
        case 0x524:
            value = s->FRG1CTL;
            break;
        case 0x528:
            value = s->FC1FCLKSEL;
            break;
        case 0x540:
            value = s->FRG2CLKSEL;
            break;
        case 0x544:
            value = s->FRG2CTL;
            break;
        case 0x548:
            value = s->FC2FCLKSEL;
            break;
        case 0x560:
            value = s->FRG3CLKSEL;
            break;
        case 0x564:
            value = s->FRG3CTL;
            break;
        case 0x568:
            value = s->FC3FCLKSEL;
            break;
        case 0x580:
            value = s->FRG4CLKSEL;
            break;
        case 0x584:
            value = s->FRG4CTL;
            break;
        case 0x588:
            value = s->FC4FCLKSEL;
            break;
        case 0x5A0:
            value = s->FRG5CLKSEL;
            break;
        case 0x5A4:
            value = s->FRG5CTL;
            break;
        case 0x5A8:
            value = s->FC5FCLKSEL;
            break;
        case 0x5C0:
            value = s->FRG6CLKSEL;
            break;
        case 0x5C4:
            value = s->FRG6CTL;
            break;
        case 0x5C8:
            value = s->FC6FCLKSEL;
            break;
        case 0x5E0:
            value = s->FRG7CLKSEL;
            break;
        case 0x5E4:
            value = s->FRG7CTL;
            break;
        case 0x5E8:
            value = s->FC7FCLKSEL;
            break;
        case 0x6C0:
            value = s->FRG14CLKSEL;
            break;
        case 0x6C4:
            value = s->FRG14CTL;
            break;
        case 0x6C8:
            value = s->FC14FCLKSEL;
            break;
        case 0x6E0:
            value = s->FRG15CLKSEL;
            break;
        case 0x6E4:
            value = s->FRG15CTL;
            break;
        case 0x6E8:
            value = s->FC15FCLKSEL;
            break;
        case 0x6FC:
            /* always ready */
            value = s->FRGPLLCLKDIV & ~0xE0000000;
            break;
        case 0x700:
            value = s->DMIC0FCLKSEL;
            break;
        case 0x704:
            value = s->DMIC0FCLKDIV;
            break;
        case 0x720:
            value = s->CT32BIT0FCLKSEL;
            break;
        case 0x724:
            value = s->CT32BIT1FCLKSEL;
            break;
        case 0x728:
            value = s->CT32BIT2FCLKSEL;
            break;
        case 0x728:
            value = s->CT32BIT3FCLKSEL;
            break;
        case 0x730:
            value = s->CT32BIT4FCLKSEL;
            break;
        case 0x740:
            value = s->AUDIOMCLKSEL;
            break;
        case 0x744:
            value = s->AUDIOMCLKDIV & (~0xE0000000);
            break;
        case 0x760:
            value = s->CLKOUTSEL0;
            break;
        case 0x764:
            value = s->CLKOUTSEL1;
            break;
        case 0x768:
            value = s->CLKOUTDIV & (~0xE0000000);
            break;
        case 0x780:
            value = s->I3C0FCLKSEL;
            break;
        case 0x784:
            value = s->I3C0FCLKSTCSEL;
            break;
        case 0x788:
            value = s->I3C0FCLKSTCDIV & (~0xE0000000);
            break;
        case 0x78C:
            value = s->I3C0FCLKSDIV & (~0xE0000000);
            break;
        case 0x790:
            value = s->I3C0FCLKDIV & (~0xE0000000);
            break;
        case 0x7A0:
            value = s->WDT1FCLKSEL;
            break;
        case 0x7C0:
            value = s->ACMP0FCLKSEL;
            break;
        case 0x7C4:
            value = s->ACMP0FCLKDIV & (~0xE0000000);
            break;
        default:
            break;
    }    

    return value;
}

static void rt_clkctl1_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RT6XXCLKCTL1State *s = RT6XX_CLKCTL1(opaque);
    uint32_t v32 = (uint32_t)value;

    switch(offset) {
        case 0x10:
            s->PSCCTL0 = v32;
            break;
        case 0x14:
            s->PSCCTL1 = v32;
            break;
        case 0x18:
            s->PSCCTL2 = v32;
            break;
        case 0x40: /* WO */
           s->PSCCTL0 |= v32;
            break;
        case 0x44:
           s->PSCCTL1 |= v32;
            break;
        case 0x48:
           s->PSCCTL2 |= v32;
            break;
        case 0x70:
           s->PSCCTL0 &= ~v32;
            break;
        case 0x74:
           s->PSCCTL1 &= ~v32;
            break;
        case 0x78:
           s->PSCCTL2 &= ~v32;
            break;
        case 0x200: /* Enable Tuning */
            /* in qemu turning does not take time  */
            s->AUDIOPLL0CLKSEL = v32;
            break;
        case 0x204:
            /* apll reset is removed  */
            s->AUDIOPLL0CTL0 = v32;
            break;
        case 0x20C:
            s->AUDIOPLL0LOCKTIMEDIV2 = v32;
            break;
        case 0x210:
            s->AUDIOPLL0NUM = v32;
            break;
        case 0x214:
            s->AUDIOPLL0DENOM = v32;
            break;
        case 0x218:
            /* always locked  */
            s->AUDIOPLL0PFD = v32; 
            break;
        case 0x240:
            s->AUDIOPLLCLKDIV = v32;
            break;
        case 0x400:
            s->DSPCPUCLKDIV = v32;
            break;
        case 0x404:
            s->DSPMAINRAMCLKDIV = v32;
            break;
        case 0x430:
            s->DSPCPUCLKSELA = v32;
            break;
        case 0x434:
            s->DSPCPUCLKSELB = v32;
            break;
        case 0x480:
            s->OSEVENTTFCLKSEL = v32;
            break;
        case 0x500:
            s->FRG0CLKSEL = v32;
            break;
        case 0x504:
            s->FRG0CTL = v32;
            break;
        case 0x508:
            s->FC0FCLKSEL = v32;
            break;
        case 0x520:
            s->FRG1CLKSEL = v32;
            break;
        case 0x524:
            s->FRG1CTL = v32;
            break;
        case 0x528:
            s->FC1FCLKSEL = v32;
            break;
        case 0x540:
            s->FRG2CLKSEL = v32;
            break;
        case 0x544:
            s->FRG2CTL = v32;
            break;
        case 0x548:
            s->FC2FCLKSEL = v32;
            break;
        case 0x560:
            s->FRG3CLKSEL = v32;
            break;
        case 0x564:
            s->FRG3CTL = v32;
            break;
        case 0x568:
            s->FC3FCLKSEL = v32;
            break;
        case 0x580:
            s->FRG4CLKSEL = v32;
            break;
        case 0x584:
            s->FRG4CTL = v32;
            break;
        case 0x588:
            s->FC4FCLKSEL = v32;
            break;
        case 0x5A0:
            s->FRG5CLKSEL = v32;
            break;
        case 0x5A4:
            s->FRG5CTL = v32;
            break;
        case 0x5A8:
            s->FC5FCLKSEL = v32;
            break;
        case 0x5C0:
            s->FRG6CLKSEL = v32;
            break;
        case 0x5C4:
            s->FRG6CTL = v32;
            break;
        case 0x5C8:
            s->FC6FCLKSEL = v32;
            break;
        case 0x5E0:
            s->FRG7CLKSEL = v32;
            break;
        case 0x5E4:
            s->FRG7CTL = v32;
            break;
        case 0x5E8:
            s->FC7FCLKSEL = v32;
            break;
        case 0x6C0:
            s->FRG14CLKSEL = v32;
            break;
        case 0x6C4:
            s->FRG14CTL = v32;
            break;
        case 0x6C8:
            s->FC14FCLKSEL = v32;
            break;
        case 0x6E0:
            s->FRG15CLKSEL = v32;
            break;
        case 0x6E4:
            s->FRG15CTL = v32;
            break;
        case 0x6E8:
            s->FC15FCLKSEL = v32;
            break;
        case 0x6FC:
            s->FRGPLLCLKDIV = v32;
            break;
        case 0x700:
            s->DMIC0FCLKSEL = v32;
            break;
        case 0x704:
            s->DMIC0FCLKDIV = v32;
            break;
        case 0x720:
            s->CT32BIT0FCLKSEL = v32;
            break;
        case 0x724:
            s->CT32BIT1FCLKSEL = v32;
            break;
        case 0x728:
            s->CT32BIT2FCLKSEL = v32;
            break;
        case 0x72C:
            s->CT32BIT3FCLKSEL = v32;
            break;
        case 0x730:
            s->CT32BIT4FCLKSEL = v32;
            break;
        case 0x740:
            s->AUDIOMCLKSEL = v32;
            break;
        case 0x744:
            s->AUDIOMCLKDIV = v32;
            break;
        case 0x760:
            s->CLKOUTSEL0 = v32;
            break;
        case 0x764:
            s->CLKOUTSEL1 = v32;
            break;
        case 0x768:
            s->CLKOUTDIV = v32;
            break;
        case 0x780:
            s->I3C0FCLKSEL = v32;
            break;
        case 0x784:
            s->I3C0FCLKSTCSEL = v32;
            break;
        case 0x788:
            s->I3C0FCLKSTCDIV = v32;
            break;
        case 0x78C:
            s->I3C0FCLKSDIV = v32;
            break;
        case 0x790:
            s->I3C0FCLKDIV = v32;
            break;
        case 0x7A0:
            s->WDT1FCLKSEL = v32;
            break;
        case 0x7C0:
            s->ACMP0FCLKSEL = v32;
            break;
        case 0x7C4:
            s->ACMP0FCLKDIV = v32;
            break;
        default:
            break;
    }

    return;
}

static void rt_clkctl1_reset_at_boot(DeviceState *dev)
{
    RT6XXCLKCTL1State *s = RT6XX_CLKCTL1(dev);
 
    s->PSCCTL0 = 0;
    s->PSCCTL1 = 0;
    s->PSCCTL2 = 0;
    s->PSCCTL0_SET = 0;
    s->PSCCTL1_SET = 0;
    s->PSCCTL2_SET = 0;
    s->PSCCTL0_CLR = 0;
    s->PSCCTL1_CLR = 0;
    s->PSCCTL2_CLR = 0;
    s->AUDIOPLL0CLKSEL = 0x7;
    s->AUDIOPLL0CTL0 = 0x160002;
    s->AUDIOPLL0LOCKTIMEDIV2 = 0xCAFE;
    s->AUDIOPLL0NUM = 0x4DD2F15;
    s->AUDIOPLL0DENOM = 0x1FFFFDB;
    s->AUDIOPLL0PFD = 0x80808080;
    s->AUDIOPLLCLKDIV = 0x40000000;
    s->DSPCPUCLKDIV = 0x40000000;
    s->DSPMAINRAMCLKDIV = 1;
    s->DSPCPUCLKSELA = 0;
    s->DSPCPUCLKSELB = 0;
    s->OSEVENTFCLKSEL = 0;
    s->FRG0CLKSEL = 0x7;
    s->FRG0CTL = 0xFF;
    s->FC0FCLKSEL = 0X7;
    s->FRG1CLKSEL = 0x7;
    s->FRG1CTL = 0xFF;
    s->FC1FCLKSEL = 0x7;
    s->FRG2CLKSEL = 0x7;
    s->FRG2CTL = 0xFF;
    s->FC2FCLKSEL = 0x7;
    s->FRG3CLKSEL = 0x7;
    s->FRG3CTL = 0xFF;
    s->FC3FCLKSEL = 0x7;
    s->FRG4CLKSEL = 0x7;
    s->FRG4CTL = 0xFF;
    s->FC4FCLKSEL = 0x7;
    s->FRG5CLKSEL = 0x7;
    s->FRG5CTL = 0xFF;
    s->FC5FCLKSEL = 0x7;
    s->FRG6CLKSEL = 0x7;
    s->FRG6CTL = 0xFF;
    s->FC6FCLKSEL = 0x7;
    s->FRG7CLKSEL = 0x7;
    s->FRG7CTL = 0xFF;
    s->FC7FCLKSEL = 0x7;
    s->FRG14CLKSEL = 0x7;
    s->FRG14CTL = 0xFF;
    s->FC14FCLKSEL = 0x7;
    s->FRG15CLKSEL = 0x7;
    s->FRG15CTL = 0xFF;
    s->FC15FCLKSEL = 0x7;
    s->FRGPLLCLKDIV = 0x40000000;
    s->DMIC0FCLKSEL = 0x7;
    s->DMIC0FCLKDIV = 0x40000000;
    s->CT32BIT0FCLKSEL = 0x7;
    s->CT32BIT1FCLKSEL = 0x7;
    s->CT32BIT2FCLKSEL = 0x7;
    s->CT32BIT3FCLKSEL = 0x7;
    s->CT32BIT4FCLKSEL = 0x7;
    s->AUDIOMCLKSEL = 0x7;
    s->AUDIOMCLKDIV = 0x40000000;
    s->CLKOUTSEL0 = 0x7;
    s->CLKOUTSEL1 = 0x7;
    s->CLKOUTDIV = 0x40000000;
    s->I3C0FCLKSEL = 0x7;
    s->I3C0FCLKSTCSEL = 0x7;
    s->I3C0FCLKSTCDIV = 0x40000000;
    s->I3C0FCLKSDIV = 0x40000000;
    s->I3C0FCLKDIV = 0x40000000;
    s->WDT1FCLKSEL = 0x7;
    s->ACMP0FCLKSEL = 0x7;
    s->ACMP0FCLKDIV = 0x40000000;
}

static const MemoryRegionOps rt_clkctl1_ops = {
    .read  = rt_clkctl1_read,
    .write = rt_clkctl1_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_clkctl1_realize(DeviceState *dev, Error **errp)
{
    RT6XXCLKCTL1State *s = RT6XX_CLKCTL1(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_clkctl1_ops, s,
                          TYPE_RT6XX_CLKCTL1, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_clkctl1 = {
    .name = RT6XX_CLKCTL1_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(PSCCTL0, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL1, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL2, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL0_SET, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL1_SET, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL2_SET, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL0_CLR, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL1_CLR, RT6XXCLKCTL1State),
        VMSTATE_UINT32(PSCCTL2_CLR, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0CTL0, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0LOCKTIMEDIV2, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0NUM, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0DENOM, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0PFD, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLLCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(DSPCPUCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(DSPMAINRAMCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(DSPCPUCLKSELA, RT6XXCLKCTL1State),
        VMSTATE_UINT32(DSPCPUCLKSELB, RT6XXCLKCTL1State),
        VMSTATE_UINT32(OSEVENTFCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG0CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG0CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC0FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG1CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG1CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC1FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG2CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG2CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC2FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG3CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG3CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC3FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG4CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG4CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC4FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG5CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG5CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC5FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG6CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG6CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC6FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG7CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG7CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC7FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG14CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG14CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC14FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG15CLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRG15CTL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FC15FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(FRGPLLCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(DMIC0FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(DMIC0FCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CT32BIT0FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CT32BIT1FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CT32BIT2FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CT32BIT3FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CT32BIT4FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOMCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(AUDIOMCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CLKOUTSEL0, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CLKOUTSEL1, RT6XXCLKCTL1State),
        VMSTATE_UINT32(CLKOUTDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(I3C0FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(I3C0FCLKSTCSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(I3C0FCLKSTCDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(I3C0FCLKSDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(I3C0FCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_UINT32(WDT1FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(ACMP0FCLKSEL, RT6XXCLKCTL1State),
        VMSTATE_UINT32(ACMP0FCLKDIV, RT6XXCLKCTL1State),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_clkctl1_init(Object *obj)
{
    RT6XXCLKCTL1State *s = RT6XX_CLKCTL1(obj);

    memory_region_init_io(&s->iomem, obj, &rt_clkctl1_ops, s, TYPE_RT6XX_CLKCTL1, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

}

static void rt_clkctl1_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_clkctl1_realize;
    dc->vmsd = &vmstate_rt_clkctl1;
    dc->reset = rt_clkctl1_reset_at_boot;
}

static const TypeInfo rt_clkctl1_info = {
    .name          = TYPE_RT6XX_CLKCTL1,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RT6XXCLKCTL1State),
    .instance_init = rt_clkctl1_init,
    .class_init    = rt_clkctl1_class_init,
};

static void rt_clkctl1_register_types(void)
{
    type_register_static(&rt_clkctl1_info);
}

type_init(rt_clkctl1_register_types)

