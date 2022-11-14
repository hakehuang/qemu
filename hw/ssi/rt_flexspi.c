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
#include "hw/irq.h"
#include "hw/ssi/rt_flexspi.h"
#include "migration/vmstate.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef DEBUG_RT_FLEXSPI
#define DEBUG_RT_FLEXSPI 0
#endif

#define DPRINTF(fmt, args...) \
    do { \
        if (DEBUG_RT_FLEXSPI) { \
            fprintf(stderr, "[%s]%s: " fmt , TYPE_RT_FLEXSPI, \
                                             __func__, ##args); \
        } \
    } while (0)

static const VMStateDescription vmstate_rt_flexspi = {
    .name = TYPE_RT_FLEXSPI,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_FIFO32(tx_fifo, RTFLEXSPIState),
        VMSTATE_FIFO32(rx_fifo, RTFLEXSPIState),
        VMSTATE_UINT32_ARRAY(lut, RTFLEXSPIState, 64),
        VMSTATE_UINT32(MCR0, RTFLEXSPIState),
        VMSTATE_UINT32(MCR1, RTFLEXSPIState),
        VMSTATE_UINT32(MCR2, RTFLEXSPIState),
        VMSTATE_UINT32(AHBCR, RTFLEXSPIState),
        VMSTATE_UINT32(INTEN, RTFLEXSPIState),
        VMSTATE_UINT32(INTR, RTFLEXSPIState),
        VMSTATE_UINT32(LUTKEY, RTFLEXSPIState),
        VMSTATE_UINT32(LUTCR, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF0CR0, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF1CR0, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF2CR0, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF3CR0, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF4CR0, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF5CR0, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF6CR0, RTFLEXSPIState),
        VMSTATE_UINT32(AHBRXBUF7CR0, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHA1CR0, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHA2CR0, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHB1CR0, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHB2CR0, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHA1CR1, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHA2CR1, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHB1CR1, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHB2CR1, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHA1CR2, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHA2CR2, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHB1CR2, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHB2CR2, RTFLEXSPIState),
        VMSTATE_UINT32(FLSHCR4, RTFLEXSPIState),
        VMSTATE_UINT32(IPCR0, RTFLEXSPIState),
        VMSTATE_UINT32(IPCR1, RTFLEXSPIState),
        VMSTATE_UINT32(IPCMD, RTFLEXSPIState),
        VMSTATE_UINT32(DLPR, RTFLEXSPIState),
        VMSTATE_UINT32(IPRXFCR, RTFLEXSPIState),
        VMSTATE_UINT32(IPTXFCR, RTFLEXSPIState),
        VMSTATE_UINT32(DLLACR, RTFLEXSPIState),
        VMSTATE_UINT32(DLLBCR, RTFLEXSPIState),
        VMSTATE_UINT32(STS0, RTFLEXSPIState),
        VMSTATE_UINT32(STS1, RTFLEXSPIState),
        VMSTATE_UINT32(STS2, RTFLEXSPIState),
        VMSTATE_UINT32(AHBSPNDSTS, RTFLEXSPIState),
        VMSTATE_UINT32(IPRXFSTS, RTFLEXSPIState),
        VMSTATE_UINT32(IPTXFSTS, RTFLEXSPIState),
        VMSTATE_END_OF_LIST()
    },
};

static void rt_flexspi_txfifo_reset(RTFLEXSPIState *s)
{
    fifo32_reset(&s->tx_fifo);
}

static void rt_flexspi_rxfifo_reset(RTFLEXSPIState *s)
{
    fifo32_reset(&s->rx_fifo);
}

static void rt_flexspi_update_irq(RTFLEXSPIState *s)
{
    int level = 0; /* TODO  */

    qemu_set_irq(s->irq, level);

    DPRINTF("IRQ level is %d\n", level);
}

