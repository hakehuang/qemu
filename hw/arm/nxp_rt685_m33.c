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
#include "qapi/error.h"
#include "qemu/dma_notifier.h" 
#include "qemu/error-report.h"
#include "qemu/module.h"
#include "qemu/notify.h"
#include "qemu/units.h"
#include "qemu/cutils.h"
#include "qemu/timer.h"
#include "migration/vmstate.h"
#include "hw/qdev-core.h"
#include "hw/sysbus.h"
#include "hw/char/serial.h"
#include "hw/misc/unimp.h"
#include "hw/usb/hcd-ehci.h"
#include "hw/loader.h"
#include "sysemu/sysemu.h"
#include "hw/qdev-clock.h"
#include "sysemu/hostmem.h"
#include "hw/adc/rt_lpadc.h"
#include "hw/arm/boot.h"
#include "hw/arm/nxp_rt685.h"
#include "hw/misc/rt_sysctl.h"
#include "hw/misc/rt_rstctl.h"
#include "hw/misc/rt_clkctl.h"
#include "hw/misc/rt_pmc.h"
#include "hw/ssi/rt_flexspi.h"
#include "hw/char/rt_flexcomm.h"
#include "hw/i2c/rt_flexcomm_i2c.h"
#include "hw/timer/rt_ostimer.h"
#include "hw/misc/fxos8700.h"
#include "hw/misc/pca9420.h"
#include "hw/misc/dbus_client.h"
#include "hw/gpio/rt_pint.h"

static rt685_m33_memmap_t _memmap;

static  qemu_irq rt685_irq_init(void * opaque, int n, int cpu_id);

#define RT685_SDIO_CAPABILITIES 0x057834b4

/*=======================================
    RT685 M33 CORE module Start
 ========================================*/
