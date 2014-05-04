
#ifndef _SC_H_WIDTHSETTER_

#define _SC_H_WIDTHSETTER__


#include<gtk/gtk.h>



#define SC_WIDTH_SETTER(obj) (obj)








typedef struct GtkWidget SCWidthSetter;


GtkWidget* sc_width_setter_new(double);
double sc_width_setter_get_value(SCWidthSetter*sett);



#endif
