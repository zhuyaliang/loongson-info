/*************************************************************************
 File Name: src/loongson-utils.c
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

 Created Time: 2022年07月08日 星期五 11时05分47秒
************************************************************************/
#include "loongson-utils.h"

#define MSGFORMAT           "<span foreground='red'font_desc='13'>%s </span>"
#define LOONGSON_NAME       "cn.loongson.info"
#define LOONGSON_PATH       "/cn/loongson/info"

static GDBusProxy *proxy;

void close_dbus_proxy (void)
{
    if (proxy != NULL)
    {
        g_object_unref (proxy);
        proxy = NULL;
    }
}

gboolean init_dbus_proxy (GError **error)
{
    proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
                                           G_DBUS_PROXY_FLAGS_NONE,
                                           NULL,
                                           LOONGSON_NAME,
                                           LOONGSON_PATH,
                                           LOONGSON_NAME,
                                           NULL, error);

    if (proxy == NULL)
        return FALSE;

    return TRUE;
}

char *loongson_dbus_call (const gchar *method_name,
                          GError     **error)
{
    GVariant *result;
    char     *strings = NULL;

    result = g_dbus_proxy_call_sync (proxy,
                                     method_name,
                                     NULL,
                                     G_DBUS_CALL_FLAGS_NONE,
                                     -1,
                                     NULL,
                                     error);
    if (!result)
        return NULL;

    if (g_variant_is_of_type (result, G_VARIANT_TYPE ("(s)")))
        g_variant_get (result, "(s)", &strings);

    g_variant_unref (result);

    return strings;
}

int loongson_dbus_call_int (const gchar *method_name,
                            GError     **error)
{
    GVariant *result;
    int       num;

    result = g_dbus_proxy_call_sync (proxy,
                                     method_name,
                                     NULL,
                                     G_DBUS_CALL_FLAGS_NONE,
                                     -1,
                                     NULL,
                                     error);
    if (!result)
        return -1;

    if (g_variant_is_of_type (result, G_VARIANT_TYPE ("(i)")))
        g_variant_get (result, "(i)", &num);

    g_variant_unref (result);

    return num;
}

void set_lable_style (GtkWidget  *lable,
                      const char *color,
                      int         font_szie,
                      const char *text,
                      gboolean    blod)
{
    char *lable_text = NULL;

    if (color == NULL)
    {
        lable_text = g_strdup_printf ("<span weight=\'light\'font_desc=\'%d\'><b>%s</b></span>", font_szie, text);
    }
    else
    {
        if(blod)
        {
            lable_text = g_strdup_printf ("<span foreground=\'%s\'weight=\'light\'font_desc=\'%d\'><b>%s</b></span>",
                             color,
                             font_szie,
                             text);
        }
        else
        {
            lable_text = g_strdup_printf("<span foreground=\'%s\'weight=\'light\'font_desc=\'%d\'>%s</span>",
                            color,
                            font_szie,
                            text);
        }
    }

    gtk_label_set_markup (GTK_LABEL(lable), lable_text);

    g_free (lable_text);
}

int loongson_message_dialog (const char *title, MsgType type, const char *msg,...)
{
    GtkWidget *dialog = NULL;
    va_list    args;
    char      *message;
    int        ret;

    switch(type)
    {
        case ERROR:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "%s",title);
            break;
        }
        case WARING:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_WARNING,
                                             GTK_BUTTONS_OK,
                                             "%s",title);
            break;
        }
        case INFOR:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "%s",title);
            break;
        }
        case QUESTION:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "%s",title);
            break;
        }
        default :
            break;

    }

    va_start (args, msg);
    message = g_strdup_vprintf (msg, args);
    va_end (args);

    gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                                MSGFORMAT,
                                                message);

    gtk_window_set_title (GTK_WINDOW (dialog), _("Message"));
    ret = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    g_free (message);

    return ret;
}

GtkWidget *grid_widget_new (void)
{
    GtkWidget *table;

    table = gtk_grid_new ();
    gtk_widget_set_can_focus (table, FALSE);
    gtk_widget_set_halign (table, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (table, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing (GTK_GRID (table), 12);
    gtk_grid_set_column_spacing (GTK_GRID (table), 18);

    return table;
}
