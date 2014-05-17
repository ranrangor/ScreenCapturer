
#include<gtk/gtk.h>
#include"sc-canvas.h"
#include"sc-operators.h"
#include"sc-shape.h"
#include"sc-arrow.h"
#include"sc-painter.h"
#include"sc-text.h"
#include"../icons/icons.h"
#include"sc-utils.h"




void operator_shape_act(GtkWidget*widget,gpointer d)
{
    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[shape]\n");
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_SHAPE);

        SCOperable*oper=sc_shape_new(canvas);

        sc_canvas_set_operator(canvas,oper);
        sc_canvas_operator_set_type(canvas,OPERATOR_SHAPE);

    }else{
        g_print("UNActived...[shape]\n");
    
        sc_canvas_operator_toggled(canvas,-1);
        sc_canvas_hide_toolmenu(canvas);
//        sc_canvas_unset_operator(canvas);

    }

    gtk_widget_show_all(GTK_WIDGET(canvas));

}

void operator_arrow_act(GtkWidget*widget,gpointer d)
{

    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[arrow]\n");
    
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_ARROW);

        SCOperable*oper=sc_arrow_new(canvas);

        sc_canvas_set_operator(canvas,oper);
        sc_canvas_operator_set_type(canvas,OPERATOR_ARROW);

    }else{
    
        g_print("UNActived...[arrow]\n");
        sc_canvas_operator_toggled(canvas,-1);
        sc_canvas_hide_toolmenu(canvas);
    
//        sc_canvas_unset_operator(canvas);
    
    }

    gtk_widget_show_all(GTK_WIDGET(canvas));

}


void operator_painter_act(GtkWidget*widget,gpointer d)
{

    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[painter]\n");
    
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_PAINTER);

        SCOperable*oper=sc_painter_new(canvas);

        sc_canvas_set_operator(canvas,oper);
        sc_canvas_operator_set_type(canvas,OPERATOR_PAINTER);

    }else{
        g_print("UNActived...[painter]\n");
    
        sc_canvas_operator_toggled(canvas,-1);
        sc_canvas_hide_toolmenu(canvas);
    
//        sc_canvas_unset_operator(canvas);
    
    }

    gtk_widget_show_all(GTK_WIDGET(canvas));

}



void operator_text_act(GtkWidget*widget,gpointer d)
{

    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[text]\n");
    
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_TEXT);

        SCOperable*oper=sc_text_new(canvas);

        sc_canvas_set_operator(canvas,oper);
        sc_canvas_operator_set_type(canvas,OPERATOR_TEXT);

    }else{
        g_print("UNActived...[text]\n");
    
        sc_canvas_operator_toggled(canvas,-1);
        sc_canvas_hide_toolmenu(canvas);
    
 //       sc_canvas_unset_operator(canvas);
    }

    gtk_widget_show_all(GTK_WIDGET(canvas));


}




void sc_canvas_register_operables(SCCanvas*canvas,GtkWidget*win)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    sc_canvas_set_appwin(canvas,win);
    sc_canvas_set_menu(canvas,sc_canvas_get_menu(canvas));

    GtkWidget*operable_box=sc_canvas_get_operable_box(canvas);


    sc_canvas_add_operator(canvas, OPERATOR_SHAPE  ,icon_shape   ,operator_shape_act);
    sc_canvas_add_operator(canvas, OPERATOR_TEXT   ,icon_text    ,operator_text_act);
    sc_canvas_add_operator(canvas, OPERATOR_PAINTER,icon_painter ,operator_painter_act);
    sc_canvas_add_operator(canvas, OPERATOR_ARROW  ,icon_arrow   ,operator_arrow_act);


}

//NEEDN'T USE
void sc_canvas_unregister_operables(SCCanvas*canvas)
{


    GtkWidget*operable_box=sc_canvas_get_operable_box(canvas);

    sc_canvas_set_menu(canvas,NULL);

}