static void rt_flexspi_common_reset(RTFLEXSPIState *s)
{
    int i;

    s->MCR0 = 0xFFFF80C2;
    s->MCR1 = 0xFFFFFFFF;
    s->MCR2 = 0x200081F7;
    s->AHBCR = 0x00000018;
    s->INTEN = 0x00000000;
    s->INTR = 0x00000000;
    s->LUTKEY = 0x5AF05AF0;
    s->LUTCR = 0x00000002;
    s->AHBRXBUF0CR0 = 0x80000010;
    s->AHBRXBUF1CR0 = 0x80010010;
    s->AHBRXBUF2CR0 = 0x80020010;
    s->AHBRXBUF3CR0 = 0x80030010;
    s->AHBRXBUF4CR0 = 0x80040010;
    s->AHBRXBUF5CR0 = 0x80050010;
    s->AHBRXBUF6CR0 = 0x80060010;
    s->AHBRXBUF7CR0 = 0x80070010;
    s->FLSHA1CR0 = 0x00010000;
    s->FLSHA2CR0 = 0x00010000;
    s->FLSHB1CR0 = 0x00010000;
    s->FLSHB2CR0 = 0x00010000;
    s->FLSHA1CR1 = 0x00000063;
    s->FLSHA2CR1 = 0x00000063;
    s->FLSHB1CR1 = 0x00000063;
    s->FLSHB2CR1 = 0x00000063;
    s->FLSHA1CR2 = 0x00000000;
    s->FLSHA2CR2 = 0x00000000;
    s->FLSHB1CR2 = 0x00000000;
    s->FLSHB2CR2 = 0x00000000;
    s->FLSHCR4 = 0x00000000;
    s->IPCR0 = 0x00000000;
    s->IPCR1 = 0x00000000;
    s->IPCMD = 0x00000000;
    s->DLPR = 0x00000000;
    s->IPRXFCR = 0x00000000;
    s->IPTXFCR = 0x00000000;
    s->DLLACR = 0x00000100;
    s->DLLBCR = 0x00000100;
    s->STS0 = 0x00000002;
    s->STS1 = 0x00000000;
    s->STS2 = 0x01000100;
    s->AHBSPNDSTS = 0x00000000;
    s->IPRXFSTS = 0x00000000;
    s->IPTXFSTS = 0x00000000;

    rt_flexspi_rxfifo_reset(s);
    rt_flexspi_txfifo_reset(s);

    for (i=0; i< LUT_SIZE; i++)
    {
       s->lut[i] = 0;
    }
}

static void rt_flexspi_reset(DeviceState *dev)
{
    RTFLEXSPIState *s = RT_FLEXSPI(dev);

    rt_flexspi_common_reset(s);
    rt_flexspi_update_irq(s);
}

static uint64_t rt_flexspi_read(void *opaque, hwaddr offset, unsigned size)
{
    uint32_t value = 0;
    RTFLEXSPIState *s = opaque;

    switch(offset)
    {
        case 0x0:
        /* clear reset  */
        value = s->MCR0 & (~0x1);
        break;
        case 0x4:
        value = s->MCR1;
        break;
        case 0x8:
        value = s->MCR2 & (~0x4000);
        break;
        case 0xC:
        value = s->AHBCR;
        break;
        case 0x10:
        value = s->INTEN;
        break;
        case 0x14:
        value = s->INTR;
        break;
        case 0x18:
        value = s->LUTKEY;
        break;
        case 0x1C:
        value = s->LUTCR;
        break;
        case 0x20:
        value = s->AHBRXBUF0CR0;
        break;
        case 0x24:
        value = s->AHBRXBUF1CR0;
        break;
        case 0x28:
        value = s->AHBRXBUF2CR0;
        break;
        case 0x2C:
        value = s->AHBRXBUF3CR0;
        break;
        case 0x30:
        value = s->AHBRXBUF4CR0;
        break;
        case 0x34:
        value = s->AHBRXBUF5CR0;
        break;
        case 0x38:
        value = s->AHBRXBUF6CR0;
        break;
        case 0x3C:
        value = s->AHBRXBUF7CR0;
        break;
        case 0x60:
        value = s->FLSHA1CR0;
        break;
        case 0x64:
        value = s->FLSHA2CR0;
        break;
        case 0x68:
        value = s->FLSHB1CR0;
        break;
        case 0x6C:
        value = s->FLSHB2CR0;
        break;
        case 0x70:
        value = s->FLSHA1CR1;
        break;
        case 0x74:
        value = s->FLSHA2CR1;
        break;
        case 0x78:
        value = s->FLSHB1CR1;
        break;
        case 0x7C:
        value = s->FLSHB2CR1;
        break;
        case 0x80:
        value = s->FLSHA1CR2;
        break;
        case 0x84:
        value = s->FLSHA2CR2;
        break;
        case 0x88:
        value = s->FLSHB1CR2;
        break;
        case 0x8C:
        value = s->FLSHB2CR2;
        break;
        case 0x94:
        value = s->FLSHCR4;
        break;
        case 0xA0:
        value = s->IPCR0;
        break;
        case 0xA4:
        value = s->IPCR1;
        break;
        case 0xB0:
        value = s->IPCMD;
        break;
        case 0xB4:
        value = s->DLPR;
        break;
        case 0xB8:
        value = s->IPRXFCR;
        break;
        case 0xBC:
        value = s->IPTXFCR;
        break;
        case 0xC0:
        value = s->DLLACR;
        break;
        case 0xC4:
        value = s->DLLBCR;
        break;
        case 0xE0:
        /* ARBIDLE and SEQIDLE  */
        value = s->STS0 | (0x3);
        break;
        case 0xE4:
        value = s->STS1;
        break;
        case 0xE8:
        /* AREFLOCK and ASLVLOCK  */
        value = s->STS2 | (0x3);
        break;
        case 0xEC:
        value = s->AHBSPNDSTS;
        break;
        case 0xF0:
        value = s->IPRXFSTS;
        break;
        case 0xF4:
        value = s->IPTXFSTS;
        break;	
        default:
        break;
    }

    return (uint64_t)value;
}

