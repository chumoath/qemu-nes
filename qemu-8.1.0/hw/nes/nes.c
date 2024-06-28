#include "qemu/osdep.h"
#include "qapi/error.h"
#include "nes_mcu.h"
#include "boot.h"
#include "qom/object.h"

struct NESMachineState {
    /*< private >*/
    MachineState parent_obj;
    /*< public >*/
    NESMcuState mcu;
};

typedef struct NESMachineState NESMachineState;

struct NESMachineClass {
    /*< private >*/
    MachineClass parent_class;
    /*< public >*/
    const char *mcu_type;
};

typedef struct NESMachineClass NESMachineClass;

#define TYPE_NES_MACHINE MACHINE_TYPE_NAME("nes-machine")

DECLARE_OBJ_CHECKERS(NESMachineState, NESMachineClass, NES_MACHINE, TYPE_NES_MACHINE)

static void nes_machine_init(MachineState *machine)
{
    NESMachineClass *amc = NES_MACHINE_GET_CLASS(machine);
    NESMachineState *ams = NES_MACHINE(machine);

    object_initialize_child(OBJECT(machine), "mcu", &ams->mcu, amc->mcu_type);
    sysbus_realize(SYS_BUS_DEVICE(&ams->mcu), &error_abort);

    if (machine->firmware) {
        if (!nes_load_firmware(&ams->mcu.cpu, machine, &ams->mcu.sram, machine->firmware)) {
            exit(1);
        }
    }
}

static void nes_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    NESMachineClass *amc = NES_MACHINE_CLASS(oc);

    mc->init = nes_machine_init;
    mc->desc        = "NES Machine";
    mc->alias       = "NES Board";
    amc->mcu_type   = TYPE_NES_MCU;
};

static const TypeInfo nes_machine_types[] = {
    {
        .name           = TYPE_NES_MACHINE,
        .parent         = TYPE_MACHINE,
        .instance_size  = sizeof(NESMachineState),
        .class_size     = sizeof(NESMachineClass),
        .class_init     = nes_machine_class_init,
    }
};

DEFINE_TYPES(nes_machine_types)
