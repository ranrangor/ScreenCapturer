


#include"sc-canvas.h"
#include"sc-operators.h"
#include"sc-shape.h"
#include"sc-arrow.h"
#include"sc-painter.h"




void operator_shape_act(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*shape=sc_shape_new(canvas);

    sc_canvas_add_operator(canvas,shape);
    gtk_widget_show_all(canvas);

}

void operator_arrow_act(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*arrow=sc_arrow_new(canvas);

    sc_canvas_add_operator(canvas,arrow);
    gtk_widget_show_all(canvas);

}


void operator_painter_act(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*painter=sc_painter_new(canvas);

    sc_canvas_add_operator(canvas,painter);
    gtk_widget_show_all(canvas);

}



void operator_text_act(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*text=sc_text_new(canvas);

    sc_canvas_add_operator(canvas,text);
    gtk_widget_show_all(canvas);

}




void sc_canvas_register_operables(SCCanvas*canvas,GtkWidget*win)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    sc_canvas_set_appwin(canvas,win);

    //if(!priv->menu){
    g_message("ADD menu...");
    sc_canvas_add_menu(canvas,sc_canvas_get_menu(canvas));
    g_message("---ADD menu...");
    
//    }

    GtkWidget*operable_box=sc_canvas_get_operable_box(canvas);

    GtkWidget*shape=gtk_button_new_with_label("shape");
    gtk_box_pack_end(GTK_BOX(operable_box),shape,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(shape),"clicked",G_CALLBACK(operator_shape_act),canvas);//&info);

    GtkWidget*arrow=gtk_button_new_with_label("arrow");
    gtk_box_pack_end(GTK_BOX(operable_box),arrow,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(arrow),"clicked",G_CALLBACK(operator_arrow_act),canvas);//&info);

    GtkWidget*painter=gtk_button_new_with_label("painter");
    gtk_box_pack_end(GTK_BOX(operable_box),painter,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(painter),"clicked",G_CALLBACK(operator_painter_act),canvas);//&info);

    GtkWidget*text=gtk_button_new_with_label("text");
    gtk_box_pack_end(GTK_BOX(operable_box),text,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(text),"clicked",G_CALLBACK(operator_text_act),canvas);//&info);



}



