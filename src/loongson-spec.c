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
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

struct _LoongsonSpecPrivate
{
    char *name;
};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonSpec, loongson_spec, GTK_TYPE_BOX)

static void
loongson_spec_fill (LoongsonSpec *spec)
{

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
loongson_spec_dispose (GObject *object)
{
    LoongsonSpec *spec;

    spec = LOONGSON_SPEC (object);
    g_free (spec->priv->name);
    G_OBJECT_CLASS (loongson_spec_parent_class)->dispose (object);
}

static void
loongson_spec_class_init (LoongsonSpecClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->constructor = loongson_spec_constructor;
    gobject_class->dispose = loongson_spec_dispose;
}

static void
loongson_spec_init (LoongsonSpec *spec)
{
    spec->priv = loongson_spec_get_instance_private (spec);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (spec), GTK_ORIENTATION_VERTICAL);
    spec->priv->name = g_strdup (_("Loongson Specifications"));
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
