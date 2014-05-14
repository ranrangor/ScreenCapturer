
#include<gtk/gtk.h>
#include"sc-widthchooser.h"
#include<math.h>



#define N_WIDTH 3
#define GW 1



struct _SCWidthChooserPriv{

    GdkWindow*chooser_window[N_WIDTH];
    int widths[N_WIDTH];
    int n_widths;    
    int current_width;
    int choosed_width;

    int border;
    int side;
    int width;
    int height;

};














static gboolean sc_width_chooser_leave(GtkWidget*widget,GdkEventCrossing*e);
static gboolean sc_width_chooser_enter(GtkWidget*widget,GdkEventCrossing*e);
static gboolean sc_width_chooser_press(GtkWidget*widget,GdkEventButton*e);


static void sc_width_chooser_realize(GtkWidget*widget);
static void sc_width_chooser_unrealize(GtkWidget*widget);
static void sc_width_chooser_map(GtkWidget*widget);
static void sc_width_chooser_unmap(GtkWidget*widget);
static void sc_width_chooser_size_allocate(GtkWidget*widget,GtkAllocation*allocation);

static void sc_width_chooser_get_preferred_width(GtkWidget* widget, gint* min, gint* nat);
//static void sc_width_chooser_get_preferred_height_for_width(GtkWidget* widget, gint* min, gint* nat);
static void sc_width_chooser_get_preferred_height(GtkWidget* widget, gint* min, gint* nat);
//static void sc_width_chooser_get_preferred_width_for_height(GtkWidget* widget, gint* min, gint* nat);

static gboolean sc_width_chooser_draw(GtkWidget*widget,cairo_t*cr);



G_DEFINE_TYPE(SCWidthChooser,sc_width_chooser,GTK_TYPE_WIDGET)

static void sc_width_chooser_class_init(SCWidthChooserClass*klass)
{

    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->get_preferred_width=sc_width_chooser_get_preferred_width;
    wclass->get_preferred_height=sc_width_chooser_get_preferred_height;
//    wclass->get_preferred_height_for_width=sc_width_chooser_get_preferred_height_for_width;
//    wclass->get_preferred_width_for_height=sc_width_chooser_get_preferred_width_for_height;

    wclass->leave_notify_event=sc_width_chooser_leave;
    wclass->enter_notify_event=sc_width_chooser_enter;
    wclass->button_press_event=sc_width_chooser_press;

    wclass->realize=sc_width_chooser_realize;
    wclass->unrealize=sc_width_chooser_unrealize;
    wclass->map=sc_width_chooser_map;
    wclass->unmap=sc_width_chooser_unmap;

    wclass->draw=sc_width_chooser_draw;

    wclass->size_allocate=sc_width_chooser_size_allocate;


    g_type_class_add_private(klass,sizeof(SCWidthChooserPriv));


}




static void init_widths(SCWidthChooser*wc)
{
    SCWidthChooserPriv* priv=wc->priv;

    int table[]={
    4,6,8
    };

    int i;
    for(i=0;i<N_WIDTH;i++){
        priv->widths[i]=table[i];
    
    }


}



static void sc_width_chooser_init(SCWidthChooser*obj)
{

    GtkWidget*widget=GTK_WIDGET(obj);
    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER_GET_PRIV(obj);
    obj->priv=priv;

    priv->n_widths=N_WIDTH;
    priv->side=28;
    priv->border=4;
    priv->current_width=-1;
//    priv->choosed_width=-1;

    init_widths(obj);

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(sc,GTK_STYLE_CLASS_BUTTON);
//    gtk_style_context_add_class(sc,GTK_STYLE_CLASS_CHECK);

    gtk_widget_set_has_window(widget,FALSE);

}




static void sc_width_chooser_get_preferred_width(GtkWidget*widget,gint* min, gint* nat)
{

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    priv->width=(priv->n_widths)*(priv->side+GW) - GW + 2*priv->border;
    g_message("width::%d",priv->width);
    if(min)
        *min=priv->width;
    if(nat)
        *nat=priv->width;


}




