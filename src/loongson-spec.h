#ifndef __LOONGSON_SPEC__
#define __LOONGSON_SPEC__

#include <gtk/gtk.h>
G_BEGIN_DECLS

#define LOONGSON_TYPE_SPEC         (loongson_spec_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonSpec, loongson_spec, LOONGSON, SPEC, GtkBox);

GtkWidget      *loongson_spec_new                     (void);
const char     *loongson_spec_get_name                (LoongsonSpec *spec);

G_END_DECLS

#endif
