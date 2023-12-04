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

#ifndef NXP_RT685_H
#define NXP_RT685_H

#include "hw/arm/armsse.h"
#include "hw/arm/armsse-version.h"
#include "hw/boards.h"
#include "hw/i2c/rt_flexcomm_i2c.h"
#include "hw/sd/sdhci.h"

/*=======================================
    RW610 CM33 CORE module Start
 ========================================*/
#define RT685_M33_NUMIRQ  60
#define RT685_M33_SRAM_BANKS  30
#define RT685_M33_FLEXCOMM_PORTS 16
#define RT685_M33_OS_TIMER_COUNT 2
#define RT685_M33_USDHC_COUNT 2

typedef struct mem_region_t {
    uint32_t start;
    uint32_t size;
    uint32_t irq_num;
    uint32_t dma_hwtrg_nr;
} mem_region;

typedef struct rt685_memmap_tag {
    mem_region pflash;
    mem_region sram_part[RT685_M33_SRAM_BANKS];
    mem_region flexcomm[RT685_M33_FLEXCOMM_PORTS];
    mem_region rom;
    mem_region flexspi0;
    mem_region sdma;
    mem_region flexspi1;
    mem_region CACHE_Control_0;
    mem_region CACHE_Control_1;
    mem_region CLKCTL0;
    mem_region CLKCTL1;
    mem_region RSTCTL0;
    mem_region RSTCTL1;
    mem_region SYSCTL0;
    mem_region SYSCTL1;
    mem_region PMC;
    mem_region PUF;
    mem_region IOPCTL;
    mem_region PERIPHERAL_MUXES;
    mem_region flexspi;
    mem_region flexspi_ctl;
    mem_region lpadc;
    mem_region ostimer[RT685_M33_OS_TIMER_COUNT];
    mem_region sdio[RT685_M33_USDHC_COUNT];
    mem_region hs_gpio;
    mem_region sec_hs_gpio;
    mem_region pmc;
    mem_region mua;
    mem_region pint[8];

} rt685_m33_memmap_t;

typedef struct {
    MachineClass parent;
    const char *armsse_type;
    int uart_overflow_irq;
} RT685_M33_MachineClass;

typedef struct {
    MachineState parent;


    ARMSSE iotkit;    

    MemoryRegion pflash;
    MemoryRegion rom;
    MemoryRegion sram[RT685_M33_SRAM_BANKS];
    MemoryRegion sram_alias[RT685_M33_SRAM_BANKS];
    MemoryRegion sram_data_alias[RT685_M33_SRAM_BANKS];
    MemoryRegion sram_data_alias_ns[RT685_M33_SRAM_BANKS];
    MemoryRegion CACHE_Control_0;
    MemoryRegion CACHE_Control_1;
    MemoryRegion CLKCTL0;
    MemoryRegion CLKCTL1;
    MemoryRegion RSTCTL0;
    MemoryRegion RSTCTL1;
    MemoryRegion SYSCTL0;
    MemoryRegion SYSCTL1;
    MemoryRegion PMC;
    MemoryRegion IOPCTL;
    MemoryRegion PERIPHERAL_MUXES;
    MemoryRegion GPIO_INT;
    MemoryRegion flexspi_ns;
    MemoryRegion flexspi_s;
    MemoryRegion flexspi_ctl;
    MemoryRegion flexcomm_ns[RT685_M33_FLEXCOMM_PORTS];
    MemoryRegion flexcomm_s[RT685_M33_FLEXCOMM_PORTS];
    MemoryRegion lpadc;
    MemoryRegion lprtc;
    MemoryRegion pmc;
    MemoryRegion pint;
    MemoryRegion puf;
    MemoryRegion rom_ns;

    MemoryRegion CACHE_Control_0_s;
    MemoryRegion CACHE_Control_1_s;
    MemoryRegion CLKCTL0_s;
    MemoryRegion CLKCTL1_s;
    MemoryRegion RSTCTL0_s;
    MemoryRegion RSTCTL1_s;
    MemoryRegion SYSCTL0_s;
    MemoryRegion SYSCTL1_s;
    MemoryRegion PMC_s;
    MemoryRegion IOPCTL_s;
    MemoryRegion PERIPHERAL_MUXES_s;
    MemoryRegion GPIO_INT_s;
    MemoryRegion flexspi_ctl_s;
    MemoryRegion os_timer[RT685_M33_OS_TIMER_COUNT];
    MemoryRegion os_timer_sec[RT685_M33_OS_TIMER_COUNT];
    MemoryRegion lpadc_s;
    MemoryRegion lprtc_s;
    MemoryRegion pmc_s;
    MemoryRegion sdio_s[RT685_M33_USDHC_COUNT];
    MemoryRegion mua_s;
    MemoryRegion pint_s;
    MemoryRegion puf_s;
    MemoryRegion rom_s;

    uint32_t boot_base_addr;

    NotifierList dma_hwtrigger_notifiers;
    Notifier hwtrigger_notifier;

    RTFLEXCOMMI2CState * flexcomm_i2c_ctl[RT685_M33_FLEXCOMM_PORTS];
    SDHCIState sdio[RT685_M33_USDHC_COUNT];

    SplitIRQ sec_resp_splitter;
    Clock *sysclk;
    Clock *s32kclk;

    char *boot_rom_path;
} RT685_M33_MachineState;

#define TYPE_RT685_CM33_CORE "rt685-cm33-core"

#define TYPE_RT685_MACHINE "rt685"
#define TYPE_RT685_M33_MACHINE MACHINE_TYPE_NAME("rt685-m33")

#define RT685_M33_MACHINE(obj) \
    OBJECT_CHECK(RT685_M33_MachineState, obj, TYPE_RT685_MACHINE)
#define RT685_MACHINE_GET_CLASS(obj) \
    OBJECT_GET_CLASS(RT685_M33_MachineClass, obj, TYPE_RT685_MACHINE)
#define RT685_MACHINE_CLASS(klass) \
    OBJECT_CLASS_CHECK(RT685_M33_MachineClass, klass, TYPE_RT685_MACHINE)

/* Main SYSCLK frequency in Hz */
#define SYSCLK_FRQ 24000000
#define S32KCLK_FRQ (32 * 1000)

#endif
