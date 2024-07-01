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