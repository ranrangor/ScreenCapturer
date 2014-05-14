#include"sc-painter.h"
#include"sc-canvas.h"
#include"sc-operable.h"
#include<math.h>
#include<gtk/gtk.h>
#include"menus/sc-colorchooser.h"
#include"menus/sc-widthchooser.h"





static void sc_operable_interface_init(SCOperableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(SCPainter,sc_painter,GTK_TYPE_WIDGET,
        G_IMPLEMENT_INTERFACE(SC_TYPE_OPERABLE,sc_operable_interface_init))






static gboolean sc_painter_draw(GtkWidget*widget, cairo_t*cr);
static gboolean sc_painter_press(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_painter_release(GtkWidget*widget, GdkEventButton*e);
static gboolean sc_painter_motion(GtkWidget*widget, GdkEventMotion*e);

static void sc_painter_realize(GtkWidget*widget);
static void sc_painter_unrealize(GtkWidget*widget);
static void sc_painter_map(GtkWidget*widget);
static void sc_painter_unmap(GtkWidget*widget);
static void sc_painter_size_allocate(GtkWidget*widget,GtkAllocation* alloc);






/**/

point*new_point(int x,int y)
{
point*p=g_slice_new(point);
p->x=x;
p->y=y;

return p;

}

void destroy_point(point*p)
{

    g_slice_free(point,p);

}








GtkWidget*painter_obtain_toolmenu(SCOperable*operable)
{
    SCPainter*painter=SC_PAINTER(operable);

    GtkWidget*box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    painter->colorchooser=sc_color_chooser_new();
    GtkWidget*sep=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    painter->widthchooser=sc_width_chooser_new(1);

    gtk_box_pack_start(GTK_BOX(box),painter->widthchooser,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),painter->colorchooser,FALSE,FALSE,0);

    return box;

}

static void sc_operable_interface_init(SCOperableInterface* iface)
{


    iface->obtain_toolmenu=painter_obtain_toolmenu;

}




static void sc_painter_class_init(SCPainterClass*klass)
{


    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->draw=sc_painter_draw;

    wclass->button_press_event=sc_painter_press;
    wclass->button_release_event=sc_painter_release;

    wclass->realize=sc_painter_realize;
    wclass->unrealize=sc_painter_unrealize;
    wclass->map=sc_painter_map;
    wclass->unmap=sc_painter_unmap;

    wclass->size_allocate=sc_painter_size_allocate;

    wclass->motion_notify_event=sc_painter_motion;


}



static void sc_painter_init(SCPainter*obj)
{


    GtkWidget* wobj=GTK_WIDGET(obj);

    //obj->painter_type=TYPE_RECT;
//    obj->painter_type=TYPE_CIRCLE;
   
    gtk_widget_set_has_window(wobj,FALSE);

    obj->points=NULL;

    obj->line_width=5; 
    obj->color.red=1;
    obj->color.alpha=1;

    

}




static void sc_painter_realize(GtkWidget*widget)
{

    SCPainter*painter=SC_PAINTER(widget);

    GdkWindow*event_window;
    GdkWindow*parent_window;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkAllocation allocation;


    gtk_widget_get_allocation(widget,&allocation);

    gtk_widget_set_realized(widget,TRUE);

    parent_window=gtk_widget_get_parent_window(widget);

    GTK_WIDGET_CLASS(sc_painter_parent_class)->realize(widget);



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

    attributes.cursor=gdk_cursor_new(GDK_PENCIL);

    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_CURSOR;



    event_window=gdk_window_new(parent_window,&attributes,attributes_mask);

    gtk_widget_register_window(widget,event_window);

    painter->event_window=event_window;

    g_object_unref(attributes.cursor);


}


static void sc_painter_unrealize(GtkWidget*widget)
{

    SCPainter*painter=SC_PAINTER(widget);

    GTK_WIDGET_CLASS(sc_painter_parent_class)->unrealize(widget);

    gdk_window_destroy(painter->event_window);
    gtk_widget_unregister_window(widget,painter->event_window);
    painter->event_window=NULL;

}



static void sc_painter_map(GtkWidget*widget)
{

    SCPainter*painter=SC_PAINTER(widget);

    GTK_WIDGET_CLASS(sc_painter_parent_class)->map(widget);


    if(painter->event_window)
        gdk_window_show(painter->event_window);


}



static void sc_painter_unmap(GtkWidget*widget)
{

    SCPainter*painter=SC_PAINTER(widget);

    GTK_WIDGET_CLASS(sc_painter_parent_class)->unmap(widget);

    if(painter->event_window)
        gdk_window_hide(painter->event_window);


}

void sc_painter_reset(SCPainter*painter)
{

    g_list_free_full(painter->points,(GDestroyNotify)destroy_point);
    painter->points=NULL;


}

void print_rect(GdkRectangle*r)
{

    g_print("[x    :%4d, y     :%4d]\n", r->x,r->y);
    g_print("[width:%4d, height:%4d]\n", r->width, r->height);


}


static gboolean sc_painter_draw(GtkWidget*widget, cairo_t*cr)
{

    SCPainter*painter=SC_PAINTER(widget);

    
    gdk_cairo_set_source_rgba(cr,&painter->color);
    cairo_set_line_width(cr,painter->line_width);
    
    cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
    
    GList*points=painter->points;

    point*p;

    if(points){
        p=points->data;
        cairo_move_to(cr,p->x,p->y);
    }
    while(points){
   
        p=points->data;

        cairo_line_to(cr,p->x,p->y);

        points=points->next;

    }

    cairo_stroke(cr);


    return FALSE;


}



static gboolean sc_painter_press(GtkWidget*widget, GdkEventButton*e)
{


    SCPainter*painter=SC_PAINTER(widget);

    //propagate event to parent window
    if(e->button==GDK_BUTTON_SECONDARY)
        return FALSE;

    painter->pressed=TRUE;

    char*colorspec=sc_color_chooser_get_color(SC_COLOR_CHOOSER(painter->colorchooser));
    gdk_rgba_parse(&painter->color,colorspec);

    painter->line_width=sc_width_chooser_get_width(SC_WIDTH_CHOOSER(painter->widthchooser));


    point*newpoint=new_point(e->x,e->y);
    painter->points=g_list_append(painter->points,newpoint);

    gtk_widget_queue_draw(widget);

    return TRUE;


}



static gboolean sc_painter_release(GtkWidget*widget, GdkEventButton*e)
{

    SCPainter*painter=SC_PAINTER(widget);

    //propagate event to parent window
    if(e->button==GDK_BUTTON_SECONDARY)
        return FALSE;

    painter->pressed=FALSE;

    SCCanvas* canvas=sc_operable_get_canvas(widget);

    sc_canvas_step_done(canvas);
    sc_painter_reset(painter);




    return TRUE;


}



static gboolean sc_painter_motion(GtkWidget*widget, GdkEventMotion*e)
{

    SCPainter*painter=SC_PAINTER(widget);

    if(painter->pressed){
    
        point*newpoint=new_point(e->x,e->y);
        painter->points=g_list_append(painter->points,newpoint);
        
    gtk_widget_queue_draw(widget);
    
    }


    return TRUE;

}






static void sc_painter_size_allocate(GtkWidget*widget, GtkAllocation*allocation)
{


    SCPainter*painter=SC_PAINTER(widget);


    gtk_widget_set_allocation(widget,allocation);


    if(gtk_widget_get_realized(widget)){
    
        gdk_window_move_resize(painter->event_window,allocation->x,allocation->y,
                allocation->width,allocation->height);

    }


}



SCOperable* sc_painter_new(SCCanvas*canvas)
{
    SCOperable*operable=(SCOperable*)g_object_new(SC_TYPE_PAINTER,NULL);
//    operable->canvas=canvas;
    sc_operable_set_canvas(operable,canvas);
    return operable;
}




