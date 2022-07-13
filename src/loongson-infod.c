/* vi: set sw=4 ts=4 wrap ai expandtab: */
/*
 * loongson-infod.c: This file is part of loongson-info.
 *
 * Copyright (C) 2022 Xiaotian Wu <wuxiaotian@loongson.cn>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 * */


#include <stdio.h>

#include <gio/gio.h>
#include <glib/gi18n.h>
#include "info-dbus-generated.h"
#include "loongson-infod.h"

#define INFO_DBUS_NAME "cn.loongson.info"
#define INFO_DBUS_PATH "/cn/loongson/info"

struct _InfoDaemon
{
    GObject            parent;
    //Info              *info;
    BusInfo           *skeleton;
    guint              bus_name_id;
    GMainLoop         *loop;
    gboolean           replace;
};

enum {
    PROP_0,
    PROP_LOOP,
    PROP_REPLACE,
    LAST_PROP
};

gboolean info_get_cpu_name (BusInfo *object,
                           GDBusMethodInvocation *invocation,
                           gpointer user_data);
static GParamSpec *properties[LAST_PROP] = { NULL };

G_DEFINE_TYPE (InfoDaemon, info_daemon, G_TYPE_OBJECT)

gboolean info_get_cpu_name (BusInfo *object,
                            GDBusMethodInvocation *invocation,
                            gpointer user_data)
{
    gchar *cpu_name;

    cpu_name = g_strdup("3A5000");
    bus_info_complete_cpu_name (object, invocation, cpu_name);
    g_free(cpu_name);

    return TRUE;
}

static void bus_acquired_handler_cb (GDBusConnection *connection,
                                     const gchar     *name,
                                     gpointer         user_data)
{
    InfoDaemon *daemon;

    GError *error = NULL;
    gboolean exported;

    daemon = INFO_DAEMON (user_data);

    g_signal_connect (daemon->skeleton, "handle-cpu-name", G_CALLBACK (info_get_cpu_name), daemon);

    exported = g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (daemon->skeleton),
            connection, INFO_DBUS_PATH, &error);

    if (!exported)
    {
        g_warning ("Failed to export interface: %s", error->message);
        g_error_free (error);

        g_main_loop_quit (daemon->loop);
    }
}

static void name_lost_handler_cb (GDBusConnection *connection,
                                  const gchar     *name,
                                  gpointer         user_data)
{
    InfoDaemon *daemon;

    daemon = INFO_DAEMON (user_data);
    g_debug("bus name lost\n");

    g_main_loop_quit (daemon->loop);
}

static void info_daemon_constructed (GObject *object)
{
    InfoDaemon *daemon;

    GBusNameOwnerFlags flags;

    daemon = INFO_DAEMON (object);

    G_OBJECT_CLASS (info_daemon_parent_class)->constructed (object);

    flags = G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT;
    if (daemon->replace)
        flags |= G_BUS_NAME_OWNER_FLAGS_REPLACE;

    daemon->bus_name_id = g_bus_own_name (G_BUS_TYPE_SYSTEM,
                                          INFO_DBUS_NAME, flags,
                                          bus_acquired_handler_cb, NULL,
                                          name_lost_handler_cb, daemon, NULL);
}

static void info_daemon_dispose (GObject *object)
{
    InfoDaemon *daemon;

    daemon = INFO_DAEMON (object);

    if (daemon->skeleton != NULL)
    {
        GDBusInterfaceSkeleton *skeleton;

        skeleton = G_DBUS_INTERFACE_SKELETON (daemon->skeleton);
        g_dbus_interface_skeleton_unexport (skeleton);

        g_clear_object (&daemon->skeleton);
    }

    if (daemon->bus_name_id > 0)
    {
        g_bus_unown_name (daemon->bus_name_id);
        daemon->bus_name_id = 0;
    }
    G_OBJECT_CLASS (info_daemon_parent_class)->dispose (object);
}

static void info_daemon_set_property (GObject *object,
                                      guint prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    InfoDaemon *daemon;

    daemon = INFO_DAEMON (object);

    switch (prop_id)
    {
        case PROP_LOOP:
            daemon->loop = g_value_get_pointer(value);
            break;
        case PROP_REPLACE:
            daemon->replace = g_value_get_boolean (value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void info_daemon_class_init (InfoDaemonClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = info_daemon_set_property;

    gobject_class->constructed = info_daemon_constructed;
    gobject_class->dispose = info_daemon_dispose;

    properties[PROP_LOOP] =
        g_param_spec_pointer("loop", "loop", "loop",
                G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE |
                G_PARAM_STATIC_STRINGS);
    properties[PROP_REPLACE] =
        g_param_spec_boolean ("replace", "replace", "replace", FALSE,
                G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE |
                G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties (gobject_class, LAST_PROP, properties);
}

static void info_daemon_init (InfoDaemon *daemon)
{
    daemon->skeleton = bus_info_skeleton_new();
    //daemon->info = info_new();
}

InfoDaemon* info_daemon_new (GMainLoop *loop, gboolean replace)
{
    return g_object_new (INFO_TYPE_DAEMON, "loop", loop, NULL);
}
