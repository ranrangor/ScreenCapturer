
#include<gtk/gtk.h>
#include"sc-shapechooser.h"
#include"../../icons/icons.h"




struct _SCShapeChooserPriv{

    GdkWindow*chooser_window;
    GdkPixbuf*shapepxf[N_SHAPE];

    int n_shapes;    
    int choosed_shape;
    int border;
    int side;
    int width;
    int height;
    gboolean pointeron;

};





static void sc_shape_chooser_next_type(SCShapeChooser*chooser);


static void sc_shape_chooser_finalize(GObject*object);

static gboolean sc_shape_chooser_enter(GtkWidget*widget,GdkEventCrossing*e);
static gboolean sc_shape_chooser_leave(GtkWidget*widget,GdkEventCrossing*e);
static gboolean sc_shape_chooser_press(GtkWidget*widget,GdkEventButton*e);


static void sc_shape_chooser_realize(GtkWidget*widget);
static void sc_shape_chooser_unrealize(GtkWidget*widget);
static void sc_shape_chooser_map(GtkWidget*widget);
static void sc_shape_chooser_unmap(GtkWidget*widget);
static void sc_shape_chooser_size_allocate(GtkWidget*widget,GtkAllocation*allocation);

static void sc_shape_chooser_get_preferred_width(GtkWidget* widget, gint* min, gint* nat);
//static void sc_shape_chooser_get_preferred_height_for_width(GtkWidget* widget, gint* min, gint* nat);
static void sc_shape_chooser_get_preferred_height(GtkWidget* widget, gint* min, gint* nat);
//static void sc_shape_chooser_get_preferred_width_for_height(GtkWidget* widget, gint* min, gint* nat);

static gboolean sc_shape_chooser_draw(GtkWidget*widget,cairo_t*cr);



G_DEFINE_TYPE(SCShapeChooser,sc_shape_chooser,GTK_TYPE_WIDGET)

static void sc_shape_chooser_class_init(SCShapeChooserClass*klass)
{

    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);
    GObjectClass*gklass=G_OBJECT_CLASS(klass);

    gklass->finalize=sc_shape_chooser_finalize;

    wclass->get_preferred_width=sc_shape_chooser_get_preferred_width;
    wclass->get_preferred_height=sc_shape_chooser_get_preferred_height;
    
    wclass->enter_notify_event=sc_shape_chooser_enter;
    wclass->leave_notify_event=sc_shape_chooser_leave;
    wclass->button_press_event=sc_shape_chooser_press;

    wclass->realize=sc_shape_chooser_realize;
    wclass->unrealize=sc_shape_chooser_unrealize;
    wclass->map=sc_shape_chooser_map;
    wclass->unmap=sc_shape_chooser_unmap;

    wclass->draw=sc_shape_chooser_draw;

    wclass->size_allocate=sc_shape_chooser_size_allocate;


    g_type_class_add_private(klass,sizeof(SCShapeChooserPriv));


}


static void sc_shape_chooser_finalize(GObject*object)
{

    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(object)->priv;

    int i;
    for(i=0; i<N_SHAPE;i++){
        if(priv->shapepxf[i]){
            g_object_unref(priv->shapepxf[i]);
//            priv->shapepxf[i]=NULL;
        }
    }

    G_OBJECT_CLASS(sc_shape_chooser_parent_class)->finalize(object);



}



static void sc_shape_chooser_init(SCShapeChooser*obj)
{

    GtkWidget*widget=GTK_WIDGET(obj);
    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER_GET_PRIV(obj);
    obj->priv=priv;

    priv->border=4;
    priv->n_shapes=N_SHAPE;
    priv->side=28;

    priv->choosed_shape=SHAPE_RECT;

    GdkPixbuf*pxf;
        
    pxf=gdk_pixbuf_new_from_inline(-1,icon_shape_rect,FALSE,NULL);
    priv->shapepxf[SHAPE_RECT]=gdk_pixbuf_scale_simple(pxf,priv->side,priv->side,GDK_INTERP_HYPER);
    g_object_unref(pxf);

    pxf=gdk_pixbuf_new_from_inline(-1,icon_shape_circle,FALSE,NULL);
    priv->shapepxf[SHAPE_CIRCLE]=gdk_pixbuf_scale_simple(pxf,priv->side,priv->side,GDK_INTERP_HYPER);
    g_object_unref(pxf);


    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(sc,GTK_STYLE_CLASS_BUTTON);

    gtk_widget_set_has_window(widget,FALSE);

}




static void sc_shape_chooser_get_preferred_width(GtkWidget*widget,gint* min, gint* nat)
{

    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;

    priv->width= priv->side+priv->border+priv->border;
    g_message("width::%d",priv->width);
    if(min)
        *min=priv->width;
    if(nat)
        *nat=priv->width;


}




static void sc_shape_chooser_get_preferred_height(GtkWidget* widget,gint* min, gint* nat)
{

    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;

    priv->height=priv->border*2+priv->side;

    g_message("height::%d",priv->height);
    if(min)
        *min=priv->height;
    if(nat)
        *nat=priv->height;


}


