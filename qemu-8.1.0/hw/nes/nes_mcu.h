#ifndef HW_NES_MCU_H
#define HW_NES_MCU_H

#include "target/nes/cpu.h"
#include "qom/object.h"
#include "hw/sysbus.h"

#define TYPE_NES_MCU     "nes-mcu"

typedef struct NESMcuState NESMcuState;
DECLARE_INSTANCE_CHECKER(NESMcuState, NES_MCU, TYPE_NES_MCU)

struct NESMcuState {
    /*< private >*/
    SysBusDevice parent_obj;
    /*< public >*/

    NESCPU cpu;
    MemoryRegion sram;
};

#endif /* HW_NES_MCU_H */
