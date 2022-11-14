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

#ifndef RT_FLEXSPI_H
#define RT_FLEXSPI_H

#include "hw/sysbus.h"
#include "hw/ssi/ssi.h"
#include "qemu/bitops.h"
#include "qemu/fifo32.h"
#include "qom/object.h"


#define TYPE_RT_FLEXSPI "rt.flexspi"
OBJECT_DECLARE_SIMPLE_TYPE(RTFLEXSPIState, RT_FLEXSPI)

#define LUT_SIZE 64
#define FLEXSPI_FIFO_SIZE 32

struct RTFLEXSPIState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion iomem;

    qemu_irq irq;

    SSIBus *bus;

    uint32_t MCR0;
    uint32_t MCR1;
    uint32_t MCR2;
    uint32_t AHBCR;
    uint32_t INTEN;
    uint32_t INTR;
    uint32_t LUTKEY;
    uint32_t LUTCR;
    uint32_t AHBRXBUF0CR0;
    uint32_t AHBRXBUF1CR0;
    uint32_t AHBRXBUF2CR0;
    uint32_t AHBRXBUF3CR0;
    uint32_t AHBRXBUF4CR0;
    uint32_t AHBRXBUF5CR0;
    uint32_t AHBRXBUF6CR0;
    uint32_t AHBRXBUF7CR0;
    uint32_t FLSHA1CR0;
    uint32_t FLSHA2CR0;
    uint32_t FLSHB1CR0;
    uint32_t FLSHB2CR0;
    uint32_t FLSHA1CR1;
    uint32_t FLSHA2CR1;
    uint32_t FLSHB1CR1;
    uint32_t FLSHB2CR1;
    uint32_t FLSHA1CR2;
    uint32_t FLSHA2CR2;
    uint32_t FLSHB1CR2;
    uint32_t FLSHB2CR2;
    uint32_t FLSHCR4;
    uint32_t IPCR0;
    uint32_t IPCR1;
    uint32_t IPCMD;
    uint32_t DLPR;
    uint32_t IPRXFCR;
    uint32_t IPTXFCR;
    uint32_t DLLACR;
    uint32_t DLLBCR;
    uint32_t STS0;
    uint32_t STS1;
    uint32_t STS2;
    uint32_t AHBSPNDSTS;
    uint32_t IPRXFSTS;
    uint32_t IPTXFSTS;

    Fifo32 rx_fifo;
    Fifo32 tx_fifo;
    uint32_t lut[LUT_SIZE];
};

#endif /* RT_FLEXSPI_H */