static void sc_shape_chooser_realize(GtkWidget*widget)
{

//    g_message("Realize...");

    GtkAllocation allocation;
    GdkWindow*parent_window;
    GdkWindowAttr attributes;
    gint attributes_mask;

    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;


    GTK_WIDGET_CLASS(sc_shape_chooser_parent_class)->realize(widget);

    gtk_widget_get_allocation(widget,&allocation);

    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.x=0;
    attributes.y=0;
    attributes.width=1;
    attributes.height=1;

    attributes.wclass=GDK_INPUT_ONLY;
    attributes.event_mask=gtk_widget_get_events(widget)|
        GDK_BUTTON_PRESS_MASK|
        GDK_LEAVE_NOTIFY_MASK|
        GDK_ENTER_NOTIFY_MASK;

    attributes_mask=GDK_WA_X|GDK_WA_Y;
    
    parent_window=gtk_widget_get_parent_window(widget);

    
    priv->chooser_window=gdk_window_new(parent_window,&attributes,attributes_mask);
    gtk_widget_register_window(widget,priv->chooser_window);


    gtk_widget_queue_resize(widget);

}


static void sc_shape_chooser_unrealize(GtkWidget*widget)
{
    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;


    gtk_widget_unregister_window(widget,priv->chooser_window);
    gdk_window_destroy(priv->chooser_window);

    GTK_WIDGET_CLASS(sc_shape_chooser_parent_class)->unrealize(widget);

}






static void sc_shape_chooser_map(GtkWidget*widget)
{
    g_message("Map...");

    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;
    GTK_WIDGET_CLASS(sc_shape_chooser_parent_class)->map(widget);

    
    gdk_window_show(priv->chooser_window);

}




static void sc_shape_chooser_unmap(GtkWidget*widget)
{


    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;

    
    gdk_window_hide(priv->chooser_window);

    GTK_WIDGET_CLASS(sc_shape_chooser_parent_class)->unmap(widget);
}

static void sc_shape_chooser_size_allocate(GtkWidget*widget,GtkAllocation*allocation)
{

    g_message("Size Allocation");

    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;

    gtk_widget_set_allocation(widget,allocation);


    int width=priv->width;//allocation->width;
    int height=priv->height;//allocation->height;
    int side=priv->side;//(height-priv->border*3)/2;

    int pos_row=allocation->y+priv->border;


    int pos_col=allocation->x+priv->border;


    if(gtk_widget_get_realized(widget)){
        g_message("Move & Resize");
    
        gdk_window_move_resize(priv->chooser_window,pos_col, pos_row,side,side);
    }
    gtk_widget_queue_resize(widget);


}




static gboolean sc_shape_chooser_draw(GtkWidget*widget,cairo_t*cr)
{

    g_message("Draw...");

    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;

    GdkRGBA c;
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget,&alloc);

//    cairo_set_source_rgba(cr,1,0,0,1);

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
//    gtk_style_context_add_class(sc,GTK_STYLE_CLASS_BUTTON);
    if(priv->pointeron){
        gtk_style_context_set_state(sc,GTK_STATE_FLAG_ACTIVE);
    }else{
        gtk_style_context_set_state(sc,GTK_STATE_FLAG_NORMAL);
    }

    int side=priv->side;//(priv->height-priv->border*3)/2;

    cairo_save(cr);
    gtk_cairo_transform_to_window(cr,widget,priv->chooser_window);
    gtk_render_background(sc,cr,0,0,priv->side,priv->side);
//    gtk_render_frame(sc,cr,0,0,priv->side,priv->side);
    cairo_restore(cr);


    int i;
    if(gtk_widget_get_realized(widget)){
        cairo_save(cr);
        gtk_cairo_transform_to_window(cr,widget,priv->chooser_window);
   
        switch(priv->choosed_shape){
            case SHAPE_RECT:
                gdk_cairo_set_source_pixbuf(cr,priv->shapepxf[SHAPE_RECT],0,0);
                cairo_paint(cr);
                break;
            case SHAPE_CIRCLE:
                gdk_cairo_set_source_pixbuf(cr,priv->shapepxf[SHAPE_CIRCLE],0,0);
                cairo_paint(cr);
                break;

            default:
                cairo_set_source_rgba(cr,1,0,0,0.8);
                cairo_paint(cr);

        }

        cairo_restore(cr);
    }


    return FALSE;


}


static gboolean sc_shape_chooser_enter(GtkWidget*widget,GdkEventCrossing*e)
{
    g_message("Enter notify..");
    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;

    priv->pointeron=TRUE;
    gtk_widget_queue_draw(widget);

    return TRUE;

}



static gboolean sc_shape_chooser_leave(GtkWidget*widget,GdkEventCrossing*e)
{
    g_message("Leave notify..");
    SCShapeChooserPriv*priv=SC_SHAPE_CHOOSER(widget)->priv;

    priv->pointeron=FALSE;

    gtk_widget_queue_draw(widget);

    return TRUE;

}


static void sc_shape_chooser_next_type(SCShapeChooser*chooser)
{

    SCShapeChooserPriv*priv=chooser->priv;

    priv->choosed_shape =(priv->choosed_shape+1)%N_SHAPE;

}



static gboolean sc_shape_chooser_press(GtkWidget*widget,GdkEventButton*e)
{

    SCShapeChooser*chooser=SC_SHAPE_CHOOSER(widget);
    SCShapeChooserPriv*priv=chooser->priv;

    sc_shape_chooser_next_type(chooser);

    gtk_widget_queue_draw(widget);

    return TRUE;

}





GtkWidget* sc_shape_chooser_new(int type)
{
    SCShapeChooser* chooser=(SCShapeChooser*)g_object_new(SC_TYPE_SHAPE_CHOOSER,NULL);
    chooser->priv->choosed_shape=CLAMP(type,0,N_SHAPE);
    return (GtkWidget*)chooser;
}



int sc_shape_chooser_get_shape(SCShapeChooser*cc)
{

    SCShapeChooserPriv*priv=cc->priv;

    return priv->choosed_shape;

}













