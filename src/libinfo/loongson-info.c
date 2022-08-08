#include <gio/gio.h>
#include <glib/gi18n.h>
#include "info-dbus-generated.h"
#include "loongson-info.h"

#define INFO_DBUS_NAME "cn.loongson.info"
#define INFO_DBUS_PATH "/cn/loongson/info"

struct _LoongsonInfo
{
    GObject          parent;
    GDBusConnection *connection;
    BusInfo         *proxy;
};

G_DEFINE_TYPE (LoongsonInfo, loongson_info, G_TYPE_OBJECT)

const char *loongson_info_get_calculation_part (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_calculation_part_sync (info->proxy,
                                              &ret,
                                              NULL,
                                             &error))
    {
        g_warning ("calculation part call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_cpu_cache (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_cpu_cache_sync (info->proxy,
                                      &ret,
                                       NULL,
                                      &error))
    {
        g_warning ("cpu cache call failed: %s", error->message);
    }

    return ret;
}

int loongson_info_get_cpu_speed (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    int ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), -1);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), -1);

    if (!bus_info_call_cpu_current_speed_sync (info->proxy,
                                              &ret,
                                               NULL,
                                              &error))
    {
        g_warning ("cpu speed call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_cpu_model_name (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_cpu_name_sync (info->proxy,
                                     &ret,
                                      NULL,
                                     &error))
    {
        g_warning ("cpu model name call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_cpu_sizes (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_cpu_sizes_sync (info->proxy,
                                      &ret,
                                       NULL,
                                      &error))
    {
        g_warning ("cpu sizes call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_cpu_technology (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_cpu_temperature_sync (info->proxy,
                                            &ret,
                                             NULL,
                                            &error))
    {
        g_warning ("cpu technology call failed: %s", error->message);
    }

    return ret;
}

int loongson_info_get_cpu_threads (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    int ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), -1);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), -1);

    if (!bus_info_call_cpu_threads_sync (info->proxy,
                                        &ret,
                                         NULL,
                                        &error))
    {
        g_warning ("cpu threads call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_extended_instruction (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_extended_instruction_sync (info->proxy,
                                                 &ret,
                                                  NULL,
                                                 &error))
    {
        g_warning ("extended instruction call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_hw_virt (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_hardware_assisted_virtualization_sync (info->proxy,
                                                             &ret,
                                                              NULL,
                                                             &error))
    {
        g_warning ("hardware assisted virtualization call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_junction_temperature (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_junction_temperature_sync (info->proxy,
                                                 &ret,
                                                  NULL,
                                                 &error))
    {
        g_warning ("junction temperature call failed: %s", error->message);
    }

    return ret;
}

int loongson_info_get_max_cpu_speed (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    int ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), -1);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), -1);

    if (!bus_info_call_maximum_cpu_frequency_sync (info->proxy,
                                                  &ret,
                                                   NULL,
                                                  &error))
    {
        g_warning ("maximun cpu speed call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_max_memory_capacity (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_maximum_memory_capacity_sync (info->proxy,
                                                    &ret,
                                                     NULL,
                                                    &error))
    {
        g_warning ("maximum memory capacity call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_memory_channel (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_memory_channel_sync (info->proxy,
                                            &ret,
                                             NULL,
                                            &error))
    {
        g_warning ("emory channel call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_memory_verification (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_memory_verification_sync (info->proxy,
                                                &ret,
                                                 NULL,
                                                &error))
    {
        g_warning ("memory verifi call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_memory_type (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_memory_style_sync (info->proxy,
                                         &ret,
                                          NULL,
                                         &error))
    {
        g_warning ("memory type call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_micro_architecture (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_micro_architecture_sync (info->proxy,
                                               &ret,
                                                NULL,
                                               &error))
    {
        g_warning ("micro architecture call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_mmu_type (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_mmu_style_sync (info->proxy,
                                      &ret,
                                       NULL,
                                      &error))
    {
        g_warning ("mmu type call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_get_packaging_method (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_packaging_method_sync (info->proxy,
                                              &ret,
                                              NULL,
                                             &error))
    {
        g_warning ("package method call failed: %s", error->message);
    }

    return ret;
}

int loongson_info_get_physical_core (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    int ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), -1);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), -1);

    if (!bus_info_call_physical_kernel_sync (info->proxy,
                                             &ret,
                                              NULL,
                                             &error))
    {
        g_warning ("physical core call failed: %s", error->message);
    }

    return ret;
}

const char *loongson_info_power_waste (LoongsonInfo *info)
{
    g_autoptr(GError) error = NULL;
    char *ret;

    g_return_val_if_fail (LOONGSON_IS_INFO (info), NULL);
    g_return_val_if_fail (BUS_IS_INFO (info->proxy), NULL);

    if (!bus_info_call_power_waste_sync (info->proxy,
                                        &ret,
                                         NULL,
                                        &error))
    {
        g_warning ("power waste call failed: %s", error->message);
    }

    return ret;
}

static void loongson_info_dispose (GObject *object)
{
    LoongsonInfo *info;
    
    info = LOONGSON_INFO (object);

    if (info->proxy != NULL)
    {
        g_object_unref (info->proxy);
    }

    G_OBJECT_CLASS (loongson_info_parent_class)->dispose (object);
}

static void loongson_info_class_init (LoongsonInfoClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->dispose = loongson_info_dispose;
}

static void loongson_info_init (LoongsonInfo *info)
{
    GError *error = NULL;
    
    info->connection = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, &error);
    if (info->connection == NULL)
    {
        if (error != NULL)
        {
            g_warning ("Failed to connect to the D-Bus daemon: %s", error->message);
            g_error_free (error);
        }
        else
        {
            g_warning ("Failed to connect to the D-Bus daemon");
        }
        return;
    }
    
    info->proxy = bus_info_proxy_new_sync (info->connection,
                                           G_DBUS_PROXY_FLAGS_NONE,
                                           INFO_DBUS_NAME,
                                           INFO_DBUS_PATH,
                                           NULL,
                                           &error);
    if (error != NULL)
    {
        g_warning ("loongson info init: getting proxy failed: %s", error->message);
        g_error_free (error);
        return;
    }

    g_dbus_proxy_set_default_timeout (G_DBUS_PROXY (info->proxy), G_MAXINT);
}

LoongsonInfo *loongson_info_new (void)
{
    LoongsonInfo *info;

    info = g_object_new (LOONGSON_TYPE_INFO, NULL);

    if (info->proxy == NULL)
        return NULL;

    return info;
}
