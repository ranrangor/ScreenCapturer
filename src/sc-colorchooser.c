
#include<gtk/gtk.h>
#include"sc-colorchooser.h"



#define N_COLOR 12



struct _SCColorChooserPriv{


//    GdkWindow*preview;
    GdkWindow*chooser_window[N_COLOR];
    GdkRGBA colors[N_COLOR];
//    GList*chooser_windows;
    int n_colors;    
    int current_color;
    int choosed_color;

    int border;
    int side;
    int width;
    int height;

};














static gboolean sc_color_chooser_motion(GtkWidget*widget,GdkEventMotion*e);
static gboolean sc_color_chooser_press(GtkWidget*widget,GdkEventButton*e);


static void sc_color_chooser_realize(GtkWidget*widget);
static void sc_color_chooser_unrealize(GtkWidget*widget);
static void sc_color_chooser_map(GtkWidget*widget);
static void sc_color_chooser_unmap(GtkWidget*widget);
static void sc_color_chooser_size_allocate(GtkWidget*widget,GtkAllocation*allocation);

static void sc_color_chooser_get_preferred_width(GtkWidget* widget, gint* min, gint* nat);
//static void sc_color_chooser_get_preferred_height_for_width(GtkWidget* widget, gint* min, gint* nat);
static void sc_color_chooser_get_preferred_height(GtkWidget* widget, gint* min, gint* nat);
//static void sc_color_chooser_get_preferred_width_for_height(GtkWidget* widget, gint* min, gint* nat);

static gboolean sc_color_chooser_draw(GtkWidget*widget,cairo_t*cr);



G_DEFINE_TYPE(SCColorChooser,sc_color_chooser,GTK_TYPE_WIDGET)

static void sc_color_chooser_class_init(SCColorChooserClass*klass)
{

    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->get_preferred_width=sc_color_chooser_get_preferred_width;
    wclass->get_preferred_height=sc_color_chooser_get_preferred_height;
//    wclass->get_preferred_height_for_width=sc_color_chooser_get_preferred_height_for_width;
//    wclass->get_preferred_width_for_height=sc_color_chooser_get_preferred_width_for_height;

    wclass->motion_notify_event=sc_color_chooser_motion;
    wclass->button_press_event=sc_color_chooser_press;

    wclass->realize=sc_color_chooser_realize;
    wclass->unrealize=sc_color_chooser_unrealize;
    wclass->map=sc_color_chooser_map;
    wclass->unmap=sc_color_chooser_unmap;

    wclass->draw=sc_color_chooser_draw;

    wclass->size_allocate=sc_color_chooser_size_allocate;


    g_type_class_add_private(klass,sizeof(SCColorChooserPriv));


}




static void init_color(GdkRGBA*cc)
{

    cc[0].red=1.0;
    cc[0].green=1.0;
    cc[0].blue=1.0;
    cc[0].alpha=1.0;

    cc[1].red=0;
    cc[1].green=0;
    cc[1].blue=0;
    cc[1].alpha=1.0;

    cc[2].red=1;
    cc[2].green=0;
    cc[2].blue=0;
    cc[2].alpha=1.0;

    cc[3].red=0;
    cc[3].green=1;
    cc[3].blue=0;
    cc[3].alpha=1.0;

    cc[4].red=0;
    cc[4].green=0;
    cc[4].blue=1;
    cc[4].alpha=1.0;

    cc[5].red=1;
    cc[5].green=1;
    cc[5].blue=0;
    cc[5].alpha=1.0;

    cc[6].red=0;
    cc[6].green=1;
    cc[6].blue=1;
    cc[6].alpha=1.0;

    cc[7].red=1;
    cc[7].green=0;
    cc[7].blue=1;
    cc[7].alpha=1.0;

    cc[8].red=0.5;
    cc[8].green=0.5;
    cc[8].blue=0.5;
    cc[8].alpha=1.0;

    cc[9].red=0.5;
    cc[9].green=0;
    cc[9].blue=0;
    cc[9].alpha=1.0;

    cc[10].red=0;
    cc[10].green=0.5;
    cc[10].blue=0;
    cc[10].alpha=1.0;

    cc[11].red=0;
    cc[11].green=0;
    cc[11].blue=0.5;
    cc[11].alpha=1.0;



}



