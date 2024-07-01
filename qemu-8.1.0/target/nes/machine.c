#include "qemu/osdep.h"
#include "cpu.h"
#include "migration/cpu.h"


const VMStateDescription vms_nes_cpu = {
    .name = "cpu",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(env.PC, NESCPU),
        VMSTATE_UINT32(env.SP, NESCPU),
        VMSTATE_UINT32(env.A, NESCPU),
        VMSTATE_UINT32(env.X, NESCPU),
        VMSTATE_UINT32(env.Y, NESCPU),
        VMSTATE_UINT32(env.f_C, NESCPU),
        VMSTATE_UINT32(env.f_Z, NESCPU),
        VMSTATE_UINT32(env.f_D, NESCPU),
        VMSTATE_UINT32(env.f_B, NESCPU),
        VMSTATE_UINT32(env.f_I, NESCPU),
        VMSTATE_UINT32(env.f_U, NESCPU),
        VMSTATE_UINT32(env.f_N, NESCPU),
        VMSTATE_UINT32(env.f_V, NESCPU),
        VMSTATE_END_OF_LIST()
    }
};
