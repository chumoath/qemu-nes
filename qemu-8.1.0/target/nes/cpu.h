#ifndef QEMU_NES_CPU_H
#define QEMU_NES_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"

#ifdef CONFIG_USER_ONLY
#error "NES 16-bit does not support user mode"
#endif

#define CPU_RESOLVING_TYPE TYPE_NES_CPU

#define TCG_GUEST_DEFAULT_MO 0

/*
 * NES has two memory spaces, data & code.
 * e.g. both have 0 address
 * ST/LD instructions access data space
 * LPM/SPM and instruction fetching access code memory space
 */
#define MMU_CODE_IDX 0
#define MMU_DATA_IDX 1

#define EXCP_RESET 1
#define EXCP_INT(n) (EXCP_RESET + (n) + 1)

/* Number of CPU registers */
#define NUMBER_OF_CPU_REGISTERS 8

typedef struct CPUArchState {
    uint32_t R_PC;

    uint32_t R_P;
    uint32_t R_Z;
    uint32_t R_N;
    uint32_t r[NUMBER_OF_CPU_REGISTERS];
} CPUNESState;

struct ArchCPU {
    /*< private >*/
    CPUState parent_obj;
    /*< public >*/

    CPUNegativeOffsetState neg;
    CPUNESState env;
};

enum
{
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

enum
{
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};

extern const struct VMStateDescription vms_nes_cpu;

void nes_cpu_do_interrupt(CPUState *cpu);
bool nes_cpu_exec_interrupt(CPUState *cpu, int int_req);
hwaddr nes_cpu_get_phys_page_debug(CPUState *cpu, vaddr addr);
int nes_cpu_gdb_read_register(CPUState *cpu, GByteArray *buf, int reg);
int nes_cpu_gdb_write_register(CPUState *cpu, uint8_t *buf, int reg);
int nes_print_insn(bfd_vma addr, disassemble_info *info);
vaddr nes_cpu_gdb_adjust_breakpoint(CPUState *cpu, vaddr addr);

#define cpu_list nes_cpu_list
#define cpu_mmu_index nes_cpu_mmu_index

static inline int nes_cpu_mmu_index(CPUNESState *env, bool ifetch)
{
    return ifetch ? MMU_CODE_IDX : MMU_DATA_IDX;
}

void nes_cpu_tcg_init(void);

void nes_cpu_list(void);
int cpu_nes_exec(CPUState *cpu);


static inline void cpu_get_tb_cpu_state(CPUNESState *env, vaddr *pc,
                                        uint64_t *cs_base, uint32_t *pflags)
{
    uint32_t flags = 0;

    *pc = env->R_PC * 2;
    *cs_base = 0;

    *pflags = flags;
}

static inline int cpu_interrupts_enabled(CPUNESState *env)
{
    return 0;
}

bool nes_cpu_tlb_fill(CPUState *cs, vaddr address, int size,
                      MMUAccessType access_type, int mmu_idx,
                      bool probe, uintptr_t retaddr);

#include "exec/cpu-all.h"

#endif /* QEMU_NES_CPU_H */
