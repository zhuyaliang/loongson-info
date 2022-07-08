/*************************************************************************
 File Name: loongson-perf.c
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
 
 Created Time: 2022年07月07日 星期四 20时06分47秒
************************************************************************/
#include "loongson-perf.h"

struct _LoongsonPerfPrivate
{
    char *name;
};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonPerf, loongson_perf, GTK_TYPE_BOX)

static void
loongson_perf_fill (LoongsonPerf *perf)
{

}

static GObject *
loongson_perf_constructor (GType                  type,
                           guint                  n_construct_properties,
                           GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonPerf   *perf;

    obj = G_OBJECT_CLASS (loongson_perf_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    perf = LOONGSON_PERF (obj);
    loongson_perf_fill (perf);

    return obj;
}

static void
loongson_perf_dispose (GObject *object)
{
    LoongsonPerf *perf;

    perf = LOONGSON_PERF (object);
    g_free (perf->priv->name);
    G_OBJECT_CLASS (loongson_perf_parent_class)->dispose (object);
}

static void
loongson_perf_class_init (LoongsonPerfClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->constructor = loongson_perf_constructor;
    gobject_class->dispose = loongson_perf_dispose;
}

static void
loongson_perf_init (LoongsonPerf *perf)
{
    perf->priv = loongson_perf_get_instance_private (perf);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (perf), GTK_ORIENTATION_VERTICAL);
    perf->priv->name = g_strdup (_("Loongson Perf"));
}

const char *loongson_perf_get_name (LoongsonPerf *perf)
{
    return perf->priv->name;
}

GtkWidget *
loongson_perf_new (void)
{
    LoongsonPerf *perf;

    perf = g_object_new (LOONGSON_TYPE_PERF, NULL);

    return GTK_WIDGET (perf);
}