static void rt685_m33_mmap_init(MachineState *machine)
{
    int i = 0;
    int j = 0;
    uint32_t base = 0x0;

    _memmap.pflash.start = 0x00000000;
    for(i = 0; i < 8; i++)
    {
        _memmap.sram_part[i].start = 0x0 + (0x8000 * i);
        _memmap.sram_part[i].size = 0x8000; /* 32K  */
    }
    base = 0x40000;
    for(i = 8, j = 0; i < 12; i++, j++)
    {
        _memmap.sram_part[i].start = (0x10000 * j) + base;
        _memmap.sram_part[i].size = 0x10000; /* 64K */
    }
    base = 0x80000;
    for(i = 12, j = 0; i < 16; i++, j++)
    {
        _memmap.sram_part[i].start = (0x20000 * j) + base;
        _memmap.sram_part[i].size = 0x20000; /* 128K */
    }
    base = 0x100000;
    for(i = 16, j = 0; i < RT685_M33_SRAM_BANKS; i++, j++)
    {
        _memmap.sram_part[i].start = (0x40000 * j) + base;
        _memmap.sram_part[i].size = 0x40000; /* 256K */
    }
    _memmap.rom.start = 0x03000000;
    _memmap.rom.size = 0x40000; /* 256KiB */
    _memmap.RSTCTL0.start = 0x40000000;
    _memmap.RSTCTL0.size = 0x1000;
    _memmap.CLKCTL0.start = 0x40001000;
    _memmap.CLKCTL0.size = 0x1000;
    _memmap.SYSCTL0.start = 0x40002000;
    _memmap.SYSCTL0.size = 0x1000;
    _memmap.PUF.start = 0x40006000;
    _memmap.PUF.size = 0x1000;
    _memmap.RSTCTL1.start = 0x40020000;
    _memmap.RSTCTL1.size = 0x1000;
    _memmap.CLKCTL1.start = 0x40021000;
    _memmap.CLKCTL1.size = 0x1000;
    _memmap.SYSCTL1.start = 0x40022000;
    _memmap.SYSCTL1.size = 0x1000;
    _memmap.PMC.start = 0x40135000;
    _memmap.PMC.size = 0x1000;
    _memmap.flexspi.start = 0x08000000;
    _memmap.flexspi.size = 0x08000000;
    _memmap.flexspi_ctl.start = 0x40134000;
    _memmap.flexspi_ctl.size = 0x1000;
    _memmap.flexcomm[0].start = 0x40106000;
    _memmap.flexcomm[0].size  = 0x1000;
    _memmap.flexcomm[0].irq_num   = 14;
    _memmap.flexcomm[1].start = 0x40107000;
    _memmap.flexcomm[1].size  = 0x1000;
    _memmap.flexcomm[1].irq_num   = 15;
    _memmap.flexcomm[2].start = 0x40108000;
    _memmap.flexcomm[2].size  = 0x1000;
    _memmap.flexcomm[2].irq_num   = 16;
    _memmap.flexcomm[3].start = 0x40109000;
    _memmap.flexcomm[3].size  = 0x1000;
    _memmap.flexcomm[3].irq_num   = 17;
    _memmap.flexcomm[4].start = 0x40122000;
    _memmap.flexcomm[4].size  = 0x1000;
    _memmap.flexcomm[4].irq_num   = 18;
    _memmap.flexcomm[5].start = 0x40123000;
    _memmap.flexcomm[5].size  = 0x1000;
    _memmap.flexcomm[5].irq_num   = 19;
    _memmap.flexcomm[6].start = 0x40124000;
    _memmap.flexcomm[6].size  = 0x1000;
    _memmap.flexcomm[6].irq_num   = 43;
    _memmap.flexcomm[7].start = 0x40125000;
    _memmap.flexcomm[7].size  = 0x1000;
    _memmap.flexcomm[7].irq_num   = 44;
    _memmap.flexcomm[8].start = 0x0;
    _memmap.flexcomm[8].size  = 0x0;
    _memmap.flexcomm[8].irq_num   = 0;
    _memmap.flexcomm[9].start = 0x0;
    _memmap.flexcomm[9].size  = 0x0;
    _memmap.flexcomm[9].irq_num   = 0;
    _memmap.flexcomm[10].start = 0x0;
    _memmap.flexcomm[10].size  = 0x0;
    _memmap.flexcomm[10].irq_num   = 0;
    _memmap.flexcomm[11].start = 0x0;
    _memmap.flexcomm[11].size  = 0x0;
    _memmap.flexcomm[11].irq_num   = 0;
    _memmap.flexcomm[12].start = 0x0;
    _memmap.flexcomm[12].size  = 0x0;
    _memmap.flexcomm[12].irq_num   = 0;
    _memmap.flexcomm[13].start = 0x0;
    _memmap.flexcomm[13].size  = 0x0;
    _memmap.flexcomm[13].irq_num   = 0;
    _memmap.flexcomm[14].start = 0x40126000;
    _memmap.flexcomm[14].size  = 0x1000;
    _memmap.flexcomm[14].irq_num   = 20;
    _memmap.flexcomm[15].start = 0x40127000;
    _memmap.flexcomm[15].size  = 0x1000;
    _memmap.flexcomm[15].irq_num   = 21;
    _memmap.lpadc.start        = 0x4013A000;
    _memmap.lpadc.size         = 0x1000;
    _memmap.lpadc.irq_num      = 22;
    _memmap.lpadc.dma_hwtrg_nr = 24;
    _memmap.ostimer[0].start   = 0x40113000;
    _memmap.ostimer[0].irq_num = 41;
    _memmap.sdio[0].start     = 0x40136000;
    _memmap.sdio[0].size      = 0x1000;
    _memmap.sdio[0].irq_num   = 45;
    _memmap.sdio[1].start     = 0x40137000;
    _memmap.sdio[1].size      = 0x1000;
    _memmap.sdio[1].irq_num   = 46;
    _memmap.pmc.start         = 0x40135000;
    _memmap.pmc.size          = 0x1000;
    _memmap.pmc.irq_num       = 58;
    _memmap.mua.start         = 0x40110000;
    _memmap.mua.size          = 0x1000;
    _memmap.mua.irq_num       = 34;
    _memmap.pint[0].start        = 0x40025000;
    _memmap.pint[0].size          = 0x1000;
    _memmap.pint[0].irq_num       = 4;
    _memmap.pint[1].irq_num       = 5;
    _memmap.pint[2].irq_num       = 6;
    _memmap.pint[3].irq_num       = 7;
    _memmap.pint[4].irq_num       = 35;
    _memmap.pint[5].irq_num       = 36;
    _memmap.pint[6].irq_num       = 37;
    _memmap.pint[7].irq_num       = 38;

}

static void rt685_dma_hwtrigger_req(Notifier *n, void *opaque)
{
    /* move to dma mux maybe better */
    /* do DMA here*/
}



