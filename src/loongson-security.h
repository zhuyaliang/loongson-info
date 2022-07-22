#ifndef __LOONGSON_SECURITY__
#define __LOONGSON_SECURITY__

#include <gtk/gtk.h>
G_BEGIN_DECLS

#define LOONGSON_TYPE_SECURITY         (loongson_security_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonSecurity, loongson_security, LOONGSON, SECURITY, GtkBox);

GtkWidget      *loongson_security_new                     (void);
const char     *loongson_security_get_name                (LoongsonSecurity *security);

G_END_DECLS

#endif
