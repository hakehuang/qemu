/*
 * Copyright (c) 2023, NXP Semiconductor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Open Source and Linux Lab nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "cpu.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/qdev-properties.h"
#include "elf.h"
#include "exec/memory.h"
#include "hw/sysbus.h"
#include "sysemu/reset.h"
#include "sysemu/runstate.h"
#include "qemu/error-report.h"
#include "qemu/option.h"
#include "xtensa_memory.h"
#include "migration/vmstate.h"
#include "hw/misc/dbus_client.h"

#define RT595_DSP_SRAM_BANKS  32

typedef struct mem_region_t {
    uint32_t start;
    uint32_t size;
    uint32_t irq_num;
    uint32_t dma_hwtrg_nr;
} mem_region;

typedef struct rt595_memmap_tag {
    mem_region pflash;
    mem_region sram_part[RT595_DSP_SRAM_BANKS];
    mem_region mub;

} rt595_dsp_memmap_t;

static rt595_dsp_memmap_t _memmap;

static void rt595_dsp_mmap_init(void)
{
    // Initialize variables
    uint32_t base = 0x00800000;
    _memmap.pflash.start = 0;
        
    // Initialize SRAM partitions
    for(int i = 0; i < 8; i++)
    {
        _memmap.sram_part[i].start = base + (0x8000 * i);
        _memmap.sram_part[i].size = 0x8000; // 32K
    }

    // Add larger SRAM partitions
    base = 0x840000;
    for(int i = 8; i < 12; i++)
    {
        _memmap.sram_part[i].start = base + (0x10000 * (i - 8));
        _memmap.sram_part[i].size = 0x10000; // 64K
    }

    base = 0x880000;
    for(int i = 12; i < 16; i++)
    {
        _memmap.sram_part[i].start = base + (0x20000 * (i - 12));
        _memmap.sram_part[i].size = 0x20000; // 128K
    }

    base = 0x900000;
    for(int i = 16; i < RT595_DSP_SRAM_BANKS; i++)
    {
        _memmap.sram_part[i].start = base + (0x40000 * (i - 16));
        _memmap.sram_part[i].size = 0x40000; // 256K
    }

    _memmap.mub.start         = 0x40111000;
    _memmap.mub.size          = 0x1000;
    _memmap.mub.irq_num       = 23;

}

typedef struct XtRT595BoardDesc {
    size_t sram_size;
    const hwaddr *io;
} XtRT595BoardDesc;

typedef struct XtRT595State {
    MemoryRegion mub;
    uint32_t freq;
    MemoryRegion pflash;
    MemoryRegion rom;
    MemoryRegion sram[RT595_DSP_SRAM_BANKS];
    MemoryRegion sram_data_alias_ns[RT595_DSP_SRAM_BANKS];
} XtRT595State;

static void xt_rt595_reset(void *opaque)
{
    return;
}

static  qemu_irq rt595_dsp_irq_init(CPUXtensaState *env, int n)
{
    qemu_irq * ext_irqs = xtensa_get_extints(env);

    return ext_irqs[n];
}

static void rt595_dsp_resolve_machine_mub(CPUXtensaState *env)
{
    qemu_irq irq = rt595_dsp_irq_init(env, _memmap.mub.irq_num);

    sysbus_create_simple(TYPE_DBUS_CLIENT_MUB, (hwaddr)_memmap.mub.start, irq);
}

static XtRT595State *xt_rt595_init(CPUXtensaState *env, uint32_t freq)
{
    XtRT595State *s = g_new(XtRT595State, 1);
    MemoryRegion *system_memory = get_system_memory();

    s->freq = freq;
    rt595_dsp_mmap_init();
    for (int i=0; i < RT595_DSP_SRAM_BANKS; i++) {
        char *sram_name = g_strdup_printf("sram%d", i);
        memory_region_init_ram(&s->sram[i], NULL, sram_name, _memmap.sram_part[i].size, &error_fatal);
        memory_region_add_subregion(system_memory,  _memmap.sram_part[i].start, &s->sram[i]);
        g_free(sram_name);
    }

    rt595_dsp_resolve_machine_mub(env);

    return s;
}

static uint64_t translate_phys_addr(void *opaque, uint64_t addr)
{
    XtensaCPU *cpu = opaque;

    return cpu_get_phys_page_debug(CPU(cpu), addr);
}

static void xt_init(const XtRT595BoardDesc *board, MachineState *machine)
{
    XtensaCPU *cpu = NULL;
    CPUXtensaState *env = NULL;
    uint32_t freq = 10000000;
    const char *kernel_filename = machine->kernel_filename;

    cpu = XTENSA_CPU(cpu_create(machine->cpu_type));
    env = &cpu->env;
    env->sregs[PRID] = 0;
    xtensa_select_static_vectors(env, 0);
    qemu_register_reset(xt_rt595_reset, cpu);
    /* Need MMU initialized prior to ELF loading,
     * so that ELF gets loaded into virtual addresses
     */
    cpu_reset(CPU(cpu));

    xt_rt595_init(env, freq);

    /* Use presence of kernel file name as 'boot from SRAM' switch. */
    if (kernel_filename) {
        uint64_t elf_entry;

        int success = load_elf(kernel_filename, NULL, translate_phys_addr, cpu,
                &elf_entry, NULL, NULL, NULL, false, EM_XTENSA, 0, 0);
        if (success > 0) {
            cpu->env.pc = elf_entry;
        }
    }
}

static void xtensa_rt595_nommu_init(MachineState *machine)
{
    static const XtRT595BoardDesc rt595_board = {
        .sram_size = 0x2000000,
    };
    xt_init(&rt595_board, machine);
}

static void xt_rt595_nommu_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "NXP RT595 EVB (" XTENSA_DEFAULT_CPU_NOMMU_MODEL ")";
    mc->init = xtensa_rt595_nommu_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = XTENSA_DEFAULT_CPU_NOMMU_TYPE;
    mc->default_ram_size = 256 * MiB;
}

static const TypeInfo xt_rt595_type = {
    .name = MACHINE_TYPE_NAME("xt-rt595-nommu"),
    .parent = TYPE_MACHINE,
    .class_init = xt_rt595_nommu_class_init,
};

static void xtensa_rt595_machine_init(void)
{
    type_register_static(&xt_rt595_type);
}

type_init(xtensa_rt595_machine_init)