/*
 * Create an alias region in @container of @size bytes starting at @base
 * which mirrors the memory starting at @orig.
 */
static void make_alias(MemoryRegion *mr, MemoryRegion *container,
                       const char *name, hwaddr base, hwaddr size, hwaddr orig)
{
    memory_region_init_alias(mr, NULL, name, container, orig, size);
    /* The alias is even lower priority than unimplemented_device regions */
    memory_region_add_subregion_overlap(container, base, mr, -1500);
}

static void rt685_resolve_machine_pint(RT685_M33_MachineState *mms)
{
    qemu_irq irq_0 = rt685_irq_init(mms, _memmap.pint[0].irq_num, 0);
    qemu_irq irq_1 = rt685_irq_init(mms, _memmap.pint[1].irq_num, 0);
    qemu_irq irq_2 = rt685_irq_init(mms, _memmap.pint[2].irq_num, 0);
    qemu_irq irq_3 = rt685_irq_init(mms, _memmap.pint[3].irq_num, 0);
    qemu_irq irq_4 = rt685_irq_init(mms, _memmap.pint[4].irq_num, 0);
    qemu_irq irq_5 = rt685_irq_init(mms, _memmap.pint[5].irq_num, 0);
    qemu_irq irq_6 = rt685_irq_init(mms, _memmap.pint[6].irq_num, 0);
    qemu_irq irq_7 = rt685_irq_init(mms, _memmap.pint[7].irq_num, 0);

    sysbus_create_varargs(TYPE_RT_PINT, (hwaddr)_memmap.pint[0].start, irq_0,
        irq_1, irq_2, irq_3, irq_4, irq_5, irq_6, irq_7, NULL);
    make_alias(&mms->pint_s, get_system_memory(), "pint sec", 0x10000000 + _memmap.pint[0].start,
                _memmap.pint[0].size, _memmap.pint[0].start);
}

static void rt685_resolve_machine_mu(RT685_M33_MachineState *mms)
{
    qemu_irq irq = rt685_irq_init(mms, _memmap.mua.irq_num, 0);

    sysbus_create_simple(TYPE_DBUS_CLIENT_MUA, (hwaddr)_memmap.mua.start, irq);
    make_alias(&mms->mua_s, get_system_memory(), "mua sec", 0x10000000 + _memmap.mua.start,
                _memmap.mua.size, _memmap.mua.start);
}

static void rt685_resolve_machine_sdio(RT685_M33_MachineState *mms, uint32_t sdio_id)
{
    char *sdio_name = g_strdup_printf("sdio%d", sdio_id);
    char *sdio_name_s = g_strdup_printf("sdio%d_s", sdio_id);
    qemu_irq irq = rt685_irq_init(mms, _memmap.sdio[sdio_id].irq_num, 0);

     object_initialize_child(OBJECT(mms), sdio_name, &mms->sdio[sdio_id], TYPE_IMX_USDHC);
    //object_initialize_child(OBJECT(mms), sdio_name, &mms->sdio[sdio_id], TYPE_SYSBUS_SDHCI);
    /* UHS-I SDIO3.0 SDR104 1.8V ADMA */
    object_property_set_uint(OBJECT(&mms->sdio[sdio_id]), "sd-spec-version", 3,
                             &error_abort);
    object_property_set_uint(OBJECT(&mms->sdio[sdio_id]), "capareg",
                             RT685_SDIO_CAPABILITIES, &error_abort);
    object_property_set_uint(OBJECT(&mms->sdio[sdio_id]), "vendor",
                             SDHCI_VENDOR_IMX, &error_abort);
    if (!sysbus_realize(SYS_BUS_DEVICE(&mms->sdio[sdio_id]), &error_fatal)) {
        return;
    }
    sysbus_connect_irq(SYS_BUS_DEVICE(&mms->sdio[sdio_id]), 0, irq);
    sysbus_mmio_map(SYS_BUS_DEVICE(&mms->sdio[sdio_id]), 0, _memmap.sdio[sdio_id].start);
    g_free(sdio_name);

    make_alias(&mms->sdio_s[sdio_id], get_system_memory(), sdio_name_s, 0x10000000 + _memmap.sdio[sdio_id].start,
            _memmap.sdio[sdio_id].size, _memmap.sdio[sdio_id].start);
    g_free(sdio_name_s);

    BusState *bus;
    DeviceState *carddev;
    DriveInfo *di;
    BlockBackend *blk;

    di = drive_get(IF_SD, 0, sdio_id);
    blk = di ? blk_by_legacy_dinfo(di) : NULL;
    bus = qdev_get_child_bus(DEVICE(&mms->sdio[sdio_id]), "sd-bus");
    carddev = qdev_new(TYPE_SD_CARD);
    qdev_prop_set_drive_err(carddev, "drive", blk, &error_fatal);
    qdev_realize_and_unref(carddev, bus, &error_fatal);
}

