


#include"sc-canvas.h"
#include"sc-operators.h"
#include"sc-shape.h"
#include"sc-arrow.h"
#include"sc-painter.h"




static void shape_clicked(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*shape=sc_shape_new(canvas);

    sc_canvas_add_op(canvas,shape);
    gtk_widget_show_all(canvas);

}

static void arrow_clicked(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*arrow=sc_arrow_new(canvas);

    sc_canvas_add_op(canvas,arrow);
    gtk_widget_show_all(canvas);

}


static void painter_clicked(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*painter=sc_painter_new(canvas);

    sc_canvas_add_op(canvas,painter);
    gtk_widget_show_all(canvas);

}


void sc_canvas_register_operables(SCCanvas*canvas,GtkWidget*win)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    sc_canvas_set_appwin(canvas,win);

    //if(!priv->menu){
    sc_canvas_add_me(canvas,sc_canvas_get_menu(canvas));
    
//    }

    GtkWidget*operable_box=sc_canvas_get_operable_box(canvas);

    GtkWidget*shape=gtk_button_new_with_label("shape");
    gtk_box_pack_end(GTK_BOX(operable_box),shape,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(shape),"clicked",G_CALLBACK(shape_clicked),canvas);//&info);

    GtkWidget*arrow=gtk_button_new_with_label("arrow");
    gtk_box_pack_end(GTK_BOX(operable_box),arrow,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(arrow),"clicked",G_CALLBACK(arrow_clicked),canvas);//&info);

    GtkWidget*painter=gtk_button_new_with_label("painter");
    gtk_box_pack_end(GTK_BOX(operable_box),painter,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(painter),"clicked",G_CALLBACK(painter_clicked),canvas);//&info);


}



