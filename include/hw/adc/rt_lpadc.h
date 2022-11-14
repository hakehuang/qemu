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

#ifndef HW_RT_LPADC_H
#define HW_RT_LPADC_H

#include "hw/sysbus.h"
#include "qom/object.h"
#include "qemu/fifo32.h"
#include "qemu/timer.h"

#define TST_REG_NUM  16
#define ADC_COMMAND_BUF_NUM 15
#define FIFO_CTL_NUM 2
#define ADC_CMP_VALUE_NUM 4

#define RESFIFO_SIZE 16

#define TYPE_RT_LPADC "rt-lpadc"
OBJECT_DECLARE_SIMPLE_TYPE(RTLPADCState, RT_LPADC)

struct RTLPADCState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t VERID;
    uint32_t PARAM;
    uint32_t CTRL;
    uint32_t STAT;
    uint32_t IE;
    uint32_t DE;
    uint32_t CFG;
    uint32_t PAUSE;
    uint32_t SWTRIG;
    uint32_t TSTAT;
    uint32_t TCTRL[TST_REG_NUM];
    uint32_t FCTRL[FIFO_CTL_NUM];
    uint32_t CMDL[ADC_COMMAND_BUF_NUM];
    uint32_t CMDH[ADC_COMMAND_BUF_NUM];
    uint32_t CV[ADC_CMP_VALUE_NUM];
    Fifo32 RESFIFO[FIFO_CTL_NUM];


    uint32_t timer_interval;
    QEMUTimer *timer;
    uint32_t dma_trigger_nr;
    uint32_t state;

    qemu_irq irq;
};

/* CTL register */
#define RSTFIFO1_MASK  (1U<<9)
#define RSTFIFO0_MASK  (1U<<8)
#define DOZEN_MASK     (1U<<2)
#define RST_MASK       (1U<<1)
#define ADCEN_MASK     1U

/* STAT register */
#define CMDACT_OFFSET  (24U)
#define CMDACT_MASK    (0xF<<CMDACT_OFFSET)
#define TRGACT_OFFSET  16U
#define TRGACT_MASK    (0xFU<<TRGACT_OFFSET)
#define ADC_ACTIVE_MASK (1U<<11)
#define TCOMP_INT_OFFSET (9U)
#define TCOMP_INT_MASK (1U<<TCOMP_INT_OFFSET)
#define TEXC_INT_OFFSET (8U)
#define TEXC_INT_MASK  (1U<<TEXC_INT_OFFSET)
#define FOF1_OFFSET    (3U)
#define FOF1_MASK      (1U<<FOF1_OFFSET)
#define RDY1_OFFSET    (2U)
#define RDY1_MASK      (1U<<RDY1_OFFSET)
#define FOF0_OFFSET    (1U)
#define FOF0_MASK      (1U<<FOF0_OFFSET)
#define RDY0_OFFSET    (0U)
#define RDY0_MASK      (1U<<RDY0_OFFSET)

/* IE register*/
#define TCOMP_IE_MASK  (0xFFU<<16)
#define TEXC_IE_MASK   (1U<<9)
#define FOFIE1_MASK    (1U<<3)
#define FWMIE1_MASK    (1U<<2)
#define FOFIE0_MASK    (1U<<1)
#define FWMIE0_MASK    (1U)

/* DE register */
#define FWMDE1_MASK    (1U<<1)
#define FWMDE0_MASK    (1U)

/* CFG register */
#define HPT_EXDI_MASK  (1U<<10)
#define TCMDRES_MASK   (1U<<9)
#define TRES_MASK      (1U<<8)
#define TPRICTRL_MASK  (1U)

/* PAUSE register */
#define PAUSEEN_MASK   (1U<<31)
#define PAUSEDLY_MASK   (0x1FFU)

/* FIFO CTL register */
#define FWMARK_OFFSET  (16U)
#define FWMARK_MASK    (0xFU<<FWMARK_OFFSET)
#define FCOUNT_MASK    (0x1FU)

/* TCTRL register  */
#define TCMD_OFFSET       24U
#define TCMD_MASK         (0xFU<<TCMD_OFFSET)
#define FIFO_SEL_A_OFFSET 1
#define FIFO_SEL_A_MASK (1U<<FIFO_SEL_A_OFFSET)

/* TCTRH register */
#define NEXT_OFFSET 24U
#define NEXT_MASK   (0xFU<<NEXT_OFFSET)

/* RESFIFO register*/
#define VALID_OFFSET 32U
#define VALID_MASK   (1U<<VALID_OFFSET)
#define CMDSRC_OFFSET 24U
#define CMDSRC_MASK  (0xF<<CMDSRC_OFFSET)
#define LOOPCNT_OFFSET 20U
#define LOOPCNT_MASK (0xF<<LOOPCNT_OFFSET)
#define TSRC_OFFSET  16U
#define TSRC_MASK    (0xF<<TSRC_OFFSET)

#define FROM_REG(R, F)    ((R&F##_MASK)>>(F##_OFFSET))
#define TO_REG(R, F, V)   ((R&(~F##_MASK))|(V<<(F##_OFFSET)))

typedef enum { 
    eLPADC_Run = 1000,
    eLPADC_Stop = 2000,
    eLPADC_Dozen = 3000,
} eLPADCState;


#endif /* HW_RT_LPADC_H */
