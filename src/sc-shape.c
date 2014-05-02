#include"sc-shape.h"
#include"sc-operable.h"
#include<math.h>
#include<gtk/gtk.h>




enum{
    NO_TYPE,
    TYPE_RECT,
    TYPE_CIRCLE,
    N_TYPES
};



static void sc_operable_interface_init(SCOperableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(SCShape,sc_shape,GTK_TYPE_WIDGET,
        G_IMPLEMENT_INTERFACE(SC_TYPE_OPERABLE,sc_operable_interface_init))






static gboolean sc_shape_draw(GtkWidget*widget, cairo_t*cr);
static gboolean sc_shape_press(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_shape_release(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_shape_motion(GtkWidget*widget, GdkEventMotion*e);

static void sc_shape_realize(GtkWidget*widget);
static void sc_shape_unrealize(GtkWidget*widget);
static void sc_shape_map(GtkWidget*widget);
static void sc_shape_unmap(GtkWidget*widget);
static void sc_shape_size_allocate(GtkWidget*widget,GtkAllocation* alloc);






static void but2_clicked(GtkWidget*widget,SCOperable*operable)
{

    SCShape*shape=SC_SHAPE(operable);
    shape->line_width=2;
}


static void but5_clicked(GtkWidget*widget,SCOperable*operable)
{

    SCShape*shape=SC_SHAPE(operable);
    shape->line_width=5;

}





GtkWidget*shape_obtain_menu(SCOperable*operable)
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

    iface->toolbutton=gtk_button_new_with_label("shape");
//    iface->
//    iface->toolmenu=create_shape_tool_menu();

    iface->obtain_menu=shape_obtain_menu;

}




static void sc_shape_class_init(SCShapeClass*klass)
{


    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->draw=sc_shape_draw;

    wclass->button_press_event=sc_shape_press;
    wclass->button_release_event=sc_shape_release;

    wclass->realize=sc_shape_realize;
    wclass->unrealize=sc_shape_unrealize;
    wclass->map=sc_shape_map;
    wclass->unmap=sc_shape_unmap;

    wclass->size_allocate=sc_shape_size_allocate;

    wclass->motion_notify_event=sc_shape_motion;


}



static void sc_shape_init(SCShape*obj)
{


    GtkWidget* wobj=GTK_WIDGET(obj);

    //obj->shape_type=TYPE_RECT;
    obj->shape_type=TYPE_CIRCLE;
   
    gtk_widget_set_has_window(wobj,FALSE);

    obj->line_width=5; 
    obj->color.red=1;
    obj->color.alpha=1;

}




static void sc_shape_realize(GtkWidget*widget)
{

    SCShape*shape=SC_SHAPE(widget);

    GdkWindow*event_window;
    GdkWindow*parent_window;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkAllocation allocation;


    gtk_widget_get_allocation(widget,&allocation);

    gtk_widget_set_realized(widget,TRUE);

    parent_window=gtk_widget_get_parent_window(widget);

    GTK_WIDGET_CLASS(sc_shape_parent_class)->realize(widget);



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

    shape->event_window=event_window;


}


static void sc_shape_unrealize(GtkWidget*widget)
{

    SCShape*shape=SC_SHAPE(widget);

    GTK_WIDGET_CLASS(sc_shape_parent_class)->unrealize(widget);

    gdk_window_destroy(shape->event_window);
    gtk_widget_unregister_window(widget,shape->event_window);
    shape->event_window=NULL;

}



static void sc_shape_map(GtkWidget*widget)
{

    SCShape*shape=SC_SHAPE(widget);

    GTK_WIDGET_CLASS(sc_shape_parent_class)->map(widget);


    if(shape->event_window)
        gdk_window_show(shape->event_window);


}



static void sc_shape_unmap(GtkWidget*widget)
{

    SCShape*shape=SC_SHAPE(widget);

    GTK_WIDGET_CLASS(sc_shape_parent_class)->unmap(widget);

    if(shape->event_window)
        gdk_window_hide(shape->event_window);


}


void print_rect(GdkRectangle*r)
{

    g_print("[x    :%4d, y     :%4d]\n", r->x,r->y);
    g_print("[width:%4d, height:%4d]\n", r->width, r->height);


}


static gboolean sc_shape_draw(GtkWidget*widget, cairo_t*cr)
{

    SCShape*shape=SC_SHAPE(widget);


    gdk_cairo_set_source_rgba(cr,&shape->color);


    cairo_set_line_width(cr,shape->line_width);
    

    print_rect(&shape->rectangle);


    cairo_save(cr);

    cairo_translate(cr,(double)shape->rectangle.x,(double)shape->rectangle.y);
    cairo_scale(cr,(shape->rectangle.width),(shape->rectangle.height));


    if(shape->shape_type==TYPE_RECT){    
  
//    cairo_set_line_width(cr,0.01);
    cairo_rectangle(cr,0,0,1,1);

    }else if(shape->shape_type==TYPE_CIRCLE){
    
    cairo_arc(cr,0.5,0.5,0.5,0,2*M_PI);
    
    }

    cairo_restore(cr);
    cairo_stroke(cr);


    return FALSE;


}



static gboolean sc_shape_press(GtkWidget*widget, GdkEventButton*e)
{


    SCShape*shape=SC_SHAPE(widget);

    shape->pressed=TRUE;
    GtkAllocation alloc;

    gtk_widget_get_allocation(widget,&alloc);

    shape->rectangle.x=(int)e->x+alloc.x;
    shape->rectangle.y=(int)e->y+alloc.y;

    shape->rectangle.width=1;
    shape->rectangle.height=1;

    return TRUE;


}



static gboolean sc_shape_release(GtkWidget*widget, GdkEventButton*e)
{

    SCShape*shape=SC_SHAPE(widget);

    shape->pressed=FALSE;

    return TRUE;


}



static gboolean sc_shape_motion(GtkWidget*widget, GdkEventMotion*e)
{

    SCShape*shape=SC_SHAPE(widget);

    if(shape->pressed){
    
    
    shape->rectangle.width=(int)e->x - shape->rectangle.x;
    shape->rectangle.height=(int)e->y - shape->rectangle.y;
    
    
    gtk_widget_queue_draw(widget);
    
    }



}






static void sc_shape_size_allocate(GtkWidget*widget, GtkAllocation*allocation)
{


    SCShape*shape=SC_SHAPE(widget);


    gtk_widget_set_allocation(widget,allocation);


    if(gtk_widget_get_realized(widget)){
    
        gdk_window_move_resize(shape->event_window,allocation->x,allocation->y,
                allocation->width,allocation->height);

    }


}


