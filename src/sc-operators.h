
#ifndef _SC_H_OPERATOR_
#define _SC_H_OPERATOR_





#include"sc-canvas.h"
#include<gtk/gtk.h>




void operator_arrow_act(GtkWidget*widget,gpointer d);
void operator_shape_act(GtkWidget*widget,gpointer d);
void operator_painter_act(GtkWidget*widget,gpointer d);
void operator_text_act(GtkWidget*widget,gpointer d);

void sc_canvas_register_operables(SCCanvas*canvas,GtkWidget*win);




#endif