static void rt685_resolve_machine_flexspi_nor(RT685_M33_MachineState *mms)
{
    HostMemoryBackend *backend;

    //memory_region_init_ram(&mms->flexspi_ahb, NULL, "flexspi_ahb.ram", 0x08000000, &error_fatal);
    //memory_region_add_subregion(system_memory, m_imx9_memmap.flexspi_ahb_start, &mms->flexspi_ahb);
    /* hardcode dtcm backend id */
    backend = (HostMemoryBackend *)object_resolve_path_type("rt685_flexspi_nor",
                      TYPE_MEMORY_BACKEND, NULL);
  
    if (!backend) {
        info_report("[ROM_EMU] not backend for flexspi nor found, use mem fallback");
        memory_region_init_ram(&mms->flexspi_ns, NULL, "flexspi", _memmap.flexspi.size, &error_fatal); /* 128M */
        memory_region_add_subregion(get_system_memory(), _memmap.flexspi.start, &mms->flexspi_ns);
        return;
    }

    MemoryRegion *ret = host_memory_backend_get_memory(backend);

    if (host_memory_backend_is_mapped(backend)) {
        error_report("memory backend %s can't be used multiple times.",
                     object_get_canonical_path_component(OBJECT(backend)));
        exit(EXIT_FAILURE);
    }
    host_memory_backend_set_mapped(backend, true);
    vmstate_register_ram_global(ret);
    memory_region_add_subregion(get_system_memory(), _memmap.flexspi.start, ret);
    info_report("[ROM_EMU] backend for flexspi nor mapped");
    return;
}

static I2CBus *rt685_i2c_get_bus(RT685_M33_MachineState *soc, uint32_t num)
{
    BusState * bus;
    g_assert(soc->flexcomm_i2c_ctl[num]!= NULL);
    bus = qdev_get_child_bus(DEVICE(soc->flexcomm_i2c_ctl[num]), "i2c-bus");
    return I2C_BUS(bus);
}

static void rt685_evk_i2c_init(RT685_M33_MachineState *soc)
{
    /* fxos8700  */
    i2c_slave_create_simple(rt685_i2c_get_bus(soc, 2), TYPE_FXOS8700, (0x1e<<1));
    /* pmic */
    i2c_slave_create_simple(rt685_i2c_get_bus(soc, 15), TYPE_PCA9420, (0x61<<1));
}

static  qemu_irq rt685_irq_init(void * opaque, int n, int cpu_id)
{
    RT685_M33_MachineState *mms = RT685_M33_MACHINE(opaque);
    ARMSSE *s = ARM_SSE(&mms->iotkit);

    qemu_irq irq = qdev_get_gpio_in(DEVICE(&s->armv7m[cpu_id]), n);
    return irq;
}

static void rt685_resolve_flexcomm_uart(RT685_M33_MachineState *mms, int flexcomm_num, int uart_id)
{
    char *flexcomm_name = g_strdup_printf("flexcomm%d_s", flexcomm_num);
    qemu_irq irq = rt685_irq_init(mms, _memmap.flexcomm[flexcomm_num].irq_num, 0);
    Chardev *chrdrv = serial_hd(uart_id);
    DeviceState * dev = qdev_new(TYPE_RT_FLEXCOMM);

    qdev_prop_set_chr(dev, "chardev", chrdrv);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, (hwaddr)_memmap.flexcomm[flexcomm_num].start);
    //sysbus_create_simple(TYPE_RT_FLEXCOMM, (hwaddr)_memmap.flexcomm[flexcomm_num].start, irq);
    make_alias(&mms->flexcomm_s[flexcomm_num], get_system_memory(), flexcomm_name, 0x10000000 + _memmap.flexcomm[flexcomm_num].start,
            _memmap.flexcomm[flexcomm_num].size, _memmap.flexcomm[flexcomm_num].start);
    g_free(flexcomm_name);
}

