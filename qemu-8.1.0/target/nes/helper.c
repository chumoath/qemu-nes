#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/error-report.h"
#include "cpu.h"
#include "hw/core/tcg-cpu-ops.h"
#include "exec/exec-all.h"
#include "exec/address-spaces.h"
#include "exec/helper-proto.h"

bool nes_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    return true;
}

void nes_cpu_do_interrupt(CPUState *cs)
{

}

hwaddr nes_cpu_get_phys_page_debug(CPUState *cs, vaddr addr)
{
    return addr;
}

bool nes_cpu_tlb_fill(CPUState *cs, vaddr address, int size,
                      MMUAccessType access_type, int mmu_idx,
                      bool probe, uintptr_t retaddr)
{
    int prot, page_size = TARGET_PAGE_SIZE;
    uint32_t paddr;

    address &= TARGET_PAGE_MASK;

    paddr = address;
    prot = PAGE_READ | PAGE_EXEC | PAGE_WRITE;
    tlb_set_page(cs, address, paddr, prot, mmu_idx, page_size);
    return true;
}

void helper_unsupported(CPUNESState *env)
{
    CPUState *cs = env_cpu(env);

    /*
     *  I count not find what happens on the real platform, so
     *  it's EXCP_DEBUG for meanwhile
     */
    cs->exception_index = EXCP_DEBUG;
    if (qemu_loglevel_mask(LOG_UNIMP)) {
        qemu_log("UNSUPPORTED\n");
        cpu_dump_state(cs, stderr, 0);
    }
    cpu_loop_exit(cs);
}

static uint16_t check_key(void)
{
    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

target_ulong helper_fullrd(CPUNESState *env, uint32_t addr)
{
    uint16_t data, dr, sr;
    if (addr == MR_KBSR)
    {
        if (check_key())
        {
            sr = (1 << 15);
            dr = getchar();
        }
        else
        {
            sr = 0;
        }

        address_space_stw(&address_space_memory, MR_KBSR * 2, sr & 0xffff, MEMTXATTRS_UNSPECIFIED, NULL);
        address_space_stw(&address_space_memory, MR_KBDR * 2, dr & 0xffff, MEMTXATTRS_UNSPECIFIED, NULL);
    }

    data = address_space_lduw(&address_space_memory, addr * 2, MEMTXATTRS_UNSPECIFIED, NULL);
    
    return data;
}

void helper_fullwr(CPUNESState *env, uint32_t data, uint32_t addr)
{
    address_space_stw(&address_space_memory, addr * 2, data & 0xffff, MEMTXATTRS_UNSPECIFIED, NULL);
}

static void update_flags(CPUNESState *env, uint16_t r)
{
    env->R_Z = 0;
    env->R_P = 0;
    env->R_N = 0;

    if (env->r[r] == 0)
    {
        env->R_Z = 1;
    }
    else if (env->r[r] >> 15)
    {
        env->R_N = 1;
    }
    else
    {
        env->R_P = 1;
    }
}

void helper_trap(CPUNESState *env, uint32_t trapvect8)
{
    switch (trapvect8) {
        case TRAP_GETC: 
        {
            env->r[0] = (uint16_t)getchar();
            update_flags(env, 0);
            break;
        }
        case TRAP_OUT:
        {
            putc((char)env->r[0], stdout);
            fflush(stdout);
            break;
        }
        case TRAP_PUTS:
        {
            uint16_t c;
            uint16_t idx;
            idx = env->r[0];

            while (1)
            {
                c = address_space_lduw(&address_space_memory, idx++ * 2, MEMTXATTRS_UNSPECIFIED, NULL);
                if (!c) break;
                putc((char)c, stdout);
            }
            fflush(stdout);
            break;
        }
        case TRAP_IN:
        {
            printf("Enter a character: ");
            char c = getchar();
            putc(c, stdout);
            fflush(stdout);
            env->r[0] = (uint16_t)c;
            update_flags(env, 0);
            break;
        }
        case TRAP_PUTSP:
        {
            uint16_t c;
            uint16_t idx;
            idx = env->r[0];
            
            while (1)
            {
                char char1, char2;
                c = address_space_lduw(&address_space_memory, idx++ * 2, MEMTXATTRS_UNSPECIFIED, NULL);
                if (!c) break;
                char1 = (c) & 0xFF;
                putc(char1, stdout);
                
                char2 = (c) >> 8;
                if (char2) putc(char2, stdout);

            }
            fflush(stdout);
            break;
        }
        case TRAP_HALT:
        {
            puts("HALT");
            fflush(stdout);
            break;
        }
    }
}

#ifdef PRINT_REGS
static void nes_log(const char *log_file_name, const char *fmt, ...)
{
    char log_file_path[64], mode[2] = "a";
    sprintf (log_file_path, "%s", log_file_name);

    #if 0
    struct stat st = {};
    stat (log_file_path, &st);
    if (st.st_size > 200 * 1024) {
        mode[0] = 'w';
    }
    #endif

    FILE *fp = NULL;
    fp = fopen (log_file_path, mode);
    
    va_list args;
    va_start (args, fmt);
    vfprintf (fp, fmt, args);
    fflush(fp);
    va_end (args);
    fclose (fp);
}

void helper_print_regs(CPUNESState *env, const void *fmt, uint32_t npc)
{
    nes_log("debug.txt", fmt);
    nes_log("debug.txt", "    npc: 0x%x\n", npc);
    nes_log("debug.txt", "    R_P: 0x%x R_Z: 0x%x R_N: 0x%x\n", env->R_P, env->R_Z, env->R_N);
    nes_log("debug.txt", "    ");
    for (int i = 0; i < NUMBER_OF_CPU_REGISTERS; i++) {
        nes_log("debug.txt", "R%d: 0x%x ", i, env->r[i]);
    }
    nes_log("debug.txt", "\n\n");
}

void helper_print_val(CPUNESState *env, uint32_t num)
{
    nes_log("debug.txt", "\n\n");
    nes_log("debug.txt", "num = 0x%x\n", num);
    nes_log("debug.txt", "\n\n");
}
#else
void helper_print_regs(CPUNESState *env, const void *fmt, uint32_t npc)
{
}

void helper_print_val(CPUNESState *env, uint32_t num)
{
}
#endif

