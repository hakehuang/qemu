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
#include "hw/misc/rt_clkctl.h"
#include "migration/vmstate.h"


#define RT_CLKCTL0_NAME "clock controllor0 for NXP RTxxx series"
#define RT_CLKCTL1_NAME "clock controllor1 for NXP RTxxx series"

static uint64_t rt_clkctl0_read(void *opaque, hwaddr offset, unsigned size)
{
    RTCLKCTL0State *s = RT_CLKCTL0(opaque);
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
        case 0x80: /* Enable Tuning */
            /* in qemu turning does not take time  */
            value = s->FRO_CONTROL & (~0x80000000);
            break;
        case 0x84:
            /* data is always valid  */
            value = s->FRO_CAPVAL | 0x80000000;
            break;
        case 0x8C:
            value = s->FRO_RDTRIM;
            break;
        case 0x90:
            value = s->FRO_SCTRIM;
            break;
        case 0x108:
            value = s->FRODIVSEL;
            break;
        case 0x10C:
            /* always locked  */
            value = 0x1; 
            break;
        case 0x110:
            value = s->FRODIVOEN;
            break;
        case 0x130:
            value = s->LOWFREQCLKDIV & (~0xE0000000);
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
        case 0x630:
            value = s->FLEXSPI1FCLKSEL;
            break;
        case 0x634:
            value = s->FLEXSPI1FCLKDIV & (~0xE0000000);
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
        case 0x770:
            value = s->DPHYCLKSEL;
            break;
        case 0x774:
            value = s->DPHYCLKDIV & (~0xE0000000);
            break;
        case 0x778:
            value = s->DPHYESCCLKSEL;
            break;
        case 0x77C:
            value = s->DPHYESCRXCLKDIV & (~0xE0000000);
            break;
        case 0x780:
            value = s->DPHYESCTXCLKDIV & (~0xE0000000);
            break;
        case 0x790:
            value = s->GPUCLKSEL;
            break;
        case 0x794:
            value = s->GPUCLKDIV & (~0xE0000000);
            break;
        case 0x7A0:
            value = s->DCPIXELCLKSEL;
            break;
        case 0x7A4:
            value = s->DCPIXELCLKDIV & (~0xE0000000);
            break;
        default:
    }    

    return value;
}

static void rt_clkctl0_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTCLKCTL0State *s = RT_CLKCTL0(opaque);
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
        case 0x80: /* Enable Tuning */
            /* in qemu turning does not take time  */
            s->FRO_CONTROL = v32;
            break;
        case 0x84:
            /* RO  */
            break;
        case 0x8C:
            s->FRO_RDTRIM = v32;
            break;
        case 0x90:
            s->FRO_SCTRIM = v32;
            break;
        case 0x108:
            s->FRODIVSEL = v32;
            break;
        case 0x10C:
            /* RO  */
            break;
        case 0x110:
            s->FRODIVOEN = v32;
            break;
        case 0x130:
            s->LOWFREQCLKDIV = v32;
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
            s->SYSPLL0LOCKTIMEDIV2 = v32;
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
            s->MAINPLLCLKDIV = v32;
            break;
        case 0x244:
            s->DSPPLLCLKDIV = v32;
            break;
        case 0x248:
            s->AUX0PLLCLKDIV = v32;
            break;
        case 0x24C:
            s->AUX1PLLCLKDIV = v32;
            break;
        case 0x400:
            s->SYSCPUAHBCLKDIV = v32;
            break;
        case 0x430:
            s->MAINCLKSELA = v32;
            break;
        case 0x434:
            s->MAINCLKSELB = v32;
            break;
        case 0x500:
            s->PFC0DIV = v32;
            break;
        case 0x504:
            s->PFC1DIV = v32;
            break;
        case 0x620:
            s->FLEXSPI0FCLKSEL = v32;
            break;
        case 0x624:
            s->FLEXSPI0FCLKDIV = v32;
            break;
        case 0x630:
            s->FLEXSPI1FCLKSEL = v32;
            break;
        case 0x634:
            s->FLEXSPI1FCLKDIV = v32;
            break;
        case 0x640:
            s->SCTFCLKSEL = v32;
            break;
        case 0x644:
            s->SCTIN7CLKDIV = v32;
            break;
        case 0x660:
            s->USBHSFCLKSEL = v32;
            break;
        case 0x664:
            s->USBHSFCLKDIV = v32;
            break;
        case 0x680:
            s->SDIO0FCLKSEL = v32;
            break;
        case 0x684:
            s->SDIO0FCLKDIV = v32;
            break;
        case 0x690:
            s->SDIO1FCLKSEL = v32;
            break;
        case 0x694:
            s->SDIO1FCLKDIV = v32;
            break;
        case 0x6D0:
            s->ADC0FCLKSEL0 = v32;
            break;
        case 0x6D4:
            s->ADC0FCLKSEL1 = v32;
            break;
        case 0x6D8:
            s->ADC0FCLKDIV = v32;
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
            s->A32KHZWAKECLKDIV = v32;
            break;
        case 0x760:
            s->SYSTICKFCLKSEL = v32;
            break;
        case 0x764:
            s->SYSTICKFCLKDIV = v32;
            break;
        case 0x770:
            s->DPHYCLKSEL = v32;
            break;
        case 0x774:
            s->DPHYCLKDIV = v32;
            break;
        case 0x778:
            s->DPHYESCCLKSEL = v32;
            break;
        case 0x77C:
            s->DPHYESCRXCLKDIV = v32;
            break;
        case 0x780:
            s->DPHYESCTXCLKDIV = v32;
            break;
        case 0x790:
            s->GPUCLKSEL = v32;
            break;
        case 0x794:
            s->GPUCLKDIV = v32;
            break;
        case 0x7A0:
            s->DCPIXELCLKSEL = v32;
            break;
        case 0x7A4:
            s->DCPIXELCLKDIV = v32;
            break;
        default:
    }    

    return;
}

