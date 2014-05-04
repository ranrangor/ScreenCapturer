#include"sc-line.h"
#include"sc-operable.h"
#include<math.h>
#include<gtk/gtk.h>







static void sc_operable_interface_init(SCOperableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(SCLine,sc_line,GTK_TYPE_WIDGET,
        G_IMPLEMENT_INTERFACE(SC_TYPE_OPERABLE,sc_operable_interface_init))






static gboolean sc_line_draw(GtkWidget*widget, cairo_t*cr);
static gboolean sc_line_press(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_line_release(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_line_motion(GtkWidget*widget, GdkEventMotion*e);

static void sc_line_realize(GtkWidget*widget);
static void sc_line_unrealize(GtkWidget*widget);
static void sc_line_map(GtkWidget*widget);
static void sc_line_unmap(GtkWidget*widget);
static void sc_line_size_allocate(GtkWidget*widget,GtkAllocation* alloc);






static void but2_clicked(GtkWidget*widget,SCOperable*operable)
{

    SCLine*line=SC_LINE(operable);
    line->line_width=2;
}


static void but5_clicked(GtkWidget*widget,SCOperable*operable)
{

    SCLine*line=SC_LINE(operable);
    line->line_width=5;

}





GtkWidget*line_obtain_menu(SCOperable*operable)
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

    iface->toolbutton=gtk_button_new_with_label("line");
//    iface->
//    iface->toolmenu=create_line_tool_menu();

    iface->obtain_menu=line_obtain_menu;

}




static void sc_line_class_init(SCLineClass*klass)
{


    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->draw=sc_line_draw;

    wclass->button_press_event=sc_line_press;
    wclass->button_release_event=sc_line_release;

    wclass->realize=sc_line_realize;
    wclass->unrealize=sc_line_unrealize;
    wclass->map=sc_line_map;
    wclass->unmap=sc_line_unmap;

    wclass->size_allocate=sc_line_size_allocate;

    wclass->motion_notify_event=sc_line_motion;


}



static void sc_line_init(SCLine*obj)
{


    GtkWidget* wobj=GTK_WIDGET(obj);

   
    gtk_widget_set_has_window(wobj,FALSE);

    obj->line_width=5; 
    obj->color.red=1;
    obj->color.alpha=1;

}




static void sc_line_realize(GtkWidget*widget)
{

    SCLine*line=SC_LINE(widget);

    GdkWindow*event_window;
    GdkWindow*parent_window;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkAllocation allocation;


    gtk_widget_get_allocation(widget,&allocation);

    gtk_widget_set_realized(widget,TRUE);

    parent_window=gtk_widget_get_parent_window(widget);

    GTK_WIDGET_CLASS(sc_line_parent_class)->realize(widget);



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

    line->event_window=event_window;


}


static void sc_line_unrealize(GtkWidget*widget)
{

    SCLine*line=SC_LINE(widget);


    gdk_window_destroy(line->event_window);
    gtk_widget_unregister_window(widget,line->event_window);
    line->event_window=NULL;

    GTK_WIDGET_CLASS(sc_line_parent_class)->unrealize(widget);
}



static void sc_line_map(GtkWidget*widget)
{

    SCLine*line=SC_LINE(widget);

    GTK_WIDGET_CLASS(sc_line_parent_class)->map(widget);


    if(line->event_window)
        gdk_window_show(line->event_window);


}



static void sc_line_unmap(GtkWidget*widget)
{

    SCLine*line=SC_LINE(widget);

    GTK_WIDGET_CLASS(sc_line_parent_class)->unmap(widget);

    if(line->event_window)
        gdk_window_hide(line->event_window);


}


void print_rect(GdkRectangle*r)
{

    g_print("[x    :%4d, y     :%4d]\n", r->x,r->y);
    g_print("[width:%4d, height:%4d]\n", r->width, r->height);


}


static gboolean sc_line_draw(GtkWidget*widget, cairo_t*cr)
{

    SCLine*line=SC_LINE(widget);

    int width=gtk_widget_get_allocated_width(widget);
    int height =gtk_widget_get_allocated_height(widget);

    cairo_set_line_width(cr,line->line_width);
    
    cairo_set_source_rgba(cr,1,0,0,1);

//    print_rect(&line->rectangle);

//
    cairo_move_to(cr,line->x1,line->y1);
    cairo_line_to(cr,line->x0,line->y0);
//
    cairo_stroke(cr);



    return FALSE;

}



static gboolean sc_line_press(GtkWidget*widget, GdkEventButton*e)
{


    SCLine*line=SC_LINE(widget);

    line->pressed=TRUE;
    GtkAllocation alloc;

    gtk_widget_get_allocation(widget,&alloc);

    line->x0=(int)e->x;
    line->y0=(int)e->y;

    line->x1=line->x0;
    line->y1=line->y0;


    return TRUE;


}



static gboolean sc_line_release(GtkWidget*widget, GdkEventButton*e)
{

    SCLine*line=SC_LINE(widget);

    line->pressed=FALSE;

    return TRUE;


}



static gboolean sc_line_motion(GtkWidget*widget, GdkEventMotion*e)
{

    SCLine*line=SC_LINE(widget);

    if(line->pressed){
    
    line->x1=(int)e->x;
    line->y1=(int)e->y;
    
    gtk_widget_queue_draw(widget);
    
    }



}






static void sc_line_size_allocate(GtkWidget*widget, GtkAllocation*allocation)
{


    SCLine*line=SC_LINE(widget);


    gtk_widget_set_allocation(widget,allocation);


    if(gtk_widget_get_realized(widget)){
    
        gdk_window_move_resize(line->event_window,allocation->x,allocation->y,
                allocation->width,allocation->height);

    }


}


