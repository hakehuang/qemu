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

#ifndef RT_FLEXCOMM_H
#define RT_FLEXCOMM_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"
#include "qemu/bitops.h"
#include "qemu/fifo8.h"


#define PERSEL_MASK 0x7
#define USARTPRESE_MASK 0x10
#define USARTD_TXDIS_MASK 0x40
#define USART_STAT_TXNOTFULL_MASK  0x20
#define USART_STAT_RXNOTEMPTY_MASK 0x40
#define USART_STAT_RXFULL_MASK     0x80
#define USART_STAT_TXEMPTY_MASK    0x10

#define USART_FIFOINTSTAT_RXLVL    0x80
#define USART_FIFOINTSTAT_TXLVL    0x40

#define USART_FIFOTRIG_RXLVL_MASK  0xF0000
#define USART_FIFOTRIG_RXLVL_OFFSET (16U)
#define FIFOTRIG_RXLVL(s) ((s->USART_FIFOTRIG&USART_FIFOTRIG_RXLVL_MASK)>>USART_FIFOTRIG_RXLVL_OFFSET)

#define USART_FIFOTRIG_TXLVL_MASK  0xF00
#define USART_FIFOTRIG_TXLVL_OFFSET (8U)
#define FIFOTRIG_TXLVL(s) ((s->USART_FIFOTRIG&USART_FIFOTRIG_TXLVL_MASK)>>USART_FIFOTRIG_TXLVL_OFFSET)

#define TYPE_RT_FLEXCOMM "rt.flexcomm"
OBJECT_DECLARE_SIMPLE_TYPE(RTFLEXCOMMState, RT_FLEXCOMM)

struct RTFLEXCOMMState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion iomem;

    uint32_t USART_CFG;
    uint32_t USART_CTL;
    uint32_t USART_STAT;
    uint32_t USART_INTENSET;
    uint32_t USART_INTENCLR;
    uint32_t USART_BRG;
    uint32_t USART_INTSTAT;
    uint32_t USART_OSR;
    uint32_t USART_ADDR;
    uint32_t USART_FIFOCFG;
    uint32_t USART_FIFOSTAT;
    uint32_t USART_FIFOTRIG;
    uint32_t USART_FIFOINTENSET;
    uint32_t USART_FIFOINTENCLR;
    uint32_t USART_FIFOINTSTAT;
    uint32_t USART_FIFOWR;
    uint32_t USART_FIFORD;
    uint32_t USART_FIFORDNOPOP;
    uint32_t USART_FIFOSIZE;
    uint32_t PSELID;
    uint32_t ID;
    uint32_t FIFO_SIZE;

    Fifo8 * RXFIFO;
    Fifo8 * TXFIFO;
    CharBackend chr;
    qemu_irq irq;
};

#endif
