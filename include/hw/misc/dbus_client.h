/*
 * Copyright (c) 2022, NXP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef DBUS_CLIENT_MU_H
#define DBUS_CLIENT_MU_H

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "qemu/dbus.h"
#include "qemu/thread.h"
#include "qemu/error-report.h"
#include "exec/address-spaces.h"
#include "sysemu/sysemu.h"
#include "hw/arm/boot.h"
#include "hw/or-irq.h"
#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_DBUS_CLIENT_MUA "dbus_client_mua"
OBJECT_DECLARE_SIMPLE_TYPE(DBUSCLIENTMUAState, DBUS_CLIENT_MUA)

struct DBUSCLIENT {
    QemuThread signal_thr;
    GDBusConnection * dbus_conn;
    GDBusProxy *proxy;    
};

struct DBUSCLIENTMUAState {
    SysBusDevice parent_obj;
    MemoryRegion iomem;
    struct DBUSCLIENT dbus;
    qemu_irq irq;
};

#define TYPE_DBUS_CLIENT_MUB "dbus_client_mub"
OBJECT_DECLARE_SIMPLE_TYPE(DBUSCLIENTMUBState, DBUS_CLIENT_MUB)

struct DBUSCLIENTMUBState {
    SysBusDevice parent_obj;
    MemoryRegion iomem; 
    struct DBUSCLIENT dbus;
    qemu_irq irq;
};

#endif
