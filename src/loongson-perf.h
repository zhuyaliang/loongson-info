#ifndef __LOONGSON_PERF__
#define __LOONGSON_PERF__

#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

G_BEGIN_DECLS

#define LOONGSON_TYPE_PERF         (loongson_perf_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonPerf, loongson_perf, LOONGSON, PERF, GtkBox);

GtkWidget      *loongson_perf_new                     (void);
const char     *loongson_perf_get_name                (LoongsonPerf *perf);

G_END_DECLS

#endif
