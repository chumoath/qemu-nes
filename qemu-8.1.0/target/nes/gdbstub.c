#include "qemu/osdep.h"
#include "gdbstub/helpers.h"

int nes_cpu_gdb_read_register(CPUState *cs, GByteArray *mem_buf, int n)
{
    NESCPU *cpu = NES_CPU(cs);
    CPUNESState *env = &cpu->env;

    switch (n) {
    case 0:
        // PC
        return gdb_get_reg8(mem_buf, env->PC);
    case 1:
        return gdb_get_reg8(mem_buf, env->SP);
    case 2:
        return gdb_get_reg8(mem_buf, env->A);
    case 3:
        return gdb_get_reg8(mem_buf, env->X);
    case 4:
        return gdb_get_reg8(mem_buf, env->Y);
    case 5:
        return gdb_get_reg8(mem_buf, env->f_Z);
    default:
        ;
    }

    return 0;
}

int nes_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    NESCPU *cpu = NES_CPU(cs);
    CPUNESState *env = &cpu->env;

    switch (n) {
    case 0:
        env->PC = lduw_p(mem_buf);
        break;
    case 1:
        env->SP = ldub_p(mem_buf);
        break;
    case 2:
        env->A = ldub_p(mem_buf);
        break;
    case 3:
        env->X = ldub_p(mem_buf);
        break;
    case 4:
        env->Y = ldub_p(mem_buf);
        break;
    case 5:
        env->f_Z = ldub_p(mem_buf);
        break;
    default:
        ;
    }
    return 0;
}

vaddr nes_cpu_gdb_adjust_breakpoint(CPUState *cpu, vaddr addr)
{
    return addr;
}