static void rt_clkctl0_reset_at_boot(DeviceState *dev)
{
    RTCLKCTL0State *s = RT_CLKCTL0(dev);

    s->PSCCTL0 = 0x5;
    s->PSCCTL1 = 0;
    s->PSCCTL2 = 0;
    s->FRO_CONTROL = 0;
    s->FRO_CAPVAL = 0;
    s->FRO_RDTRIM = 0x000003BF;
    s->FRO_SCTRIM = 0x20;
    s->FRODIVSEL = 0;
    s->FROCLKSTATUS = 0;
    s->FRODIVOEN = 0;
    s->LOWFREQCLKDIV = 0;
    s->SYSOSCCTL0 = 0;
    s->SYSOSCBYPASS = 0;
    s->LPOSCCTL0 = 0x807BC4D4;
    s->OSC32KHZCTL0 = 0;
    s->SYSPLL0CLKSEL = 0x7;
    s->SYSPLL0CTL0 = 0x00160002;
    s->SYSPLL0LOCKTIMEDIV2 = 0x0000CAFE;
    s->SYSPLL0NUM = 0x04DD2F15;
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
    s->FLEXSPI0FCLKSEL = 0x7;
    s->FLEXSPI0FCLKDIV = 0x40000000;
    s->FLEXSPI1FCLKSEL = 0x7;
    s->FLEXSPI1FCLKDIV = 0x40000000;
    s->SCTFCLKSEL = 0x7;
    s->SCTIN7CLKDIV = 0x40000000;
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
    s->A32KHZWAKECLKSEL = 0x1;
    s->A32KHZWAKECLKDIV = 0x1F;
    s->SYSTICKFCLKSEL = 0x7;
    s->SYSTICKFCLKDIV = 0x40000000;
    s->DPHYCLKSEL = 0x7;
    s->DPHYCLKDIV = 0x40000000;
    s->DPHYESCCLKSEL = 0x7;
    s->DPHYESCRXCLKDIV = 0x40000000;
    s->DPHYESCTXCLKDIV = 0x40000000;
    s->GPUCLKSEL = 0x7;
    s->GPUCLKDIV = 0x40000000;
    s->DCPIXELCLKSEL = 0x7;
    s->DCPIXELCLKDIV = 0x40000000;

}

