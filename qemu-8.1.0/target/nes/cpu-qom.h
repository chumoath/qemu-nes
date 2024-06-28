#ifndef TARGET_NES_CPU_QOM_H
#define TARGET_NES_CPU_QOM_H

#include "hw/core/cpu.h"
#include "qom/object.h"

#define TYPE_NES_CPU "nes-cpu"

OBJECT_DECLARE_CPU_TYPE(NESCPU, NESCPUClass, NES_CPU)

struct NESCPUClass {
    /*< private >*/
    CPUClass parent_class;
    /*< public >*/
    DeviceRealize parent_realize;
    ResettablePhases parent_phases;
};


#endif /* TARGET_NES_CPU_QOM_H */
