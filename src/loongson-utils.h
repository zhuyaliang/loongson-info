#ifndef __LOONGSON_UTILS__
#define __LOONGSON_UTILS__

#include <gtk/gtk.h>
#include <fcntl.h>
#include <sys/types.h>
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>
#include "config.h"

#define PROC_CPUINFO "/proc/cpuinfo"
#define get_str(field_name,ptr)               \
  if (g_str_has_prefix(tmp[0], field_name)) { \
    ptr = g_strdup(tmp[1]);                   \
    g_strfreev(tmp);                          \
    continue;                                 \
  }

typedef enum
{
    ERROR = 0,
    WARING,
    INFOR,
    QUESTION
}MsgType;

G_BEGIN_DECLS

void        set_lable_style         (GtkWidget  *lable ,
                                     const char *color,
                                     int         font_szie,
                                     const char *text,
                                     gboolean    blod);

int         loongson_message_dialog (const char *title,
                                     MsgType     type,
                                     const char *msg,...);

GtkWidget   *grid_widget_new         (void);
G_END_DECLS

#endif
