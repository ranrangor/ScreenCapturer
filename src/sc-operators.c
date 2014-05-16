
#include<gtk/gtk.h>
#include"sc-canvas.h"
#include"sc-operators.h"
#include"sc-shape.h"
#include"sc-arrow.h"
#include"sc-painter.h"
#include"../icons/icons.h"
#include"sc-utils.h"




void operator_shape_act(GtkWidget*widget,gpointer d)
{
    g_print("CB,,,shape..\n");
    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[shape]\n");
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_SHAPE);

        GtkWidget*shape=sc_shape_new(canvas);

        sc_canvas_set_operator(canvas,shape);
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

    g_print("CB,,,arrow..\n");
    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[arrow]\n");
    
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_ARROW);

        GtkWidget*shape=sc_arrow_new(canvas);

        sc_canvas_set_operator(canvas,shape);
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

    g_print("CB,,,painter..\n");
    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[painter]\n");
    
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_PAINTER);

        GtkWidget*shape=sc_painter_new(canvas);

        sc_canvas_set_operator(canvas,shape);
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

    g_print("CB,,,text..\n");
    SCCanvas*canvas=SC_CANVAS(d);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
        g_print("Actived...[text]\n");
    
        sc_canvas_operator_button_reset(canvas);
        sc_canvas_operator_toggled(canvas,OPERATOR_TEXT);

        GtkWidget*shape=sc_text_new(canvas);

        sc_canvas_set_operator(canvas,shape);
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

    /*
//    GtkWidget*shape=gtk_button_new_with_label("shape");
    GtkWidget*shape=sc_image_toggle_button_new_by_size(icon_shape_circle,20);
    gtk_box_pack_end(GTK_BOX(operable_box),shape,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(shape),"clicked",G_CALLBACK(operator_shape_act),canvas);//&info);

//    GtkWidget*arrow=gtk_button_new_with_label("arrow");
    GtkWidget*arrow=sc_image_toggle_button_new_by_size(icon_arrow,20);
    gtk_box_pack_end(GTK_BOX(operable_box),arrow,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(arrow),"clicked",G_CALLBACK(operator_arrow_act),canvas);//&info);

//    GtkWidget*painter=gtk_button_new_with_label("painter");
    GtkWidget*painter=sc_image_toggle_button_new_by_size(icon_painter,20);
    gtk_box_pack_end(GTK_BOX(operable_box),painter,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(painter),"clicked",G_CALLBACK(operator_painter_act),canvas);//&info);

//    GtkWidget*text=gtk_button_new_with_label("text");
    GtkWidget*text=sc_image_toggle_button_new_by_size(icon_text,20);
    gtk_box_pack_end(GTK_BOX(operable_box),text,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(text),"clicked",G_CALLBACK(operator_text_act),canvas);//&info);
*/

sc_canvas_add_operator(canvas, OPERATOR_SHAPE  ,icon_shape   ,operator_shape_act);
sc_canvas_add_operator(canvas, OPERATOR_TEXT   ,icon_text    ,operator_text_act);
sc_canvas_add_operator(canvas, OPERATOR_PAINTER,icon_painter ,operator_painter_act);
sc_canvas_add_operator(canvas, OPERATOR_ARROW  ,icon_arrow   ,operator_arrow_act);


}



