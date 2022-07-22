#ifndef __LOONGSON_STATE__
#define __LOONGSON_STATE__

#include <gtk/gtk.h>
G_BEGIN_DECLS

#define LOONGSON_TYPE_STATE         (loongson_state_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonState, loongson_state, LOONGSON, STATE, GtkBox);

GtkWidget      *loongson_state_new                     (void);
const char     *loongson_state_get_name                (LoongsonState *state);

G_END_DECLS

#endif
