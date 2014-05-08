
#ifndef _SC_H_OPERATOR_
#define _SC_H_OPERATOR_





#include"sc-canvas.h"
#include<gtk/gtk.h>



static void shape_clicked(GtkWidget*widget,gpointer d);
static void arrow_clicked(GtkWidget*widget,gpointer d);
static void painter_clicked(GtkWidget*widget,gpointer d);


void sc_canvas_register_operables(SCCanvas*canvas,GtkWidget*win);




#endif
