/*
 * QEMU ATmega MCU
 *
 * Copyright (c) 2019-2020 Philippe Mathieu-Daudé
 *
 * This work is licensed under the terms of the GNU GPLv2 or later.
 * See the COPYING file in the top-level directory.
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "qemu/osdep.h"
#include "qemu/module.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "sysemu/sysemu.h"
#include "hw/qdev-properties.h"
#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/misc/unimp.h"
#include "nes_mcu.h"

struct NESMcuClass {
    /*< private >*/
    SysBusDeviceClass parent_class;
    /*< public >*/
    const char *cpu_type;
    size_t sram_size;
};

typedef struct NESMcuClass NESMcuClass;

DECLARE_CLASS_CHECKERS(NESMcuClass, NES_MCU, TYPE_NES_MCU)

static void nes_realize(DeviceState *dev, Error **errp)
{
    NESMcuState *s = NES_MCU(dev);
    const NESMcuClass *mc = NES_MCU_GET_CLASS(dev);

    /* CPU */
    object_initialize_child(OBJECT(dev), "cpu", &s->cpu, mc->cpu_type);
    qdev_realize(DEVICE(&s->cpu), NULL, &error_abort);

    /* SRAM */
    memory_region_init_ram(&s->sram, OBJECT(dev), "sram", mc->sram_size, &error_abort);
    memory_region_add_subregion(get_system_memory(), 0, &s->sram);
}

static void nes_class_init(ObjectClass *oc, void *data)
{
    NESMcuClass *amc = NES_MCU_CLASS(oc);
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = nes_realize;
    amc->cpu_type = TYPE_NES_CPU;
    #define MEMORY_MAX (1 << 17)
    amc->sram_size = MEMORY_MAX;
    #undef MEMORY_MAX
};

static const TypeInfo nes_mcu_types[] = {
    {
        .name           = TYPE_NES_MCU,
        .parent         = TYPE_SYS_BUS_DEVICE,
        .instance_size  = sizeof(NESMcuState),
        .class_size     = sizeof(NESMcuClass),
        .class_init     = nes_class_init,
    }
};

DEFINE_TYPES(nes_mcu_types)
