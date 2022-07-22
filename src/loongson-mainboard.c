/*************************************************************************
 File Name: loongson-mainboard.c
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

 Created Time: 2022年07月07日 星期四 20时05分15秒
************************************************************************/
#include "loongson-mainboard.h"
#include "loongson-utils.h"

struct _LoongsonMainboardPrivate
{
    char *name;
    char *bios_name;
    char *product_name;
    char *memory_style;
    char *micro_architecture;
    char *cpu_cache;
    char *mmu;
    char *calculation_part;
    char *extended_instruction;
    char *hw_virt;
};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonMainboard, loongson_mainboard, GTK_TYPE_BOX)

static void get_loongson_bios_name (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->bios_name = loongson_dbus_call ("BiosName", &error);
    if (mb->priv->bios_name == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_product_name (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->product_name = loongson_dbus_call ("ProductName", &error);
    if (mb->priv->product_name == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_memory_style (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->memory_style = loongson_dbus_call ("MemoryStyle", &error);
    if (mb->priv->memory_style == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_micro_architecture (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->micro_architecture = loongson_dbus_call ("MicroArchitecture", &error);
    if (mb->priv->micro_architecture == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_cpu_cache (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->cpu_cache = loongson_dbus_call ("CpuCache", &error);
    if (mb->priv->cpu_cache == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_mmu (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->mmu = loongson_dbus_call ("MmuStyle", &error);
    if (mb->priv->mmu == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_calculation_part (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->calculation_part = loongson_dbus_call ("CalculationPart", &error);
    if (mb->priv->calculation_part == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_extended_instruction (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->extended_instruction = loongson_dbus_call ("ExtendedInstruction", &error);
    if (mb->priv->extended_instruction == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void get_loongson_hw_virt (LoongsonMainboard *mb)
{
    g_autoptr(GError) error = NULL;

    mb->priv->hw_virt = loongson_dbus_call ("HardwareAssistedVirtualization", &error);
    if (mb->priv->hw_virt == NULL)
    {
        loongson_message_dialog (_("Get loongson mainboard"),
                                 WARING,
                                 "%s", "error->message");
    }
}


static void set_mainboard_data (LoongsonMainboard *mb)
{
    mb->priv->name = g_strdup (_("Loongson Mainboard"));

    get_loongson_bios_name (mb);
    get_loongson_product_name (mb);
    get_loongson_memory_style (mb);
    get_loongson_micro_architecture (mb);
    get_loongson_cpu_cache (mb);
    get_loongson_mmu (mb);
    get_loongson_calculation_part (mb);
    get_loongson_extended_instruction (mb);
    get_loongson_hw_virt (mb);
}

static void
loongson_mainboard_fill (LoongsonMainboard *mainboard)
{

    GtkWidget *vbox;
    GtkWidget *image;
    GtkWidget *label;
    GtkWidget *table;
    g_autoptr(GdkPixbuf) pb = NULL;
    g_autoptr(GdkPixbuf) pb2 = NULL;

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start (GTK_BOX (mainboard), vbox, FALSE, FALSE, 0);

    pb = gdk_pixbuf_new_from_file (ICONSDIR"loongson-mb.png", NULL);
    pb2 = gdk_pixbuf_scale_simple (pb, 150, 120, GDK_INTERP_BILINEAR);
    image = gtk_image_new_from_pixbuf(pb2);
    gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, FALSE, 12);

    table = grid_widget_new ();
    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 6);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Product Name"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 0, 1, 1);

    label = gtk_label_new (mainboard->priv->product_name);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 0, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Memory"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 2, 1, 1);

    label = gtk_label_new (mainboard->priv->memory_style);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 2, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("BIOS Information"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 1, 1, 1);

    label = gtk_label_new (mainboard->priv->bios_name);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 1, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Microarchitecture"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 3, 1, 1);

    label = gtk_label_new (mainboard->priv->micro_architecture);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 3, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("CPU Cache"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 4, 1, 1);

    label = gtk_label_new (mainboard->priv->cpu_cache);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 4, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("MMU"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 5, 1, 1);

    label = gtk_label_new (mainboard->priv->mmu);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 5, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Calculation part"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 6, 1, 1);

    label = gtk_label_new (mainboard->priv->calculation_part);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 6, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Extended instruction"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 7, 1, 1);

    label = gtk_label_new (mainboard->priv->extended_instruction);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 7, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Hardware assisted virtualization"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 8, 1, 1);

    label = gtk_label_new (mainboard->priv->hw_virt);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 8, 1, 1);
}

static GObject *
loongson_mainboard_constructor (GType                  type,
                           guint                  n_construct_properties,
                           GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonMainboard   *mainboard;

    obj = G_OBJECT_CLASS (loongson_mainboard_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    mainboard = LOONGSON_MAINBOARD (obj);
    loongson_mainboard_fill (mainboard);

    return obj;
}

static void
loongson_mainboard_destroy (GtkWidget *widget)
{
    LoongsonMainboard *mainboard;

    mainboard = LOONGSON_MAINBOARD (widget);
    g_free (mainboard->priv->name);
}

static void
loongson_mainboard_class_init (LoongsonMainboardClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *gtk_class = GTK_WIDGET_CLASS (klass);

    gobject_class->constructor = loongson_mainboard_constructor;
    gtk_class->destroy = loongson_mainboard_destroy;
}

static void
loongson_mainboard_init (LoongsonMainboard *mainboard)
{
    mainboard->priv = loongson_mainboard_get_instance_private (mainboard);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (mainboard), GTK_ORIENTATION_VERTICAL);
    set_mainboard_data (mainboard);
}

const char *loongson_mainboard_get_name (LoongsonMainboard *mainboard)
{
    return mainboard->priv->name;
}

GtkWidget *
loongson_mainboard_new (void)
{
    LoongsonMainboard *mainboard;

    mainboard = g_object_new (LOONGSON_TYPE_MAINBOARD, NULL);

    return GTK_WIDGET (mainboard);
}
