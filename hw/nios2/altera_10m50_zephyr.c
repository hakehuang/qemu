/*
 * Copyright (c) 2018 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "cpu.h"
#include "hw/sysbus.h"
#include "hw/hw.h"
#include "hw/char/serial.h"
#include "sysemu/sysemu.h"
#include "sysemu/reset.h"
#include "hw/qdev-properties.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "qemu/config-file.h"
#include "qemu/option.h"
#include "boot.h"

#define DEBUG

#ifdef DEBUG
# define DPRINTF(format, ...)     printf(format, ## __VA_ARGS__)
#else
# define DPRINTF(format, ...)     do { } while (0)
#endif

#define ROM_BASE		0x0
#define ROM_SIZE		32

#define RAM_BASE		0x400000
#define RAM_SIZE		262144

#define U16550_0_BASE		0x440000
#define U16550_0_IRQ_IDX	1

#define TIMER_0_BASE		0x440200
#define TIMER_0_FREQ		50000000
#define TIMER_0_IRQ_IDX		2

#define ALT_CPU_EXCEPTION_ADDR  0x00400020
#define ALT_CPU_RESET_ADDR      0x00000000

#define EM_ALTERA_NIOS2		113

static struct {
    uint32_t bootstrap_pc;
} boot_info_zephyr;

static void main_cpu_reset(void *opaque)
{
    Nios2CPU *cpu = opaque;
    CPUNios2State *env = &cpu->env;

    cpu_reset(CPU(cpu));
    env->regs[R_PC] = boot_info_zephyr.bootstrap_pc;
}

static void altera_10m50_zephyr_init(MachineState *machine)
{
    const char *kernel_filename;
    MemoryRegion *sysmem = get_system_memory();
    Nios2CPU *cpu;
    DeviceState *dev;
    qemu_irq irq[32];
    int kernel_size;
    int i;

    MemoryRegion *rom = g_new(MemoryRegion, 1);
    MemoryRegion *ram = g_new(MemoryRegion, 1);

    cpu = NIOS2_CPU(object_new(TYPE_NIOS2_CPU));
    object_property_set_bool(OBJECT(cpu), "mmu_present", false,
                             &error_abort);
    object_property_set_bool(OBJECT(cpu), "realized", true, &error_abort);

    kernel_filename = current_machine->kernel_filename;

    memory_region_init_ram(rom, NULL, "nios2.rom", ROM_SIZE, &error_fatal);
    memory_region_set_readonly(rom, true);
    memory_region_add_subregion(sysmem, ROM_BASE, rom);

    memory_region_init_ram(ram, NULL, "nios2.ram", RAM_SIZE, &error_fatal);
    memory_region_add_subregion(sysmem, RAM_BASE, ram);

    for (i = 0; i < 32; i++) {
        irq[i] = qdev_get_gpio_in_named(DEVICE(cpu), "IRQ", i);
    }

    /* Register: Altera 16550 UART */
    serial_mm_init(sysmem, U16550_0_BASE, 2, irq[U16550_0_IRQ_IDX],
		    115200, serial_hd(0), DEVICE_NATIVE_ENDIAN);

    /* Register: Timer sys_clk_timer  */
    dev = qdev_new("ALTR.timer");
    qdev_prop_set_uint32(dev, "clock-frequency", TIMER_0_FREQ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_0_BASE);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_0_IRQ_IDX]);

    cpu->reset_addr = ALT_CPU_RESET_ADDR;
    cpu->exception_addr = ALT_CPU_EXCEPTION_ADDR;
    cpu->fast_tlb_miss_addr = ALT_CPU_RESET_ADDR;
    DPRINTF("\tcpu->env.reset_addr: \t\t%0x\n", cpu->reset_addr);
    DPRINTF("\tcpu->env.exception_addr: \t%0x\n", cpu->exception_addr);

    if (kernel_filename) {
        uint64_t entry;

        /* Boots a kernel elf binary.  */
        kernel_size = load_elf(kernel_filename, NULL, NULL, NULL,
                               &entry, NULL, NULL, NULL,
                               0, EM_ALTERA_NIOS2, 0, 0);

        boot_info_zephyr.bootstrap_pc = entry;

        /* Not an ELF image, try a RAW image.  */
        if (kernel_size < 0) {
            hwaddr uentry, loadaddr;

            kernel_size = load_uimage(kernel_filename, &uentry,
                    &loadaddr, 0, NULL, NULL);
            boot_info_zephyr.bootstrap_pc = uentry;
        }

        if (kernel_size < 0) {
            fprintf(stderr, "qemu: could not load kernel '%s'\n",
                    kernel_filename);
            exit(1);
        }
    }
    qemu_register_reset(main_cpu_reset, cpu);

}

static void altera_10m50_zephyr_machine_init(MachineClass *mc)
{
    mc->desc = "Altera 10m50 for Zephyr.";
    mc->init = altera_10m50_zephyr_init;
    mc->is_default = 0;
}

DEFINE_MACHINE("altera_10m50_zephyr", altera_10m50_zephyr_machine_init)
