#include "qemu/osdep.h"
#include "cpu.h"
#include "migration/cpu.h"


const VMStateDescription vms_nes_cpu = {
    .name = "cpu",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(env.R_PC, NESCPU),
        VMSTATE_UINT32(env.R_P, NESCPU),
        VMSTATE_UINT32(env.R_N, NESCPU),
        VMSTATE_UINT32(env.R_Z, NESCPU),
        VMSTATE_UINT32_ARRAY(env.r, NESCPU, NUMBER_OF_CPU_REGISTERS),
        VMSTATE_END_OF_LIST()
    }
};
