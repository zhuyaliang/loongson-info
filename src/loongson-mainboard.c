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
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

struct _LoongsonMainboardPrivate
{
    char *name;
};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonMainboard, loongson_mainboard, GTK_TYPE_BOX)

static void
loongson_mainboard_fill (LoongsonMainboard *mainboard)
{

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
loongson_mainboard_dispose (GObject *object)
{
    LoongsonMainboard *mainboard;

    mainboard = LOONGSON_MAINBOARD (object);
    g_free (mainboard->priv->name);
    G_OBJECT_CLASS (loongson_mainboard_parent_class)->dispose (object);
}

static void
loongson_mainboard_class_init (LoongsonMainboardClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->constructor = loongson_mainboard_constructor;
    gobject_class->dispose = loongson_mainboard_dispose;
}

static void
loongson_mainboard_init (LoongsonMainboard *mainboard)
{
    mainboard->priv = loongson_mainboard_get_instance_private (mainboard);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (mainboard), GTK_ORIENTATION_VERTICAL);
    mainboard->priv->name = g_strdup (_("Loongson Mainboard"));
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
