#include"sc-arrow.h"
#include"sc-operable.h"
#include<math.h>
#include<gtk/gtk.h>







static void sc_operable_interface_init(SCOperableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(SCArrow,sc_arrow,GTK_TYPE_WIDGET,
        G_IMPLEMENT_INTERFACE(SC_TYPE_OPERABLE,sc_operable_interface_init))






static gboolean sc_arrow_draw(GtkWidget*widget, cairo_t*cr);
static gboolean sc_arrow_press(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_arrow_release(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_arrow_motion(GtkWidget*widget, GdkEventMotion*e);

static void sc_arrow_realize(GtkWidget*widget);
static void sc_arrow_unrealize(GtkWidget*widget);
static void sc_arrow_map(GtkWidget*widget);
static void sc_arrow_unmap(GtkWidget*widget);
static void sc_arrow_size_allocate(GtkWidget*widget,GtkAllocation* alloc);






static void but2_clicked(GtkWidget*widget,SCOperable*operable)
{

    SCArrow*arrow=SC_ARROW(operable);
    arrow->line_width=2;
}


static void but5_clicked(GtkWidget*widget,SCOperable*operable)
{

    SCArrow*arrow=SC_ARROW(operable);
    arrow->line_width=5;

}





GtkWidget*arrow_obtain_menu(SCOperable*operable)
{

    GtkWidget*box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
    GtkWidget*but_2=gtk_button_new_with_label("2");
    GtkWidget*but_5=gtk_button_new_with_label("5");

    g_signal_connect(G_OBJECT(but_2),"clicked",G_CALLBACK(but2_clicked),operable);
    g_signal_connect(G_OBJECT(but_5),"clicked",G_CALLBACK(but5_clicked),operable);


    gtk_box_pack_start(GTK_BOX(box),but_2,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),but_5,FALSE,FALSE,0);

    gtk_widget_show(but_2);
    gtk_widget_show(but_5);
    gtk_widget_show(box);

    return box;



}

static void sc_operable_interface_init(SCOperableInterface* iface)
{

    iface->toolbutton=gtk_button_new_with_label("arrow");
//    iface->
//    iface->toolmenu=create_arrow_tool_menu();

    iface->obtain_menu=arrow_obtain_menu;

}




static void sc_arrow_class_init(SCArrowClass*klass)
{


    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->draw=sc_arrow_draw;

    wclass->button_press_event=sc_arrow_press;
    wclass->button_release_event=sc_arrow_release;

    wclass->realize=sc_arrow_realize;
    wclass->unrealize=sc_arrow_unrealize;
    wclass->map=sc_arrow_map;
    wclass->unmap=sc_arrow_unmap;

    wclass->size_allocate=sc_arrow_size_allocate;

    wclass->motion_notify_event=sc_arrow_motion;


}



static void sc_arrow_init(SCArrow*obj)
{


    GtkWidget* wobj=GTK_WIDGET(obj);

   
    gtk_widget_set_has_window(wobj,FALSE);

    obj->line_width=5; 
    obj->color.red=1;
    obj->color.alpha=1;

}




static void sc_arrow_realize(GtkWidget*widget)
{

    SCArrow*arrow=SC_ARROW(widget);

    GdkWindow*event_window;
    GdkWindow*parent_window;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkAllocation allocation;


    gtk_widget_get_allocation(widget,&allocation);

    gtk_widget_set_realized(widget,TRUE);

    parent_window=gtk_widget_get_parent_window(widget);

    GTK_WIDGET_CLASS(sc_arrow_parent_class)->realize(widget);



    attributes.x=allocation.x;
    attributes.y=allocation.y;
    attributes.width=allocation.width;
    attributes.height=allocation.height;

    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.wclass=GDK_INPUT_ONLY;

    attributes.event_mask=gtk_widget_get_events(widget)|
        GDK_POINTER_MOTION_MASK|
        GDK_BUTTON_PRESS_MASK|
        GDK_BUTTON_RELEASE_MASK;


    attributes_mask=GDK_WA_X|GDK_WA_Y;



    event_window=gdk_window_new(parent_window,&attributes,attributes_mask);

    gtk_widget_register_window(widget,event_window);

    arrow->event_window=event_window;


}


static void sc_arrow_unrealize(GtkWidget*widget)
{

    SCArrow*arrow=SC_ARROW(widget);


    gdk_window_destroy(arrow->event_window);
    gtk_widget_unregister_window(widget,arrow->event_window);
    arrow->event_window=NULL;

    GTK_WIDGET_CLASS(sc_arrow_parent_class)->unrealize(widget);
}



static void sc_arrow_map(GtkWidget*widget)
{

    SCArrow*arrow=SC_ARROW(widget);

    GTK_WIDGET_CLASS(sc_arrow_parent_class)->map(widget);


    if(arrow->event_window)
        gdk_window_show(arrow->event_window);


}



static void sc_arrow_unmap(GtkWidget*widget)
{

    SCArrow*arrow=SC_ARROW(widget);

    GTK_WIDGET_CLASS(sc_arrow_parent_class)->unmap(widget);

    if(arrow->event_window)
        gdk_window_hide(arrow->event_window);


}


void print_rect(GdkRectangle*r)
{

    g_print("[x    :%4d, y     :%4d]\n", r->x,r->y);
    g_print("[width:%4d, height:%4d]\n", r->width, r->height);


}


static gboolean sc_arrow_draw(GtkWidget*widget, cairo_t*cr)
{

    SCArrow*arrow=SC_ARROW(widget);

    int width=gtk_widget_get_allocated_width(widget);
    int height =gtk_widget_get_allocated_height(widget);

    double arrow_width=1.5*arrow->line_width;
    double arrow_length=2*arrow_width;


    double dashs[]={arrow_length,arrow_length,100000};



    cairo_set_line_width(cr,arrow->line_width);
    
    cairo_set_source_rgba(cr,1,0,0,1);

//    print_rect(&arrow->rectangle);

////
//
//    cairo_rectangle(cr,arrow->x0,arrow->y0,arrow->x1-arrow->x0,arrow->y1-arrow->y0);
//
//    cairo_stroke(cr);
//
    cairo_move_to(cr,arrow->x1,arrow->y1);
    cairo_line_to(cr,arrow->x0,arrow->y0);
//
    cairo_set_dash(cr,dashs,3,arrow_length);
    cairo_stroke(cr);
//


    double rad=0.0;
    int dx,dy;
    dx=arrow->x1-arrow->x0;
    dy=arrow->y1-arrow->y0;

    if(dx>0){
   
        rad= atan(((double)dy)/dx);

    }else{//dx<=0 
   
        rad=atan(((double)dy)/dx)+M_PI;

    }

    cairo_save(cr);
    cairo_translate(cr,arrow->x1,arrow->y1);

    cairo_rotate(cr,rad);

    cairo_move_to(cr,0,0);
    cairo_rel_line_to(cr,-arrow_length,-arrow_width);
    cairo_rel_line_to(cr,0,arrow_width*2);
    cairo_rel_line_to(cr,0,0);
    cairo_fill(cr);

    cairo_restore(cr);

    return FALSE;

}



static gboolean sc_arrow_press(GtkWidget*widget, GdkEventButton*e)
{


    SCArrow*arrow=SC_ARROW(widget);

    arrow->pressed=TRUE;
    GtkAllocation alloc;

    gtk_widget_get_allocation(widget,&alloc);

    arrow->x0=(int)e->x;
    arrow->y0=(int)e->y;

    arrow->x1=arrow->x0;
    arrow->y1=arrow->y0;


    return TRUE;


}



static gboolean sc_arrow_release(GtkWidget*widget, GdkEventButton*e)
{

    SCArrow*arrow=SC_ARROW(widget);

    arrow->pressed=FALSE;

    return TRUE;


}



static gboolean sc_arrow_motion(GtkWidget*widget, GdkEventMotion*e)
{

    SCArrow*arrow=SC_ARROW(widget);

    if(arrow->pressed){
    
    arrow->x1=(int)e->x;
    arrow->y1=(int)e->y;
    
    gtk_widget_queue_draw(widget);
    
    }



}






static void sc_arrow_size_allocate(GtkWidget*widget, GtkAllocation*allocation)
{


    SCArrow*arrow=SC_ARROW(widget);


    gtk_widget_set_allocation(widget,allocation);


    if(gtk_widget_get_realized(widget)){
    
        gdk_window_move_resize(arrow->event_window,allocation->x,allocation->y,
                allocation->width,allocation->height);

    }


}


