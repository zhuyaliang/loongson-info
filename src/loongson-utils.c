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

void set_lable_style (GtkWidget  *lable ,
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
