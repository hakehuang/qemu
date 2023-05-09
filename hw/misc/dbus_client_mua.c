/*
 * QEMU dbus_client_mua.c
 *
 * Copyright (C) 2022 NXP
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qemu/dbus.h"
#include "qemu/error-report.h"
#include "qapi/error.h"
#include "qom/object_interfaces.h"
#include "qapi/qmp/qerror.h"
#include "qom/object.h"
#include "hw/misc/dbus_client.h"
#include "hw/irq.h"
#include "migration/vmstate.h"

static uint64_t dbus_client_mua_read(void *opaque, hwaddr offset,
                                   unsigned size)
{
    uint32_t ret = 0;
    g_autoptr(GError) err = NULL;
    g_autoptr(GVariant) result = NULL;
    g_autoptr(GVariant) value = NULL;
    DBUSCLIENTMUAState *s = DBUS_CLIENT_MUA(opaque);

    if (!s->dbus.proxy) {
        return ret;
    } else {
        result = g_dbus_proxy_call_sync(s->dbus.proxy, "MUARead",
                                        g_variant_new("(t)", offset),
                                        G_DBUS_CALL_FLAGS_NO_AUTO_START,
                                        -1, NULL, &err);

        if (!result) {
            error_report("%s: Failed to Load: %s", __func__, err->message);
            return -1;
        }

        g_variant_get(result, "(u)", &ret);
    }

    return ret;
}


static void dbus_client_mua_write(void *opaque, hwaddr offset,
        uint64_t value, unsigned size)
{
    DBUSCLIENTMUAState *s = DBUS_CLIENT_MUA(opaque);

    if (!s->dbus.proxy) {
        return;
    } else {
        g_autoptr(GError) err = NULL;
        g_autoptr(GVariant) result = NULL;

        /* TODO need process clear irq if needed */

        /* write to dbus */
        result = g_dbus_proxy_call_sync(s->dbus.proxy, "MUAWrite",
                                        g_variant_new("(tt)",
                                        offset, value),
                                        G_DBUS_CALL_FLAGS_NO_AUTO_START,
                                        -1, NULL, &err);
        if (!result) {
            error_report("%s: Failed to Load: %s", __func__, err->message);
        }
    }
    return;
}

static void
on_signal (GDBusProxy *proxy,
           gchar      *sender_name,
           gchar      *signal_name,
           GVariant   *parameters,
           gpointer    user_data)
{
    gchar *parameters_str;
    DBUSCLIENTMUAState *s = (DBUSCLIENTMUAState *)user_data;

    parameters_str = g_variant_print (parameters, TRUE);
    g_print (" *** Received Signal: %s: %s\n",
           signal_name,
           parameters_str);
    g_free (parameters_str);
   

    qemu_irq_raise(s->irq);

}


static const MemoryRegionOps dbus_client_mua_ops = {
    .read = dbus_client_mua_read,
    .write = dbus_client_mua_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription dbus_client_mua_vm = {
    .name = TYPE_DBUS_CLIENT_MUA,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_END_OF_LIST()
    }
};


static void dbus_client_mua_init(Object *obj)
{
    g_autoptr(GHashTable) proxies = NULL;
    g_autoptr(GError) err = NULL;
    DBUSCLIENTMUAState *s = DBUS_CLIENT_MUA(obj);
    g_autoptr(GVariant) result = NULL;

    /*find the TYPE_DBUS_OBJ object*/
    s->dbus.dbus_conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &err);
    g_assert_no_error(err);

    s->dbus.proxy = g_dbus_proxy_new_sync(s->dbus.dbus_conn,
                      G_DBUS_PROXY_FLAGS_NONE,
                      NULL,     /* GDBusInterfaceInfo */
                      "org.qemu.client",      /* name */
                      "/org/qemu/client",     /* object path */
                      "org.qemu.client.mua", /* interface */
                      NULL,     /* GCancellable */
                      &err);
    g_assert_no_error(err);
    if (!s->dbus.proxy) {
        warn_report("%s: Failed to find proxy Id 0", __func__);
    } else {
        g_dbus_proxy_call_sync(s->dbus.proxy, "HelloWorld",
                                   g_variant_new ("(s)", "mua"),
                                   G_DBUS_CALL_FLAGS_NO_AUTO_START,
                                   -1, NULL, &err);
        g_signal_connect(s->dbus.proxy,
            "g-signal",
            G_CALLBACK (on_signal),
            s);
    }

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);
    memory_region_init_io(&s->iomem, obj, &dbus_client_mua_ops, s, TYPE_DBUS_CLIENT_MUA, 0x1000);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);
}

static void dbus_client_mua_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd = &dbus_client_mua_vm;
}

static const TypeInfo dbus_client_mua_info = {
    .name          = TYPE_DBUS_CLIENT_MUA,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DBUSCLIENTMUAState),
    .instance_init = dbus_client_mua_init,
    .class_init    = dbus_client_mua_class_init
};


static void dbus_client_mua_types(void)
{
    type_register_static(&dbus_client_mua_info);
}

type_init(dbus_client_mua_types)