static void sc_color_chooser_init(SCColorChooser*obj)
{

    GtkWidget*widget=GTK_WIDGET(obj);
    SCColorChooserPriv*priv=SC_COLOR_CHOOSER_GET_PRIV(obj);
    obj->priv=priv;

//    priv->width=200;
//    priv->height=40;
    priv->border=4;
    priv->n_colors=12;
    priv->side=12;

    priv->choosed_color=2;//red

    init_color((GdkRGBA*)&priv->colors);



    gtk_widget_set_has_window(widget,FALSE);

}




static void sc_color_chooser_get_preferred_width(GtkWidget*widget,gint* min, gint* nat)
{

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    priv->width=((priv->n_colors+4)/2*(priv->side+priv->border))+priv->border;
    g_message("width::%d",priv->width);
    if(min)
        *min=priv->width;
    if(nat)
        *nat=priv->width;


}




static void sc_color_chooser_get_preferred_height(GtkWidget* widget,gint* min, gint* nat)
{

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    priv->height=priv->border*3+priv->side*2;

    g_message("height::%d",priv->height);
    if(min)
        *min=priv->height;
    if(nat)
        *nat=priv->height;


}


static void sc_color_chooser_realize(GtkWidget*widget)
{

    g_message("Realize...");

    GtkAllocation allocation;
    GdkWindow*parent_window;
    GdkWindowAttr attributes;
    gint attributes_mask;

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

//    gtk_widget_set_realized(widget,TRUE);

    GTK_WIDGET_CLASS(sc_color_chooser_parent_class)->realize(widget);

    gtk_widget_get_allocation(widget,&allocation);

    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.x=0;
    attributes.y=0;
    attributes.width=1;
    attributes.height=1;

    attributes.wclass=GDK_INPUT_ONLY;
    attributes.event_mask=gtk_widget_get_events(widget)|GDK_BUTTON_PRESS_MASK|GDK_POINTER_MOTION_MASK;

    attributes_mask=GDK_WA_X|GDK_WA_Y;
    
    parent_window=gtk_widget_get_parent_window(widget);

    int i;
    for(i=0;i<priv->n_colors;i++){
    
        priv->chooser_window[i]=gdk_window_new(parent_window,&attributes,attributes_mask);
        gtk_widget_register_window(widget,priv->chooser_window[i]);
    }


//    priv->preview==gdk_window_new(parent_window,&attributes,attributes_mask);

    gtk_widget_queue_resize(widget);

}


static void sc_color_chooser_unrealize(GtkWidget*widget)
{

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;


    int i;
    for(i=0; i<priv->n_colors;i++){
    
        gtk_widget_unregister_window(widget,priv->chooser_window[i]);
        gdk_window_destroy(priv->chooser_window[i]);
    }

    GTK_WIDGET_CLASS(sc_color_chooser_parent_class)->unrealize(widget);

}









static void sc_color_chooser_map(GtkWidget*widget)
{
    g_message("Map...");

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;
    GTK_WIDGET_CLASS(sc_color_chooser_parent_class)->map(widget);

    int i;
    for(i=0; i<priv->n_colors;i++){
    
        gdk_window_show(priv->chooser_window[i]);
    }

}




static void sc_color_chooser_unmap(GtkWidget*widget)
{


    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    int i;
    for(i=0; i<priv->n_colors;i++){
    
        gdk_window_hide(priv->chooser_window[i]);
    }

    GTK_WIDGET_CLASS(sc_color_chooser_parent_class)->unmap(widget);
}

