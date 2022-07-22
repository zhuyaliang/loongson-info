/*************************************************************************
 File Name: loongson-spec.c
 Author: zhuyaliang

 Copyright (C) 2022  zhuyaliang https://github.com/zhuyaliang/
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 Created Time: 2022年07月07日 星期四 18时01分38秒
************************************************************************/
#include "loongson-spec.h"
#include "loongson-utils.h"
#include <sys/utsname.h>

struct _LoongsonSpecPrivate
{
    char *name;
    char *machine;
    char *byte_order;
    char *vendor_id;
    char *model_name;
    char *cpu_family;
    char *technology;
    char *power_waste;
    char *junction_temperature;
    char *package_method;
    char *size;
};

typedef struct loongson_cpu
{
    char *model_name;
    char *technology;
}loongson_cpu;

static struct loongson_cpu loongson_cpus [] =
{
    {"Loongson-3A R4 (Loongson-3A4000)", "28nm"},
    {"Loongson-3A5000", "14nm"},
    {"Loongson-3C5000L", "14nm"},
    {NULL, NULL},

};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonSpec, loongson_spec, GTK_TYPE_BOX)

static void get_cpu_info (LoongsonSpec *spec)
{
    FILE *cpuinfo;
    gchar buffer[128];

    cpuinfo = fopen (PROC_CPUINFO, "r");
    if (!cpuinfo)
        return;

    while (fgets (buffer, 128, cpuinfo))
    {
        gchar **tmp = g_strsplit(buffer, ":", 2);
        if (tmp[0] && tmp[1]) {
            tmp[0] = g_strstrip(tmp[0]);
            tmp[1] = g_strstrip(tmp[1]);

            get_str ("system type", spec->priv->vendor_id);
            get_str ("model name", spec->priv->model_name);
            get_str ("cpu family", spec->priv->cpu_family);
            if (spec->priv->cpu_family == NULL)
                get_str ("cpu model", spec->priv->cpu_family);
        }
        g_strfreev(tmp);
    }
    fclose(cpuinfo);
}

static void get_cpu_technology (LoongsonSpec *spec)
{
    int i = 0;

    while (loongson_cpus[i].model_name != NULL)
    {
        if (g_strcmp0 (loongson_cpus[i].model_name, spec->priv->model_name) == 0)
        {
            spec->priv->technology = g_strdup (loongson_cpus[i].technology);
        }
        i++;
    }
}

static void get_cpu_machine (LoongsonSpec *spec)
{
    struct utsname  u;

    uname(&u);
    spec->priv->machine = g_strdup (u.machine);
}

static void get_cpu_byte_order (LoongsonSpec *spec)
{
    int a = 0x12345678;

    if (*((char *)&a) == 0x12)
        spec->priv->byte_order = g_strdup (_("big endian"));
    else
        spec->priv->byte_order = g_strdup (_("little endian"));

}

