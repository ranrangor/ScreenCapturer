
#ifndef _SC_H_FONTSIZECHOOSER_

#define _SC_H_FONTSIZECHOOSER_


#include<gtk/gtk.h>



#define SC_FONTSIZE_CHOOSER(obj) (obj)








typedef GtkWidget SCFontsizeChooser;


GtkWidget* sc_fontsize_chooser_new(int );
int sc_fontsize_chooser_get_size(SCFontsizeChooser*chooser);
int sc_fontsize_chooser_get_table(SCFontsizeChooser*chooser,int**table);



#endif
