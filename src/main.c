#include "loongson-window.h"
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

int
main (int argc, char **argv)
{
    GtkWidget *window;

    bindtextdomain (GETTEXT_PACKAGE, LUNAR_CALENDAR_LOCALEDIR);
    textdomain (GETTEXT_PACKAGE);

    gtk_init (&argc, &argv);
    
    window = loongson_window_new (); 
    gtk_widget_show_all (window);

    gtk_main ();
    return 0;
}
