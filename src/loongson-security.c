/*************************************************************************
 File Name: loongson-security.c
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

 Created Time: 2022年07月07日 星期四 20时02分18秒
************************************************************************/
#include "loongson-security.h"
#include "loongson-utils.h"

struct _LoongsonSecurity
{
    GtkBox box;
    char *name;
    char *firewalld;
    char *memory_verifi;
};

G_DEFINE_TYPE (LoongsonSecurity, loongson_security, GTK_TYPE_BOX)

static char *loongson_firewalld_state (void)
{
    FILE *fp = NULL;
    char  cmd[128] = "ps -efH | grep firewalld | grep -v grep";
    char  buf[50] = { 0 };

    fp = popen (cmd, "r");
    if (fread (buf, 50, 1, fp) != 0)
    {
        pclose (fp);
        return g_strdup (_("Enabled"));
    }

    pclose (fp);
    return g_strdup (_("Disabled"));
}

static void get_cpu_memory_verifi (LoongsonSecurity *se)
{
    g_autoptr(GError) error = NULL;

    se->memory_verifi = loongson_dbus_call ("MemoryVerification", &error);
    if (se->memory_verifi == NULL)
    {
        loongson_message_dialog (_("Get loongson security"),
                                 WARING,
                                 "%s", "error->message");
    }
}

static void loongson_security_set_data (LoongsonSecurity *security)
{
    security->name = g_strdup (_("Loongson Security"));
    security->firewalld = loongson_firewalld_state ();
    get_cpu_memory_verifi (security);
}

static void
loongson_security_fill (LoongsonSecurity *security)
{
    GtkWidget *vbox;
    GtkWidget *image;
    GtkWidget *label;
    GtkWidget *table;
    g_autoptr(GdkPixbuf) pb = NULL;
    g_autoptr(GdkPixbuf) pb2 = NULL;

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start (GTK_BOX (security), vbox, FALSE, FALSE, 0);

    pb = gdk_pixbuf_new_from_file (ICONSDIR"loongson-security.png", NULL);
    pb2 = gdk_pixbuf_scale_simple (pb, 120, 120, GDK_INTERP_BILINEAR);
    image = gtk_image_new_from_pixbuf(pb2);
    gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, FALSE, 12);

    table = grid_widget_new ();
    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 6);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Memory Check"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 0, 1, 1);

    label = gtk_label_new (security->memory_verifi);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 0, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Firewall"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 1, 1, 1);

    label = gtk_label_new (security->firewalld);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 1, 1, 1);
}

static GObject *
loongson_security_constructor (GType                  type,
                           guint                  n_construct_properties,
                           GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonSecurity   *security;

    obj = G_OBJECT_CLASS (loongson_security_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    security = LOONGSON_SECURITY (obj);
    loongson_security_fill (security);

    return obj;
}

static void
loongson_security_destroy (GtkWidget *widget)
{
    LoongsonSecurity *security;

    security = LOONGSON_SECURITY (widget);
    g_free (security->name);
    g_free (security->firewalld);
}

static void
loongson_security_class_init (LoongsonSecurityClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *gtk_class = GTK_WIDGET_CLASS (klass);

    gobject_class->constructor = loongson_security_constructor;
    gtk_class->destroy = loongson_security_destroy;
}

static void
loongson_security_init (LoongsonSecurity *security)
{
    gtk_orientable_set_orientation (GTK_ORIENTABLE (security), GTK_ORIENTATION_VERTICAL);
    loongson_security_set_data (security);
}

const char *loongson_security_get_name (LoongsonSecurity *security)
{
    return security->name;
}

GtkWidget *
loongson_security_new (void)
{
    LoongsonSecurity *security;

    security = g_object_new (LOONGSON_TYPE_SECURITY, NULL);

    return GTK_WIDGET (security);
}