static const MemoryRegionOps rt_clkctl0_ops = {
    .read  = rt_clkctl0_read,
    .write = rt_clkctl0_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_clkctl0_realize(DeviceState *dev, Error **errp)
{
    RTCLKCTL0State *s = RT_CLKCTL0(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_clkctl0_ops, s,
                          TYPE_RT_CLKCTL0, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_clkctl0 = {
    .name = RT_CLKCTL0_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(PSCCTL0, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL1, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL2, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL0_SET, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL1_SET, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL2_SET, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL0_CLR, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL1_CLR, RTCLKCTL0State),
        VMSTATE_UINT32(PSCCTL2_CLR, RTCLKCTL0State),
        VMSTATE_UINT32(FRO_CONTROL, RTCLKCTL0State),
        VMSTATE_UINT32(FRO_CAPVAL, RTCLKCTL0State),
        VMSTATE_UINT32(FRO_RDTRIM, RTCLKCTL0State),
        VMSTATE_UINT32(FRO_SCTRIM, RTCLKCTL0State),
        VMSTATE_UINT32(FRODIVSEL, RTCLKCTL0State),
        VMSTATE_UINT32(FROCLKSTATUS, RTCLKCTL0State),
        VMSTATE_UINT32(FRODIVOEN, RTCLKCTL0State),
        VMSTATE_UINT32(LOWFREQCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(SYSOSCCTL0, RTCLKCTL0State),
        VMSTATE_UINT32(SYSOSCBYPASS, RTCLKCTL0State),
        VMSTATE_UINT32(LPOSCCTL0, RTCLKCTL0State),
        VMSTATE_UINT32(OSC32KHZCTL0, RTCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0CLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0CTL0, RTCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0LOCKTIMEDIV2, RTCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0NUM, RTCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0DENOM, RTCLKCTL0State),
        VMSTATE_UINT32(SYSPLL0PFD, RTCLKCTL0State),
        VMSTATE_UINT32(MAINPLLCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(DSPPLLCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(AUX0PLLCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(AUX1PLLCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(SYSCPUAHBCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(MAINCLKSELA, RTCLKCTL0State),
        VMSTATE_UINT32(MAINCLKSELB, RTCLKCTL0State),
        VMSTATE_UINT32(PFC0DIV, RTCLKCTL0State),
        VMSTATE_UINT32(PFC1DIV, RTCLKCTL0State),
        VMSTATE_UINT32(FLEXSPI0FCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(FLEXSPI0FCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(FLEXSPI1FCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(FLEXSPI1FCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(SCTFCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(SCTIN7CLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(USBHSFCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(USBHSFCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(SDIO0FCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(SDIO0FCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(SDIO1FCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(SDIO1FCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(ADC0FCLKSEL0, RTCLKCTL0State),
        VMSTATE_UINT32(ADC0FCLKSEL1, RTCLKCTL0State),
        VMSTATE_UINT32(ADC0FCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(UTICKFCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(WDT0FCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(A32KHZWAKECLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(A32KHZWAKECLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(SYSTICKFCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(SYSTICKFCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(DPHYCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(DPHYCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(DPHYESCCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(DPHYESCRXCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(DPHYESCTXCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(GPUCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(GPUCLKDIV, RTCLKCTL0State),
        VMSTATE_UINT32(DCPIXELCLKSEL, RTCLKCTL0State),
        VMSTATE_UINT32(DCPIXELCLKDIV, RTCLKCTL0State),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_clkctl0_init(Object *obj)
{
    RTCLKCTL0State *s = RT_CLKCTL0(obj);

    memory_region_init_io(&s->iomem, obj, &rt_clkctl0_ops, s, TYPE_RT_CLKCTL0, 0x1000);
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
    .name          = TYPE_RT_CLKCTL0,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTCLKCTL0State),
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
    RTCLKCTL1State *s = RT_CLKCTL1(opaque);
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
        case 0x600:
            value = s->FRG8CLKSEL;
            break;
        case 0x604:
            value = s->FRG8CTL;
            break;
        case 0x608:
            value = s->FC8FCLKSEL;
            break;
        case 0x620:
            value = s->FRG9CLKSEL;
            break;
        case 0x624:
            value = s->FRG9CTL;
            break;
        case 0x628:
            value = s->FC9FCLKSEL;
            break;
        case 0x640:
            value = s->FRG10CLKSEL;
            break;
        case 0x644:
            value = s->FRG10CTL;
            break;
        case 0x648:
            value = s->FC10FCLKSEL;
            break;
        case 0x660:
            value = s->FRG11CLKSEL;
            break;
        case 0x664:
            value = s->FRG11CTL;
            break;
        case 0x668:
            value = s->FC11FCLKSEL;
            break;
        case 0x680:
            value = s->FRG12CLKSEL;
            break;
        case 0x684:
            value = s->FRG12CTL;
            break;
        case 0x688:
            value = s->FC12FCLKSEL;
            break;
        case 0x6A0:
            value = s->FRG13CLKSEL;
            break;
        case 0x6A4:
            value = s->FRG13CTL;
            break;
        case 0x6A8:
            value = s->FC13FCLKSEL;
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
        case 0x700:
            value = s->FRG16CLKSEL;
            break;
        case 0x704:
            value = s->FRG16CTL;
            break;
        case 0x708:
            value = s->FC16FCLKSEL;
            break;
        case 0x720:
            value = s->FRG17CLKSEL;
            break;
        case 0x724:
            value = s->FRG17CTL;
            break;
        case 0x728:
            value = s->FLEXIOCLKSEL;
            break;
        case 0x740:
            value = s->FLEXIOCLKDIV & (~0xE0000000);
            break;
        case 0x760:
            value = s->FRGPLLCLKDIV & (~0xE0000000);
            break;
        case 0x780:
            value = s->DMIC0FCLKSEL;
            break;
        case 0x784:
            value = s->DMIC0FCLKDIV & (~0xE0000000);
            break;
        case 0x7A0:
            value = s->CT32BIT0FCLKSEL;
            break;
        case 0x7A4:
            value = s->CT32BIT1FCLKSEL;
            break;
        case 0x7A8:
            value = s->CT32BIT2FCLKSEL;
            break;
        case 0x7AC:
            value = s->CT32BIT3FCLKSEL;
            break;
        case 0x7B0:
            value = s->CT32BIT4FCLKSEL;
            break;
        case 0x7C0:
            value = s->AUDIOMCLKSEL;
            break;
        case 0x7C4:
            value = s->AUDIOMCLKDIV & (~0xE0000000);
            break;
        case 0x7E0:
            value = s->CLKOUTSEL0;
            break;
        case 0x7E4:
            value = s->CLKOUTSEL1;
            break;
        case 0x7E8:
            value = s->CLKOUTFCLKDIV & (~0xE0000000);
            break;
        case 0x800:
            value = s->I3C01FCLKSEL;
            break;
        case 0x804:
            value = s->I3C01FCLKSTCSEL;
            break;
        case 0x808:
            value = s->I3C01FCLKSTCDIV & (~0xE0000000);
            break;
        case 0x80C:
            value = s->I3C01FCLKSDIV & (~0xE0000000);
            break;
        case 0x810:
            value = s->I3C01FCLKDIV & (~0xE0000000);
            break;
        case 0x814:
            value = s->I3C01FCLKSTSTCLKSEL;
            break;
        case 0x820:
            value = s->WDT1FCLKSEL;
            break;
        case 0x830:
            value = s->ACMP0FCLKSEL;
            break;
        case 0x834:
            value = s->ACMP0FCLKDIV & (~0xE0000000);
            break;
        default:
    }    

    return value;
}

static void rt_clkctl1_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTCLKCTL1State *s = RT_CLKCTL1(opaque);
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
        case 0x600:
            s->FRG8CLKSEL = v32;
            break;
        case 0x604:
            s->FRG8CTL = v32;
            break;
        case 0x608:
            s->FC8FCLKSEL = v32;
            break;
        case 0x620:
            s->FRG9CLKSEL = v32;
            break;
        case 0x624:
            s->FRG9CTL = v32;
            break;
        case 0x628:
            s->FC9FCLKSEL = v32;
            break;
        case 0x640:
            s->FRG10CLKSEL = v32;
            break;
        case 0x644:
            s->FRG10CTL = v32;
            break;
        case 0x648:
            s->FC10FCLKSEL = v32;
            break;
        case 0x660:
            s->FRG11CLKSEL = v32;
            break;
        case 0x664:
            s->FRG11CTL = v32;
            break;
        case 0x668:
            s->FC11FCLKSEL = v32;
            break;
        case 0x680:
            s->FRG12CLKSEL = v32;
            break;
        case 0x684:
            s->FRG12CTL = v32;
            break;
        case 0x688:
            s->FC12FCLKSEL = v32;
            break;
        case 0x6A0:
            s->FRG13CLKSEL = v32;
            break;
        case 0x6A4:
            s->FRG13CTL = v32;
            break;
        case 0x6A8:
            s->FC13FCLKSEL = v32;
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
        case 0x700:
            s->FRG16CLKSEL = v32;
            break;
        case 0x704:
            s->FRG16CTL = v32;
            break;
        case 0x708:
            s->FC16FCLKSEL = v32;
            break;
        case 0x720:
            s->FRG17CLKSEL = v32;
            break;
        case 0x724:
            s->FRG17CTL = v32;
            break;
        case 0x728:
            s->FLEXIOCLKSEL = v32;
            break;
        case 0x740:
            s->FLEXIOCLKDIV = v32;
            break;
        case 0x760:
            s->FRGPLLCLKDIV = v32;
            break;
        case 0x780:
            s->DMIC0FCLKSEL = v32;
            break;
        case 0x784:
            s->DMIC0FCLKDIV = v32;
            break;
        case 0x7A0:
            s->CT32BIT0FCLKSEL = v32;
            break;
        case 0x7A4:
            s->CT32BIT1FCLKSEL = v32;
            break;
        case 0x7A8:
            s->CT32BIT2FCLKSEL = v32;
            break;
        case 0x7AC:
            s->CT32BIT3FCLKSEL = v32;
            break;
        case 0x7B0:
            s->CT32BIT4FCLKSEL = v32;
            break;
        case 0x7C0:
            s->AUDIOMCLKSEL = v32;
            break;
        case 0x7C4:
            s->AUDIOMCLKDIV = v32;
            break;
        case 0x7E0:
            s->CLKOUTSEL0 = v32;
            break;
        case 0x7E4:
            s->CLKOUTSEL1 = v32;
            break;
        case 0x7E8:
            s->CLKOUTFCLKDIV = v32;
            break;
        case 0x800:
            s->I3C01FCLKSEL = v32;
            break;
        case 0x804:
            s->I3C01FCLKSTCSEL = v32;
            break;
        case 0x808:
            s->I3C01FCLKSTCDIV = v32;
            break;
        case 0x80C:
            s->I3C01FCLKSDIV = v32;
            break;
        case 0x810:
            s->I3C01FCLKDIV = v32;
            break;
        case 0x814:
            s->I3C01FCLKSTSTCLKSEL = v32;
            break;
        case 0x820:
            s->WDT1FCLKSEL = v32;
            break;
        case 0x830:
            s->ACMP0FCLKSEL = v32;
            break;
        case 0x834:
            s->ACMP0FCLKDIV = v32;
            break;
        default:
    }

    return;
}

static void rt_clkctl1_reset_at_boot(DeviceState *dev)
{
    RTCLKCTL1State *s = RT_CLKCTL1(dev);
    
    s->PSCCTL0 = 0x0;
    s->PSCCTL1 = 0x0;
    s->PSCCTL2 = 0x0;
    s->AUDIOPLL0CLKSEL = 0x7;
    s->AUDIOPLL0CTL0 = 0x160002;
    s->AUDIOPLL0LOCKTIMEDIV2 = 0x000CAFE;
    s->AUDIOPLL0NUM = 0x04DD2F15;
    s->AUDIOPLL0DENOM = 0x1FFFFFDB;
    s->AUDIOPLL0PFD = 0x80808080;
    s->AUDIOPLLCLKDIV = 0x40000000;
    s->DSPCPUCLKDIV = 0x40000000;
    s->DSPCPUCLKSELA = 0x0;
    s->DSPCPUCLKSELB = 0x0;
    s->OSEVENTTFCLKSEL = 0x0;
    s->FRG0CLKSEL = 0x7;
    s->FRG0CTL = 0xFF;
    s->FC0FCLKSEL = 0x7;
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
    s->FRG8CLKSEL = 0x7;
    s->FRG8CTL = 0xFF;
    s->FC8FCLKSEL = 0x7;
    s->FRG9CLKSEL = 0x7;
    s->FRG9CTL = 0xFF;
    s->FC9FCLKSEL = 0x7;
    s->FRG10CLKSEL = 0x7;
    s->FRG10CTL = 0xFF;
    s->FC10FCLKSEL = 0x7;
    s->FRG11CLKSEL = 0x7;
    s->FRG11CTL = 0xFF;
    s->FC11FCLKSEL = 0x7;
    s->FRG12CLKSEL = 0x7;
    s->FRG12CTL = 0xFF;
    s->FC12FCLKSEL = 0x7;
    s->FRG13CLKSEL = 0x7;
    s->FRG13CTL = 0xFF;
    s->FC13FCLKSEL = 0x7;
    s->FRG14CLKSEL = 0x7;
    s->FRG14CTL = 0xFF;
    s->FC14FCLKSEL = 0x7;
    s->FRG15CLKSEL = 0x7;
    s->FRG15CTL = 0xFF;
    s->FC15FCLKSEL = 0x7;
    s->FRG16CLKSEL = 0x7;
    s->FRG16CTL = 0xFF;
    s->FC16FCLKSEL = 0x7;
    s->FRG17CLKSEL = 0x7;
    s->FRG17CTL = 0xFF;
    s->FLEXIOCLKSEL = 0x7;
    s->FLEXIOCLKDIV = 0x40000000;
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
    s->CLKOUTFCLKDIV = 0x40000000;
    s->I3C01FCLKSEL = 0x7;
    s->I3C01FCLKSTCSEL = 0x7;
    s->I3C01FCLKSTCDIV = 0x40000000;
    s->I3C01FCLKSDIV = 0x40000000;
    s->I3C01FCLKDIV = 0x40000000;
    s->I3C01FCLKSTSTCLKSEL = 0x0;
    s->WDT1FCLKSEL = 0x0;
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
    RTCLKCTL1State *s = RT_CLKCTL1(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &rt_clkctl1_ops, s,
                          TYPE_RT_CLKCTL1, 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
}

static const VMStateDescription vmstate_rt_clkctl1 = {
    .name = RT_CLKCTL1_NAME,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(PSCCTL0, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL1, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL2, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL0_SET, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL1_SET, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL2_SET, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL0_CLR, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL1_CLR, RTCLKCTL1State),
        VMSTATE_UINT32(PSCCTL2_CLR, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0CTL0, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0LOCKTIMEDIV2, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0NUM, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0DENOM, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLL0PFD, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOPLLCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(DSPCPUCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(DSPCPUCLKSELA, RTCLKCTL1State),
        VMSTATE_UINT32(DSPCPUCLKSELB, RTCLKCTL1State),
        VMSTATE_UINT32(OSEVENTTFCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG0CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG0CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC0FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG1CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG1CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC1FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG2CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG2CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC2FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG3CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG3CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC3FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG4CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG4CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC4FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG5CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG5CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC5FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG6CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG6CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC6FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG7CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG7CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC7FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG8CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG8CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC8FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG9CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG9CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC9FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG10CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG10CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC10FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG11CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG11CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC11FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG12CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG12CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC12FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG13CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG13CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC13FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG14CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG14CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC14FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG15CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG15CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC15FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG16CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG16CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FC16FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG17CLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FRG17CTL, RTCLKCTL1State),
        VMSTATE_UINT32(FLEXIOCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(FLEXIOCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(FRGPLLCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(DMIC0FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(DMIC0FCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(CT32BIT0FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(CT32BIT1FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(CT32BIT2FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(CT32BIT3FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(CT32BIT4FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOMCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(AUDIOMCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(CLKOUTSEL0, RTCLKCTL1State),
        VMSTATE_UINT32(CLKOUTSEL1, RTCLKCTL1State),
        VMSTATE_UINT32(CLKOUTFCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(I3C01FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(I3C01FCLKSTCSEL, RTCLKCTL1State),
        VMSTATE_UINT32(I3C01FCLKSTCDIV, RTCLKCTL1State),
        VMSTATE_UINT32(I3C01FCLKSDIV, RTCLKCTL1State),
        VMSTATE_UINT32(I3C01FCLKDIV, RTCLKCTL1State),
        VMSTATE_UINT32(I3C01FCLKSTSTCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(WDT1FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(ACMP0FCLKSEL, RTCLKCTL1State),
        VMSTATE_UINT32(ACMP0FCLKDIV, RTCLKCTL1State),
        VMSTATE_END_OF_LIST()
    }
};

static void rt_clkctl1_init(Object *obj)
{
    RTCLKCTL1State *s = RT_CLKCTL1(obj);

    memory_region_init_io(&s->iomem, obj, &rt_clkctl1_ops, s, TYPE_RT_CLKCTL1, 0x1000);
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
    .name          = TYPE_RT_CLKCTL1,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTCLKCTL1State),
    .instance_init = rt_clkctl1_init,
    .class_init    = rt_clkctl1_class_init,
};

static void rt_clkctl1_register_types(void)
{
    type_register_static(&rt_clkctl1_info);
}

type_init(rt_clkctl1_register_types)

