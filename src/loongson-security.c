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
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

struct _LoongsonSecurityPrivate
{
    char *name;
};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonSecurity, loongson_security, GTK_TYPE_BOX)

static void
loongson_security_fill (LoongsonSecurity *security)
{

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
loongson_security_dispose (GObject *object)
{
    LoongsonSecurity *security;

    security = LOONGSON_SECURITY (object);
    g_free (security->priv->name);
    G_OBJECT_CLASS (loongson_security_parent_class)->dispose (object);
}

static void
loongson_security_class_init (LoongsonSecurityClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->constructor = loongson_security_constructor;
    gobject_class->dispose = loongson_security_dispose;
}

static void
loongson_security_init (LoongsonSecurity *security)
{
    security->priv = loongson_security_get_instance_private (security);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (security), GTK_ORIENTATION_VERTICAL);
    security->priv->name = g_strdup (_("Loongson Security"));
}

const char *loongson_security_get_name (LoongsonSecurity *security)
{
    return security->priv->name;
}

GtkWidget *
loongson_security_new (void)
{
    LoongsonSecurity *security;

    security = g_object_new (LOONGSON_TYPE_SECURITY, NULL);

    return GTK_WIDGET (security);
}
