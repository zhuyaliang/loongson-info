#ifndef __LOONGSON_MAINBOARD__
#define __LOONGSON_MAINBOARD__

#include <gtk/gtk.h>
G_BEGIN_DECLS

#define LOONGSON_TYPE_MAINBOARD         (loongson_mainboard_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonMainboard, loongson_mainboard, LOONGSON, MAINBOARD, GtkBox);

GtkWidget      *loongson_mainboard_new                     (void);
const char     *loongson_mainboard_get_name                (LoongsonMainboard *mainboard);

G_END_DECLS

#endif
