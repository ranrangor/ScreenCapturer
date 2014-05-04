
#include<gtk/gtk.h>
#include"sc-widthsetter.h"





GtkWidget* sc_width_setter_new(double default_val)
{

    GtkWidget*button;
    GtkAdjustment* adjustment;

    adjustment=gtk_adjustment_new(default_val,1.0,15.0,1.0,2.0,0.0);

    button=gtk_spin_button_new(adjustment,0.5,1);
    gtk_widget_show(button);

    return button;

}

double sc_width_setter_get_value(SCWidthSetter*sett)
{

    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(sett));

}



