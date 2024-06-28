/*
 * QEMU NES CPU
 *
 * Copyright (c) 2019-2020 Michael Rolnik
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/lgpl-2.1.html>
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/qemu-print.h"
#include "exec/exec-all.h"
#include "cpu.h"
#include "disas/dis-asm.h"
#include "tcg/debug-assert.h"

static void nes_cpu_set_pc(CPUState *cs, vaddr value)
{
    NESCPU *cpu = NES_CPU(cs);

    cpu->env.R_PC = value / 2; /* internally PC points to words */
}

static vaddr nes_cpu_get_pc(CPUState *cs)
{
    NESCPU *cpu = NES_CPU(cs);

    return cpu->env.R_PC * 2;
}

static bool nes_cpu_has_work(CPUState *cs)
{
    return true;
}

static void nes_cpu_synchronize_from_tb(CPUState *cs,
                                        const TranslationBlock *tb)
{
    NESCPU *cpu = NES_CPU(cs);
    CPUNESState *env = &cpu->env;

    tcg_debug_assert(!(cs->tcg_cflags & CF_PCREL));
    env->R_PC = tb->pc / 2;
}

static void nes_restore_state_to_opc(CPUState *cs,
                                     const TranslationBlock *tb,
                                     const uint64_t *data)
{
    NESCPU *cpu = NES_CPU(cs);
    CPUNESState *env = &cpu->env;

    env->R_PC = data[0];
}

static void nes_cpu_reset_hold(Object *obj)
{
    CPUState *cs = CPU(obj);
    NESCPU *cpu = NES_CPU(cs);
    NESCPUClass *mcc = NES_CPU_GET_CLASS(cpu);
    CPUNESState *env = &cpu->env;

    if (mcc->parent_phases.hold) {
        mcc->parent_phases.hold(obj);
    }

    env->R_PC = 0x3000;
    env->R_Z = 1;
    env->R_P = 0;
    env->R_N = 0;
    memset(env->r, 0, sizeof(env->r));
}

static void nes_cpu_disas_set_info(CPUState *cpu, disassemble_info *info)
{
    info->mach = bfd_arch_nes;
    info->print_insn = nes_print_insn;
}

static void nes_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState *cs = CPU(dev);
    NESCPUClass *mcc = NES_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }
    qemu_init_vcpu(cs);
    cpu_reset(cs);

    mcc->parent_realize(dev, errp);
}

static void nes_cpu_set_int(void *opaque, int irq, int level)
{

}

static void nes_cpu_initfn(Object *obj)
{
    NESCPU *cpu = NES_CPU(obj);

    cpu_set_cpustate_pointers(cpu);
}

static ObjectClass *nes_cpu_class_by_name(const char *cpu_model)
{
    ObjectClass *oc;

    oc = object_class_by_name(cpu_model);
    if (object_class_dynamic_cast(oc, TYPE_NES_CPU) == NULL ||
        object_class_is_abstract(oc)) {
        oc = NULL;
    }
    return oc;
}

static void nes_cpu_dump_state(CPUState *cs, FILE *f, int flags)
{
    NESCPU *cpu = NES_CPU(cs);
    CPUNESState *env = &cpu->env;
    int i;

    qemu_fprintf(f, "\n");
    qemu_fprintf(f, "R_PC:    %04x\n", env->R_PC);
    qemu_fprintf(f, "R_P:      %04x\n", env->R_P);
    qemu_fprintf(f, "R_Z:      %04x\n", env->R_Z);
    qemu_fprintf(f, "R_N:      %04x\n", env->R_N);
    qemu_fprintf(f, "\n");
    for (i = 0; i < ARRAY_SIZE(env->r); i++) {
        qemu_fprintf(f, "R[%02d]:  %04x   ", i, env->r[i]);
        qemu_fprintf(f, "\n");
    }
}

#include "hw/core/sysemu-cpu-ops.h"

static const struct SysemuCPUOps nes_sysemu_ops = {
    .get_phys_page_debug = nes_cpu_get_phys_page_debug,
};

#include "hw/core/tcg-cpu-ops.h"

static const struct TCGCPUOps nes_tcg_ops = {
    .initialize = nes_cpu_tcg_init,
    .synchronize_from_tb = nes_cpu_synchronize_from_tb,
    .restore_state_to_opc = nes_restore_state_to_opc,
    .cpu_exec_interrupt = nes_cpu_exec_interrupt,
    .tlb_fill = nes_cpu_tlb_fill,
    .do_interrupt = nes_cpu_do_interrupt,
};

static void nes_cpu_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    CPUClass *cc = CPU_CLASS(oc);
    NESCPUClass *mcc = NES_CPU_CLASS(oc);
    ResettableClass *rc = RESETTABLE_CLASS(oc);

    device_class_set_parent_realize(dc, nes_cpu_realizefn, &mcc->parent_realize);

    resettable_class_set_parent_phases(rc, NULL, nes_cpu_reset_hold, NULL,
                                       &mcc->parent_phases);

    cc->class_by_name = nes_cpu_class_by_name;

    cc->has_work = nes_cpu_has_work;
    cc->dump_state = nes_cpu_dump_state;
    cc->set_pc = nes_cpu_set_pc;
    cc->get_pc = nes_cpu_get_pc;
    dc->vmsd = &vms_nes_cpu;
    cc->sysemu_ops = &nes_sysemu_ops;
    cc->disas_set_info = nes_cpu_disas_set_info;
    cc->gdb_read_register = nes_cpu_gdb_read_register;
    cc->gdb_write_register = nes_cpu_gdb_write_register;
    cc->gdb_adjust_breakpoint = nes_cpu_gdb_adjust_breakpoint;
    cc->gdb_num_core_regs = 35;
    cc->gdb_core_xml_file = "nes-cpu.xml";
    cc->tcg_ops = &nes_tcg_ops;
}

static void nes_cpu_list_entry(gpointer data, gpointer user_data)
{
    const char *typename = object_class_get_name(OBJECT_CLASS(data));

    qemu_printf("%s\n", typename);
}

void nes_cpu_list(void)
{
    GSList *list;
    list = object_class_get_list_sorted(TYPE_NES_CPU, false);
    g_slist_foreach(list, nes_cpu_list_entry, NULL);
    g_slist_free(list);
}

static const TypeInfo nes_cpu_type_info[] = {
    {
        .name = TYPE_NES_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(NESCPU),
        .instance_init = nes_cpu_initfn,
        .class_size = sizeof(NESCPUClass),
        .class_init = nes_cpu_class_init,
    }
};

DEFINE_TYPES(nes_cpu_type_info)
