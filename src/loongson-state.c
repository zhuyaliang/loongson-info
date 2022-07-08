/*************************************************************************
 File Name: loongson-state.c
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
 
 Created Time: 2022年07月07日 星期四 19时57分02秒
************************************************************************/
#include "loongson-state.h"
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

struct _LoongsonStatePrivate
{
    char *name;
};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonState, loongson_state, GTK_TYPE_BOX)

static void
loongson_state_fill (LoongsonState *state)
{

}

static GObject *
loongson_state_constructor (GType                  type,
                           guint                  n_construct_properties,
                           GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonState   *state;

    obj = G_OBJECT_CLASS (loongson_state_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    state = LOONGSON_STATE (obj);
    loongson_state_fill (state);

    return obj;
}

static void
loongson_state_dispose (GObject *object)
{
    LoongsonState *state;

    state = LOONGSON_STATE (object);
    g_free (state->priv->name);
    G_OBJECT_CLASS (loongson_state_parent_class)->dispose (object);
}

static void
loongson_state_class_init (LoongsonStateClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->constructor = loongson_state_constructor;
    gobject_class->dispose = loongson_state_dispose;
}

static void
loongson_state_init (LoongsonState *state)
{
    state->priv = loongson_state_get_instance_private (state);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (state), GTK_ORIENTATION_VERTICAL);
    state->priv->name = g_strdup (_("Loongson State"));
}

const char *loongson_state_get_name (LoongsonState *state)
{
    return state->priv->name;
}

GtkWidget *
loongson_state_new (void)
{
    LoongsonState *state;

    state = g_object_new (LOONGSON_TYPE_STATE, NULL);

    return GTK_WIDGET (state);
}