static void get_cpu_power_waste (LoongsonSpec *spec)
{
    g_autoptr(GError) error = NULL;

    spec->priv->power_waste = loongson_dbus_call ("PowerWaste", &error);
    if (spec->priv->power_waste == NULL)
    {
        loongson_message_dialog (_("Get loongson spec"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_cpu_junction_temperature (LoongsonSpec *spec)
{
    g_autoptr(GError) error = NULL;

    spec->priv->junction_temperature = loongson_dbus_call ("JunctionTemperature", &error);
    if (spec->priv->junction_temperature == NULL)
    {
        loongson_message_dialog (_("Get loongson spec"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_cpu_packaging_method (LoongsonSpec *spec)
{
    g_autoptr(GError) error = NULL;

    spec->priv->package_method = loongson_dbus_call ("PackagingMethod", &error);
    if (spec->priv->package_method == NULL)
    {
        loongson_message_dialog (_("Get loongson spec"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_cpu_size (LoongsonSpec *spec)
{
    g_autoptr(GError) error = NULL;

    spec->priv->size = loongson_dbus_call ("CpuSizes", &error);
    if (spec->priv->size == NULL)
    {
        loongson_message_dialog (_("Get loongson spec"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void set_spec_data (LoongsonSpec *spec)
{
    spec->priv->name = g_strdup (_("Loongson Specifications"));
    get_cpu_byte_order (spec);
    get_cpu_machine (spec);
    get_cpu_info (spec);
    get_cpu_technology (spec);
    get_cpu_power_waste (spec);
    get_cpu_junction_temperature (spec);
    get_cpu_packaging_method (spec);
    get_cpu_size (spec);
}

static void
loongson_spec_fill (LoongsonSpec *spec)
{
    GtkWidget *vbox;
    GtkWidget *image;
    GtkWidget *label;
    GtkWidget *table;
    g_autoptr(GdkPixbuf) pb = NULL;
    g_autoptr(GdkPixbuf) pb2 = NULL;

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start (GTK_BOX (spec), vbox, FALSE, FALSE, 0);

    pb = gdk_pixbuf_new_from_file (ICONSDIR"loongson-spec.png", NULL);
    pb2 = gdk_pixbuf_scale_simple (pb, 180, 50, GDK_INTERP_BILINEAR);
    image = gtk_image_new_from_pixbuf(pb2);
    gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, FALSE, 0);

    label = gtk_label_new (NULL);
    set_lable_style (label, "black", 13, spec->priv->machine, TRUE);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

    table = grid_widget_new ();
    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 6);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("byte order"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 0, 1, 1);

    label = gtk_label_new (spec->priv->byte_order);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 0, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("CPU Series"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 1, 1, 1);

    label = gtk_label_new (spec->priv->cpu_family);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 1, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("CPU Model Name"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 2, 1, 1);

    label = gtk_label_new (spec->priv->model_name);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 2, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Process Technology"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 3, 1, 1);

    label = gtk_label_new (spec->priv->technology);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 3, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Power Dissipation"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 4, 1, 1);

    label = gtk_label_new (spec->priv->power_waste);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 4, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Junction Temperature"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 5, 1, 1);

    label = gtk_label_new (spec->priv->junction_temperature);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 5, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Packaging Method"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 6, 1, 1);

    label = gtk_label_new (spec->priv->package_method);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 6, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Size"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 7, 1, 1);

    label = gtk_label_new (spec->priv->size);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 7, 1, 1);

}

static GObject *
loongson_spec_constructor (GType                  type,
                           guint                  n_construct_properties,
                           GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonSpec   *spec;

    obj = G_OBJECT_CLASS (loongson_spec_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    spec = LOONGSON_SPEC (obj);
    loongson_spec_fill (spec);

    return obj;
}

static void
loongson_spec_destroy (GtkWidget *widget)
{
    LoongsonSpec *spec;

    spec = LOONGSON_SPEC (widget);
    g_free (spec->priv->name);
    g_free (spec->priv->machine);
    g_free (spec->priv->byte_order);
    g_free (spec->priv->vendor_id);
    g_free (spec->priv->model_name);
    g_free (spec->priv->cpu_family);
    g_free (spec->priv->technology);
}

static void
loongson_spec_class_init (LoongsonSpecClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *gtk_class = GTK_WIDGET_CLASS (klass);

    gobject_class->constructor = loongson_spec_constructor;
    gtk_class->destroy = loongson_spec_destroy;
}

static void
loongson_spec_init (LoongsonSpec *spec)
{
    spec->priv = loongson_spec_get_instance_private (spec);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (spec), GTK_ORIENTATION_VERTICAL);
    set_spec_data (spec);
}

const char *loongson_spec_get_name (LoongsonSpec *spec)
{
    return spec->priv->name;
}

GtkWidget *
loongson_spec_new (void)
{
    LoongsonSpec *spec;

    spec = g_object_new (LOONGSON_TYPE_SPEC, NULL);

    return GTK_WIDGET (spec);
}
