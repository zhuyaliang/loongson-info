#ifndef __LOONGSON_WINDOW__
#define __LOONGSON_WINDOW__

#include "loongson-utils.h"
G_BEGIN_DECLS

#define LOONGSON_TYPE_WINDOW         (loongson_window_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonWindow, loongson_window, LOONGSON, WINDOW, GtkWindow);

GtkWidget         *loongson_window_new                     (void);

G_END_DECLS

#endif