static void sc_width_chooser_get_preferred_height(GtkWidget* widget,gint* min, gint* nat)
{

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    priv->height=priv->border*2+priv->side;

    if(min)
        *min=priv->height;
    if(nat)
        *nat=priv->height;


}


static void sc_width_chooser_realize(GtkWidget*widget)
{


    GtkAllocation allocation;
    GdkWindow*parent_window;
    GdkWindowAttr attributes;
    gint attributes_mask;

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

//    gtk_widget_set_realized(widget,TRUE);

    GTK_WIDGET_CLASS(sc_width_chooser_parent_class)->realize(widget);

    gtk_widget_get_allocation(widget,&allocation);

    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.x=0;
    attributes.y=0;
    attributes.width=1;
    attributes.height=1;

    attributes.wclass=GDK_INPUT_ONLY;
    attributes.event_mask=gtk_widget_get_events(widget)|
        GDK_BUTTON_PRESS_MASK|
        GDK_POINTER_MOTION_MASK|
        GDK_ENTER_NOTIFY_MASK|
        GDK_LEAVE_NOTIFY_MASK;

    attributes_mask=GDK_WA_X|GDK_WA_Y;
    
    parent_window=gtk_widget_get_parent_window(widget);

    int i;
    for(i=0;i<priv->n_widths;i++){
    
        priv->chooser_window[i]=gdk_window_new(parent_window,&attributes,attributes_mask);
        gtk_widget_register_window(widget,priv->chooser_window[i]);
    }

    gtk_widget_queue_resize(widget);

}


static void sc_width_chooser_unrealize(GtkWidget*widget)
{

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;


    int i;
    for(i=0; i<priv->n_widths;i++){
    
        gtk_widget_unregister_window(widget,priv->chooser_window[i]);
        gdk_window_destroy(priv->chooser_window[i]);
    }

    GTK_WIDGET_CLASS(sc_width_chooser_parent_class)->unrealize(widget);

}









static void sc_width_chooser_map(GtkWidget*widget)
{
    g_message("Map...");

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;
    GTK_WIDGET_CLASS(sc_width_chooser_parent_class)->map(widget);

    int i;
    for(i=0; i<priv->n_widths;i++){
    
        gdk_window_show(priv->chooser_window[i]);
    }

}




static void sc_width_chooser_unmap(GtkWidget*widget)
{


    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    int i;
    for(i=0; i<priv->n_widths;i++){
    
        gdk_window_hide(priv->chooser_window[i]);
    }

    GTK_WIDGET_CLASS(sc_width_chooser_parent_class)->unmap(widget);
}


static int calculate_side(int width,int height,int border,int n)
{
    int w=width-(n+1)*border;
    int h=height-2*border;

    int s=w/4.0;

    if(s<h)
        return s;
    else
        return h;

}

static void sc_width_chooser_size_allocate(GtkWidget*widget,GtkAllocation*allocation)
{

    g_message("Size Allocation");

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    gtk_widget_set_allocation(widget,allocation);

    
//    priv->side=calculate_side(allocation->width,allocation->height,priv->border,4);


    int width=(priv->side+priv->border)*priv->n_widths+priv->border;
    int height=priv->side+2*priv->border;
    int side=priv->side;

    int pos_row=allocation->y+priv->border;
    int pos_col=allocation->x+priv->border;
    int col_gap=side+GW;

    if(gtk_widget_get_realized(widget)){
        g_message("Move & Resize");
    
    int i;
        for(i=0;i<priv->n_widths;i++){
            gdk_window_move_resize(priv->chooser_window[i],pos_col+i*col_gap, pos_row,side,side);
        }
    }
    gtk_widget_queue_resize(widget);


}


