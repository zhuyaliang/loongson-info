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
#include "loongson-utils.h"
#define TIME_WITH_SYS_TIME 1
#include <glibtop/cpu.h>
#include <glibtop/sysinfo.h>
#include <glibtop/swap.h>
#include <glibtop/mem.h>
#include <glibtop/loadavg.h>

enum
{
    CPU_TOTAL,
    CPU_USED,
    N_CPU_STATES
};

struct _LoongsonStatePrivate
{
    char   *name;
    guint   time_id;
    guint64 ncpu;
    guint64 tasks;

    /* union { */
    struct
    {
        guint now; /* 0 -> current, 1 -> last
                      now ^ 1 each time */
        /* times[now], times[now ^ 1] is last */
        guint64 times[2][GLIBTOP_NCPU][N_CPU_STATES];
    } cpu;
    GtkWidget *cpu_label;
    GtkWidget *mem_label;
    GtkWidget *swap_label;
    GtkWidget *loadavg_label;
};

G_DEFINE_TYPE_WITH_PRIVATE (LoongsonState, loongson_state, GTK_TYPE_BOX)

static float get_loongson_cpu_state (LoongsonState *state)
{
#undef  NOW
#undef  LAST
#define NOW  (state->priv->cpu.times[state->priv->cpu.now])
#define LAST (state->priv->cpu.times[state->priv->cpu.now ^ 1])
    guint       i;
    float       cpu_usage = 0.0;
    glibtop_cpu cpu;

    glibtop_get_cpu (&cpu);

    for (i = 0; i < state->priv->ncpu; i++)
    {
        NOW[i][CPU_TOTAL] = cpu.xcpu_total[i];
        NOW[i][CPU_USED] = cpu.xcpu_user[i] + cpu.xcpu_nice[i] + cpu.xcpu_sys[i];
    }

    for (i = 0; i < state->priv->ncpu; i++)
    {
        float load;
        float total, used;

        total = NOW[i][CPU_TOTAL] - LAST[i][CPU_TOTAL];
        used  = NOW[i][CPU_USED]  - LAST[i][CPU_USED];

        load = used / MAX(total, 1.0f);
        cpu_usage += load + 0.1;
    }
    state->priv->cpu.now ^= 1;
#undef NOW
#undef LAST
    return cpu_usage;
}
static float get_loongson_memory_state (LoongsonState *state)
{
    float       mempercent;
    glibtop_mem mem;

    glibtop_get_mem (&mem);

    mempercent  = (float)mem.user  / (float)mem.total;

    return mempercent;
}

static float get_loongson_swap_state (LoongsonState *state)
{
    float        swappercent;
    glibtop_swap swap;

    glibtop_get_swap (&swap);

    swappercent = (swap.total ? (float)swap.used / (float)swap.total : 0.0f);

    return swappercent;
}

static double get_loongson_loadavg_state (LoongsonState *state)
{
    glibtop_loadavg loadavg;

    glibtop_get_loadavg (&loadavg); 

    state->priv->tasks = loadavg.nr_tasks;

    return loadavg.loadavg[2];
}

static gboolean update_loongson_state (LoongsonState *state)
{
    gchar   *text;
    float    cpu;
    float    mem;
    float    swap;
    double   loadavg;

    cpu = get_loongson_cpu_state (state);
    text = g_strdup_printf("%.1f%%", (cpu / state->priv->ncpu) * 100.0f);
    set_lable_style (state->priv->cpu_label, "red", 12, text, TRUE);
    g_free (text);

    mem  = get_loongson_memory_state (state);
    text = g_strdup_printf("%.1f%%", mem * 100.0f);
    set_lable_style (state->priv->mem_label, "red", 12, text, TRUE);
    g_free (text);

    swap = get_loongson_swap_state (state);
    text = g_strdup_printf("%.1f%%", swap * 100.0f);
    set_lable_style (state->priv->swap_label, "red", 12, text, TRUE);
    g_free (text);

    loadavg = get_loongson_loadavg_state (state);
    text = g_strdup_printf("%.1lf%%", loadavg * 100.0f);
    set_lable_style (state->priv->loadavg_label, "red", 12, text, TRUE);
    g_free (text);

    return TRUE;
}

static void
loongson_state_fill (LoongsonState *state)
{
    GtkWidget *vbox;
    GtkWidget *image;
    GtkWidget *label;
    GtkWidget *table;
    g_autoptr(GdkPixbuf) pb = NULL;
    g_autoptr(GdkPixbuf) pb2 = NULL;

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start (GTK_BOX (state), vbox, FALSE, FALSE, 0);

    pb = gdk_pixbuf_new_from_file (ICONSDIR"loongson-state.png", NULL);
    pb2 = gdk_pixbuf_scale_simple (pb, 120, 120, GDK_INTERP_BILINEAR);
    image = gtk_image_new_from_pixbuf(pb2);
    gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, FALSE, 12);

    table = grid_widget_new ();
    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 6);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Temperature"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 0, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 0, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Fan speed"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 1, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 0);
    gtk_grid_attach (GTK_GRID (table) ,label, 1, 1, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Memory Usage"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 2, 1, 1);

    state->priv->mem_label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(state->priv->mem_label), 0);
    gtk_grid_attach (GTK_GRID (table) ,state->priv->mem_label, 1, 2, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Swap Usage"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 3, 1, 1);

    state->priv->swap_label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(state->priv->swap_label), 0);
    gtk_grid_attach (GTK_GRID (table) ,state->priv->swap_label, 1, 3, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("CPU utilization"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 4, 1, 1);

    state->priv->cpu_label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(state->priv->cpu_label), 0);
    gtk_grid_attach (GTK_GRID (table) ,state->priv->cpu_label, 1, 4, 1, 1);

    label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(label), 1);
    set_lable_style (label, "gray", 12, _("Load average"), TRUE);
    gtk_grid_attach (GTK_GRID (table) ,label, 0, 5, 1, 1);

    state->priv->loadavg_label = gtk_label_new (NULL);
    gtk_label_set_xalign (GTK_LABEL(state->priv->loadavg_label), 0);
    gtk_grid_attach (GTK_GRID (table) ,state->priv->loadavg_label, 1, 5, 1, 1);

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
loongson_state_destroy (GtkWidget *widget)
{
    LoongsonState *state;

    state = LOONGSON_STATE (widget);
    g_free (state->priv->name);
    g_source_remove (state->priv->time_id);
}

static void
loongson_state_class_init (LoongsonStateClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *gtk_class = GTK_WIDGET_CLASS (klass);

    gobject_class->constructor = loongson_state_constructor;
    gtk_class->destroy = loongson_state_destroy;
}

static void
loongson_state_init (LoongsonState *state)
{

    state->priv = loongson_state_get_instance_private (state);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (state), GTK_ORIENTATION_VERTICAL);
    state->priv->name = g_strdup (_("Loongson State"));
    state->priv->ncpu = glibtop_get_sysinfo ()->ncpu;
    state->priv->time_id = g_timeout_add (1000, (GSourceFunc) update_loongson_state, state);
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