static void rt685_resolve_flexcomm_i2c(RT685_M33_MachineState *mms, int flexcomm_num)
{
    char *flexcomm_name = g_strdup_printf("flexcomm%d_s", flexcomm_num);
    qemu_irq irq = rt685_irq_init(mms, _memmap.flexcomm[flexcomm_num].irq_num, 0);

    mms->flexcomm_i2c_ctl[flexcomm_num] = (RTFLEXCOMMI2CState *)sysbus_create_simple(TYPE_RT_FLEXCOMMI2C,
                    (hwaddr)_memmap.flexcomm[flexcomm_num].start, irq);
    make_alias(&mms->flexcomm_s[flexcomm_num], get_system_memory(), flexcomm_name,
                0x10000000 + _memmap.flexcomm[flexcomm_num].start,
            _memmap.flexcomm[flexcomm_num].size, _memmap.flexcomm[flexcomm_num].start);
    g_free(flexcomm_name);
}


static void rt685_resolve_lpadc(RT685_M33_MachineState *mms, int id)
{
    char *lpadc_name = g_strdup_printf("lpadc%d_s", id);
    qemu_irq irq = rt685_irq_init(mms, _memmap.lpadc.irq_num, 0);

    sysbus_create_simple(TYPE_RT_LPADC, (hwaddr)_memmap.lpadc.start, irq);
    make_alias(&mms->lpadc_s, get_system_memory(), lpadc_name, 0x10000000 + _memmap.lpadc.start,
                _memmap.lpadc.size, _memmap.lpadc.start);
    g_free(lpadc_name);
}

static void rt685_resolve_ostimer(RT685_M33_MachineState *mms, int id)
{
    char *ostimer_name = g_strdup_printf("ostimer%d_s", id);
    qemu_irq irq = rt685_irq_init(mms, _memmap.ostimer[id].irq_num, 0);

    sysbus_create_simple(TYPE_RT_OSTIMER, (hwaddr)_memmap.ostimer[id].start, irq);
    make_alias(&mms->os_timer_sec[id], get_system_memory(), ostimer_name, 0x10000000 + _memmap.ostimer[id].start,
                _memmap.ostimer[id].size, _memmap.ostimer[id].start);
    g_free(ostimer_name);
}

static void rt685_resolve_pmc(RT685_M33_MachineState *mms, int id)
{
    char *pmc_name = g_strdup_printf("pmc%d_s", id);
    qemu_irq irq = rt685_irq_init(mms, _memmap.pmc.irq_num, 0);

    sysbus_create_simple(TYPE_RT_PMC, (hwaddr)_memmap.pmc.start, irq);
    make_alias(&mms->pmc_s, get_system_memory(), pmc_name, 0x10000000 + _memmap.pmc.start,
                _memmap.pmc.size, _memmap.pmc.start);
    g_free(pmc_name);
}

