/*************************************************************************
  File Name: loongson-window.c

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

  Created Time: 2022年07月07日 星期四 15时01分28秒
 ************************************************************************/

#include "loongson-window.h"
#include "loongson-perf.h"
#include "loongson-spec.h"
#include "loongson-state.h"
#include "loongson-security.h"
#include "loongson-mainboard.h"

struct _LoongsonWindow
{
    GtkWindow     window;
    GtkWidget    *loongspec;  //  规格 型号、制成、功耗、结温、封装、尺寸
    GtkWidget    *loongmb;    //  主板信息 支持内存类型、微架构、CPU缓存(cache)、MMU、扩展指令
    GtkWidget    *loongperf;   //  性能 核数、内存频率、内存通道数、支持最大内存容量、主频、末级Cache容量
    GtkWidget    *loongsec;   //  安全 ECC 校验
    GtkWidget    *loongstate;  //  属性 例如cpu 温度，风扇转速
};

G_DEFINE_TYPE (LoongsonWindow, loongson_window, GTK_TYPE_WINDOW)

static void
loongson_window_fill (LoongsonWindow *loongwin)
{
    GtkWidget  *box;
    GtkWidget  *sidebar;
    GtkWidget  *stack;
    GtkWidget  *separator;
    const char *name;

    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add (GTK_CONTAINER (loongwin), box);

    sidebar = gtk_stack_sidebar_new ();
    gtk_box_pack_start (GTK_BOX (box), sidebar, FALSE, FALSE, 6);

    stack = gtk_stack_new ();
    gtk_box_pack_start (GTK_BOX (box), stack, TRUE, TRUE, 0);
    gtk_stack_set_transition_type (GTK_STACK (stack), GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN);
    gtk_stack_sidebar_set_stack (GTK_STACK_SIDEBAR (sidebar), GTK_STACK (stack));

    separator = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start (GTK_BOX(box), separator, FALSE, FALSE, 6);

    loongwin->loongspec = loongson_spec_new ();
    name = loongson_spec_get_name (LOONGSON_SPEC (loongwin->loongspec));
    gtk_stack_add_named (GTK_STACK (stack), loongwin->loongspec, name);
    gtk_container_child_set (GTK_CONTAINER (stack), loongwin->loongspec, "title", name, NULL);
    gtk_widget_show (loongwin->loongspec);

    loongwin->loongmb = loongson_mainboard_new ();
    name = loongson_mainboard_get_name (LOONGSON_MAINBOARD (loongwin->loongmb));
    gtk_stack_add_named (GTK_STACK (stack), loongwin->loongmb, name);
    gtk_container_child_set (GTK_CONTAINER (stack), loongwin->loongmb, "title", name, NULL);
    gtk_widget_show (loongwin->loongmb);

    loongwin->loongsec = loongson_security_new ();
    name = loongson_security_get_name (LOONGSON_SECURITY (loongwin->loongsec));
    gtk_stack_add_named (GTK_STACK (stack), loongwin->loongsec, name);
    gtk_container_child_set (GTK_CONTAINER (stack), loongwin->loongsec, "title", name, NULL);
    gtk_widget_show (loongwin->loongsec);

    loongwin->loongstate = loongson_state_new ();
    name = loongson_state_get_name (LOONGSON_STATE (loongwin->loongstate));
    gtk_stack_add_named (GTK_STACK (stack), loongwin->loongstate, name);
    gtk_container_child_set (GTK_CONTAINER (stack), loongwin->loongstate, "title", name, NULL);
    gtk_widget_show (loongwin->loongstate);

    loongwin->loongperf = loongson_perf_new ();
    name = loongson_perf_get_name (LOONGSON_PERF (loongwin->loongperf));
    gtk_stack_add_named (GTK_STACK (stack), loongwin->loongperf, name);
    gtk_container_child_set (GTK_CONTAINER (stack), loongwin->loongperf, "title", name, NULL);
    gtk_widget_show (loongwin->loongperf);

}

static GObject *
loongson_window_constructor (GType                  type,
                             guint                  n_construct_properties,
                             GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonWindow   *loongsonwin;

    obj = G_OBJECT_CLASS (loongson_window_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    loongsonwin = LOONGSON_WINDOW (obj);
    loongson_window_fill (loongsonwin);

    return obj;
}

static void
loongson_window_destroy (GtkWidget *widget)
{
    //LoongsonWindow *loongsonwin;

    //loongsonwin = LOONGSON_WINDOW (widget);

    close_dbus_proxy ();
    GTK_WIDGET_CLASS (loongson_window_parent_class)->destroy (widget);

    gtk_main_quit ();
}

static void
loongson_window_class_init (LoongsonWindowClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass   *gtk_class = GTK_WIDGET_CLASS (klass);

    gobject_class->constructor = loongson_window_constructor;
    gtk_class->destroy = loongson_window_destroy;
}

static void
loongson_window_init (LoongsonWindow *loongsonwin)
{
    GtkWindow  *window;

    window = GTK_WINDOW (loongsonwin);
    gtk_window_set_title (GTK_WINDOW (window), _("Loongson Info"));
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    gtk_window_set_position (window, GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (window), 650, 550);
}

GtkWidget *
loongson_window_new (void)
{
    LoongsonWindow *loongsonwin;
    gboolean        ret;
    g_autoptr(GError) error = NULL;

    ret = init_dbus_proxy (&error);
    if (ret == FALSE)
    {
        loongson_message_dialog (_("init loongson-infod gdbus"),
                                ERROR,
                                "%s", error->message);
        return NULL;
    }

    loongsonwin = g_object_new (LOONGSON_TYPE_WINDOW,
                               "type", GTK_WINDOW_TOPLEVEL,
                                NULL);


    return GTK_WIDGET (loongsonwin);
}