static gboolean sc_width_chooser_draw(GtkWidget*widget,cairo_t*cr)
{


    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget,&alloc);

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);

    gtk_render_background(sc,cr,0,0,alloc.width,alloc.height);
    int side=priv->side;//(priv->height-priv->border*3)/2;
    

    int i;
    if(gtk_widget_get_realized(widget)){

        for(i=0;i<priv->n_widths;i++){

             if(priv->choosed_width==i){
                g_print("Choosed:.%d\t",i);
            cairo_save(cr);
            gtk_cairo_transform_to_window(cr,widget,priv->chooser_window[i]);
            gtk_render_frame(sc,cr,0,0,side,side);

//            gtk_widget_set_state_flags(widget,GTK_STATE_FLAG_ACTIVE,TRUE);
            gtk_render_background(sc,cr,0,0,side,side);
            cairo_restore(cr);
                cairo_set_source_rgba(cr,0.0,0.8,0,1);
            
            }else
            if(priv->current_width==i){
                g_print("Current:.%d\t",i);
            cairo_save(cr);
            gtk_cairo_transform_to_window(cr,widget,priv->chooser_window[i]);
            gtk_render_frame(sc,cr,0,0,side,side);

//            gtk_widget_set_state_flags(widget,GTK_STATE_FLAG_PRELIGHT,TRUE);
            gtk_render_background(sc,cr,0,0,side,side);
            cairo_restore(cr);
                cairo_set_source_rgba(cr,0,0,0.8,1);

            }else{

                g_print("Normal:.%d\t",i);
            cairo_save(cr);
            gtk_cairo_transform_to_window(cr,widget,priv->chooser_window[i]);
            gtk_render_frame(sc,cr,0,0,side,side);

//            gtk_widget_set_state_flags(widget,GTK_STATE_FLAG_NORMAL,TRUE);
            gtk_render_background(sc,cr,0,0,side,side);
            cairo_restore(cr);
                cairo_set_source_rgba(cr,0,0,0.0,1);

            }
          
//GtkStateFlags nstat=gtk_widget_get_state_flags (widget);
//            g_print(":: state:%x\t",nstat);

//            gtk_render_background(sc,cr,0,0,side,side);
            cairo_save(cr);
            gtk_cairo_transform_to_window(cr,widget,priv->chooser_window[i]);
//            gtk_render_frame(sc,cr,0,0,side,side);
            cairo_arc(cr,side/2.0,side/2.0,priv->widths[i]/2.0,0,2*M_PI);
            cairo_fill(cr);
            cairo_restore(cr);
        }
        g_print("\n");
    }

    return TRUE;

}


static int seek_num_of_window(SCWidthChooserPriv* priv, GdkWindow*win)
{

//    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    int i;
    for(i=0; i<priv->n_widths;i++){
        if(priv->chooser_window[i] == win){
            return i;
        }
    }
    return -1;

}


static gboolean sc_width_chooser_enter(GtkWidget*widget,GdkEventCrossing*e)
{
    g_message("Enter notify..");
    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    int nn=seek_num_of_window(priv,e->window);

    if(nn!=-1)
        priv->current_width=nn;

    gtk_widget_queue_draw(widget);

    return TRUE;

}



static gboolean sc_width_chooser_leave(GtkWidget*widget,GdkEventCrossing*e)
{
    g_message("Leave notify..");
    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    priv->current_width=-1;

    gtk_widget_queue_draw(widget);

    return TRUE;

}



static gboolean sc_width_chooser_press(GtkWidget*widget,GdkEventButton*e)
{

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(widget)->priv;

    int nn=seek_num_of_window(priv,e->window);

    if(nn!=-1)
        priv->choosed_width=nn;


    gtk_widget_queue_draw(widget);

    return TRUE;

}






GtkWidget* sc_width_chooser_new(int i)
{

    GtkWidget*wc= (GtkWidget*)g_object_new(SC_TYPE_WIDTH_CHOOSER,NULL);  

    SCWidthChooserPriv*priv=SC_WIDTH_CHOOSER(wc)->priv;
    priv->choosed_width=i;

    return wc;
}



int sc_width_chooser_get_width(SCWidthChooser*wc)
{

    SCWidthChooserPriv*priv=wc->priv;

    return priv->widths[priv->choosed_width];

}