static void rt685_m33_common_init(MachineState *machine)
{
    RT685_M33_MachineState *mms = RT685_M33_MACHINE(machine);
    MachineClass *mmc = MACHINE_GET_CLASS(machine);
    RT685_M33_MachineClass *mc = RT685_MACHINE_GET_CLASS(mms);
    MemoryRegion *system_memory = get_system_memory();
    DeviceState *iotkitdev;

    if (strcmp(machine->cpu_type, mmc->default_cpu_type) != 0) {
        error_report("This board can only be used with CPU %s",
                mmc->default_cpu_type);
        exit(EXIT_FAILURE);
    }

    rt685_m33_mmap_init(machine);

    /* These clocks don't need migration because they are fixed-frequency */
    mms->sysclk = clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(mms->sysclk, SYSCLK_FRQ);
    mms->s32kclk = clock_new(OBJECT(machine), "S32KCLK");
    clock_set_hz(mms->s32kclk, S32KCLK_FRQ);

    //memory_region_init(system_memory, OBJECT(machine), "rt685 amba", UINT64_MAX);

    object_initialize_child(OBJECT(machine), TYPE_IOTKIT, &mms->iotkit, mc->armsse_type);
    iotkitdev = DEVICE(&mms->iotkit);
    object_property_set_link(OBJECT(&mms->iotkit), "memory", OBJECT(system_memory), &error_abort);
    //qdev_prop_set_uint32(iotkitdev, "EXP_NUMIRQ", RT685_M33_NUMIRQ);
    printf("init-svtor= 0x%x\n", mms->boot_base_addr);
    qdev_prop_set_uint32(iotkitdev, "init-svtor", mms->boot_base_addr);
    qdev_prop_set_uint32(iotkitdev, "SRAM_ADDR_WIDTH", 15);
    qdev_connect_clock_in(iotkitdev, "MAINCLK", mms->sysclk);
    qdev_connect_clock_in(iotkitdev, "S32KCLK", mms->s32kclk);
    sysbus_realize(SYS_BUS_DEVICE(&mms->iotkit), &error_fatal);

#ifdef CONFIG_POSIX
    Object *backend = NULL;
    backend = object_property_get_link(OBJECT(mms), "memory-backend", &error_fatal);
    if (backend != NULL) {
        printf("[ROM_EMU]: backend file init\n");
    }
#endif
    int i = 0;
    for (i=0; i < RT685_M33_SRAM_BANKS; i++) {
        if (_memmap.sram_part[i].size == 0) {
		continue;
	}
        char *sram_name = g_strdup_printf("sram%d", i);
        memory_region_init_ram(&mms->sram[i], NULL, sram_name, _memmap.sram_part[i].size, &error_fatal);
        memory_region_add_subregion(system_memory,  _memmap.sram_part[i].start, &mms->sram[i]);
        make_alias(&mms->sram_alias[i], system_memory, "sram alias", 0x10000000 + _memmap.sram_part[i].start,
                _memmap.sram_part[i].size, _memmap.sram_part[i].start);
        make_alias(&mms->sram_data_alias[i], system_memory, "sram data alias", 0x30000000 + _memmap.sram_part[i].start,
                _memmap.sram_part[i].size, _memmap.sram_part[i].start);
        make_alias(&mms->sram_data_alias_ns[i], system_memory, "sram alias", 0x20000000 + _memmap.sram_part[i].start,
                _memmap.sram_part[i].size, _memmap.sram_part[i].start);
        g_free(sram_name);
    }

#if 1 
    /* TODO need get from DTS */
    rt685_resolve_flexcomm_uart(mms, 0, 0); /* dbug uart */
    rt685_resolve_flexcomm_i2c(mms, 2);  /* arduio i2c meter and i2c accelerator meter*/
    rt685_resolve_flexcomm_uart(mms, 4, 1); /* arduio uart*/
    rt685_resolve_flexcomm_i2c(mms, 15); /* pmic i2c */
    rt685_resolve_lpadc(mms, 0);
    rt685_resolve_ostimer(mms, 0);
    rt685_evk_i2c_init(mms);
    rt685_resolve_pmc(mms, 0);
    rt685_resolve_machine_mu(mms);
    rt685_resolve_machine_pint(mms);
#endif

    rt685_resolve_machine_flexspi_nor(mms);    
    make_alias(&mms->flexspi_s, system_memory, "flexspi sec", 0x10000000 + _memmap.flexspi.start,
                _memmap.flexspi.size, _memmap.flexspi.start);

    sysbus_create_simple(TYPE_RT_SYSCTL0, (hwaddr)_memmap.SYSCTL0.start, NULL);
    make_alias(&mms->SYSCTL0_s, system_memory, "sysctrl0 sec", 0x10000000 + _memmap.SYSCTL0.start,
                _memmap.SYSCTL0.size, _memmap.SYSCTL0.start);

    sysbus_create_simple(TYPE_RT_SYSCTL1, (hwaddr)_memmap.SYSCTL1.start, NULL);
    make_alias(&mms->SYSCTL1_s, system_memory, "sysctrl1 sec", 0x10000000 + _memmap.SYSCTL1.start,
                _memmap.SYSCTL1.size, _memmap.SYSCTL1.start);

    sysbus_create_simple(TYPE_RT_RSTCTL0, (hwaddr)_memmap.RSTCTL0.start, NULL);
    make_alias(&mms->RSTCTL0_s, system_memory, "rstctrl0 sec", 0x10000000 + _memmap.RSTCTL0.start,
                _memmap.RSTCTL0.size, _memmap.RSTCTL0.start);

    sysbus_create_simple(TYPE_RT_RSTCTL1, (hwaddr)_memmap.RSTCTL1.start, NULL);
    make_alias(&mms->RSTCTL1_s, system_memory, "rstctrl1 sec", 0x10000000 + _memmap.RSTCTL1.start,
                _memmap.RSTCTL1.size, _memmap.RSTCTL1.start);

    sysbus_create_simple(TYPE_RT_CLKCTL0, (hwaddr)_memmap.CLKCTL0.start, NULL);
    make_alias(&mms->CLKCTL0_s, system_memory, "clkctrl0 sec", 0x10000000 + _memmap.CLKCTL0.start,
                _memmap.CLKCTL0.size, _memmap.CLKCTL0.start);

    sysbus_create_simple(TYPE_RT_CLKCTL1, (hwaddr)_memmap.CLKCTL1.start, NULL);
    make_alias(&mms->CLKCTL1_s, system_memory, "clkctrl1 sec", 0x10000000 + _memmap.CLKCTL1.start,
                _memmap.CLKCTL1.size, _memmap.CLKCTL1.start);

    //sysbus_create_simple(TYPE_RT_PMC, (hwaddr)_memmap.PMC.start, NULL);
    //make_alias(&mms->PMC_s, system_memory, "PMC sec", 0x10000000 + _memmap.PMC.start,
    //            _memmap.PMC.size, _memmap.PMC.start);

    sysbus_create_simple(TYPE_RT_FLEXSPI, (hwaddr)_memmap.flexspi_ctl.start, NULL);
    make_alias(&mms->flexspi_ctl_s, system_memory, "flexspi ctl sec", 0x10000000 + _memmap.flexspi_ctl.start,
                _memmap.flexspi_ctl.size, _memmap.flexspi_ctl.start);

    make_alias(&mms->PERIPHERAL_MUXES_s, system_memory, "IOMUX_s", 0x10000000 + _memmap.PERIPHERAL_MUXES.start,
                _memmap.PERIPHERAL_MUXES.size, _memmap.PERIPHERAL_MUXES.start);

    /* DMA hwtrigger */
    dma_trigger_init_notifier_list(&mms->dma_hwtrigger_notifiers);
    mms->hwtrigger_notifier.notify = rt685_dma_hwtrigger_req;
    dma_trigger_notifier_connect(&mms->hwtrigger_notifier);

    rt685_resolve_machine_sdio(mms, 0);
    rt685_resolve_machine_sdio(mms, 1);
    /* ram boot  */
    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0x80000000, 0x400000);
}