static void rt_flexspi_write(void *opaque, hwaddr offset, uint64_t value,
                           unsigned size)
{
    RTFLEXSPIState *s = opaque;
    uint32_t v32 = (uint32_t)value;

    switch(offset)
    {
    case 0x0:
    s->MCR0 = v32;
    break;
    case 0x4:
    s->MCR1 = v32;
    break;
    case 0x8:
    s->MCR2 = v32;
    break;
    case 0xC:
    s->AHBCR = v32;
    break;
    case 0x10:
    s->INTEN = v32;
    break;
    case 0x14:
    s->INTR = s->INTR & (~(v32 & 0x1FFF));
    /* AHBCMDERR is set clear err and error code */
    if (v32 & 0x10 ) {
        s->STS1 = s->STS1 & (~0xF0F);
    }
    break;
    case 0x18:
    s->LUTKEY = v32;
    break;
    case 0x1C:
    s->LUTCR = v32;
    break;
    case 0x20:
    s->AHBRXBUF0CR0 = v32;
    break;
    case 0x24:
    s->AHBRXBUF1CR0 = v32;
    break;
    case 0x28:
    s->AHBRXBUF2CR0 = v32;
    break;
    case 0x2C:
    s->AHBRXBUF3CR0 = v32;
    break;
    case 0x30:
    s->AHBRXBUF4CR0 = v32;
    break;
    case 0x34:
    s->AHBRXBUF5CR0 = v32;
    break;
    case 0x38:
    s->AHBRXBUF6CR0 = v32;
    break;
    case 0x3C:
    s->AHBRXBUF7CR0 = v32;
    break;
    case 0x60:
    s->FLSHA1CR0 = v32;
    break;
    case 0x64:
    s->FLSHA2CR0 = v32;
    break;
    case 0x68:
    s->FLSHB1CR0 = v32;
    break;
    case 0x6C:
    s->FLSHB2CR0 = v32;
    break;
    case 0x70:
    s->FLSHA1CR1 = v32;
    break;
    case 0x74:
    s->FLSHA2CR1 = v32;
    break;
    case 0x78:
    s->FLSHB1CR1 = v32;
    break;
    case 0x7C:
    s->FLSHB2CR1 = v32;
    break;
    case 0x80:
    s->FLSHA1CR2 = v32;
    break;
    case 0x84:
    s->FLSHA2CR2 = v32;
    break;
    case 0x88:
    s->FLSHB1CR2 = v32;
    break;
    case 0x8C:
    s->FLSHB2CR2 = v32;
    break;
    case 0x94:
    s->FLSHCR4 = v32;
    break;
    case 0xA0:
    s->IPCR0 = v32;
    break;
    case 0xA4:
    s->IPCR1 = v32;
    break;
    case 0xB0:
    s->IPCMD = v32;
    break;
    case 0xB4:
    s->DLPR = v32;
    break;
    case 0xB8:
    s->IPRXFCR = v32;
    break;
    case 0xBC:
    s->IPTXFCR = v32;
    break;
    case 0xC0:
    s->DLLACR = v32;
    break;
    case 0xC4:
    s->DLLBCR = v32;
    break;
    case 0xE0:
    break;
    case 0xE4:
    break;
    case 0xE8:
    break;
    case 0xEC:
    break;
    case 0xF0:
    break;
    case 0xF4:
    break;
    default:
    break;
    } 
}

static const struct MemoryRegionOps rt_flexspi_ops = {
    .read = rt_flexspi_read,
    .write = rt_flexspi_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void rt_flexspi_realize(DeviceState *dev, Error **errp)
{
    RTFLEXSPIState *s = RT_FLEXSPI(dev);

    s->bus = ssi_create_bus(dev, "spi");

    memory_region_init_io(&s->iomem, OBJECT(dev), &rt_flexspi_ops, s,
                          TYPE_RT_FLEXSPI, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iomem);
    sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq);

    fifo32_create(&s->tx_fifo, FLEXSPI_FIFO_SIZE);
    fifo32_create(&s->rx_fifo, FLEXSPI_FIFO_SIZE);
}

static void rt_flexspi_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = rt_flexspi_realize;
    dc->vmsd = &vmstate_rt_flexspi;
    dc->reset = rt_flexspi_reset;
    dc->desc = "NXP RT flexspi Controller";
}

static const TypeInfo rt_flexspi_info = {
    .name          = TYPE_RT_FLEXSPI,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RTFLEXSPIState),
    .class_init    = rt_flexspi_class_init,
};

static void rt_flexspi_register_types(void)
{
    type_register_static(&rt_flexspi_info);
}

type_init(rt_flexspi_register_types)