static void sc_color_chooser_size_allocate(GtkWidget*widget,GtkAllocation*allocation)
{

    g_message("Size Allocation");

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    gtk_widget_set_allocation(widget,allocation);


    int width=priv->width;//allocation->width;
    int height=priv->height;//allocation->height;
    int side=priv->side;//(height-priv->border*3)/2;

    int pos_row0=allocation->y+priv->border;
    int pos_row1=allocation->y+priv->border*2+side;

    int pos_preview=allocation->x+priv->border;
    int prev_side=height-2*priv->border;

    int pos_col0=allocation->x+prev_side+2*priv->border;
    int col_gap=side+priv->border;


    if(gtk_widget_get_realized(widget)){
        g_message("Move & Resize");
    
    int i;
    int col;
        for(i=0,col=0;i<priv->n_colors;i+=2){
            gdk_window_move_resize(priv->chooser_window[i],pos_col0+col*col_gap, pos_row0,side,side);
            gdk_window_move_resize(priv->chooser_window[i+1],pos_col0+col*col_gap,pos_row1,side,side);
            col++;
        }
    }
    gtk_widget_queue_resize(widget);


}



void render_border(cairo_t *cr,int x,int y, int width,int height)//, gboolean active)
{

    cairo_save(cr);

    cairo_set_line_join(cr,CAIRO_LINE_JOIN_ROUND);
    cairo_set_source_rgba(cr,0,0,0,1);

    cairo_set_line_width(cr,2);

    cairo_rectangle(cr,x,y,width,height);
    cairo_stroke(cr);
    cairo_set_source_rgba(cr,1,1,1,1);
    cairo_set_line_width(cr,1);
    cairo_rectangle(cr,x,y,width,height);

    cairo_stroke_preserve(cr);
    cairo_fill(cr);
    


    cairo_restore(cr);

}


static gboolean sc_color_chooser_draw(GtkWidget*widget,cairo_t*cr)
{

    g_message("Draw...");

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget,&alloc);

    cairo_set_source_rgba(cr,1,0,0,1);

    int side=priv->side;//(priv->height-priv->border*3)/2;

    int i;
    if(gtk_widget_get_realized(widget)){
    for(i=0;i<priv->n_colors;i++){
    cairo_save(cr);
    gtk_cairo_transform_to_window(cr,widget,priv->chooser_window[i]);


    render_border(cr,0,0,side,side);

    gdk_cairo_set_source_rgba(cr,&priv->colors[i]);

    if(i==priv->current_color){
        cairo_rectangle(cr,0.5,0.5,side,side);
    }else{
        cairo_rectangle(cr,0,0,side,side);
    }
    cairo_fill(cr);
    
    cairo_restore(cr);
    
    }

    }

    int prev_side=priv->height-2*priv->border;

    int prev_x=alloc.x+priv->border;
    int prev_y=alloc.y+priv->border;

    gdk_cairo_set_source_rgba(cr,&priv->colors[priv->choosed_color]);

    render_border(cr,prev_x,prev_y,prev_side,prev_side);
    cairo_rectangle(cr,prev_x,prev_y,prev_side,prev_side);
    cairo_fill(cr);



    return FALSE;


}


static int seek_num_of_window(SCColorChooserPriv* priv, GdkWindow*win)
{

//    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    int i;
    for(i=0; i<priv->n_colors;i++){
    
        if(priv->chooser_window[i] == win){
            return i;
        
        }
    
    }
    return -1;

}


static gboolean sc_color_chooser_motion(GtkWidget*widget,GdkEventMotion*e)
{

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    int nn=seek_num_of_window(priv,e->window);

    if(nn!=-1)
        priv->current_color=nn;


    gtk_widget_queue_draw(widget);


}


static gboolean sc_color_chooser_press(GtkWidget*widget,GdkEventButton*e)
{

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

     int nn=seek_num_of_window(priv,e->window);

    if(nn!=-1)
        priv->choosed_color=nn;



    gtk_widget_queue_draw(widget);


}


