static void rt685_m33_idau_check(IDAUInterface *ii, uint32_t address,
        int *iregion, bool *exempt, bool *ns, bool *nsc)
{
    int region = extract32(address, 28, 4);

    *ns = !(region & 1);
    *nsc = false;
    /* 0xe0000000..0xe00fffff and 0xf0000000..0xf00fffff are exempt */
    *exempt = (address & 0xeff00000) == 0xe0000000;
    *iregion = region;
}

static Property rt685_m33_soc_properties[] = {
    DEFINE_PROP_UINT32("boot-base-addr", RT685_M33_MachineState, boot_base_addr,
                        0x18001000),
    DEFINE_PROP_END_OF_LIST(),
};

static void rt685_m33_class_init(ObjectClass *oc, void *data)
{
    IDAUInterfaceClass *iic = IDAU_INTERFACE_CLASS(oc);
    MachineClass *mc = MACHINE_CLASS(oc);
    RT685_M33_MachineClass *mmc = RT685_MACHINE_CLASS(oc);

    iic->check = rt685_m33_idau_check;

    mc->default_cpus = 1; 
    mc->max_cpus = 1;
    mc->min_cpus = mc->default_cpus;
    mc->minimum_page_bits = 10;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m33");
    mc->desc = "RT685 for Cortex-M33";
    mc->default_ram_size = 128 * MiB;
    mc->init = rt685_m33_common_init;
    mmc->armsse_type = TYPE_RT500_M33;
    device_class_set_props(DEVICE_CLASS(oc), rt685_m33_soc_properties);
}

static const TypeInfo rt685_m33_info = {
    .name = TYPE_RT685_M33_MACHINE,
    .parent = TYPE_MACHINE,
    .instance_size = sizeof(RT685_M33_MachineState),
    .class_size = sizeof(RT685_M33_MachineClass),
    .class_init = rt685_m33_class_init,
};

static void rt685_m33_machine_init(void)
{
    type_register_static(&rt685_m33_info);
}

type_init(rt685_m33_machine_init);
