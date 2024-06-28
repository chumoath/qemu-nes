#ifndef HW_NES_BOOT_H
#define HW_NES_BOOT_H

#include "hw/boards.h"
#include "cpu.h"

bool nes_load_firmware(NESCPU *cpu, MachineState *ms, MemoryRegion *mr, const char *firmware);

#endif
