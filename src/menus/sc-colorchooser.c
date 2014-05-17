
#include<gtk/gtk.h>
#include"sc-colorchooser.h"
#include"../sc-text.h"


#define N_COLOR 12



struct _SCColorChooserPriv{


    GtkWidget* associated_operator;
//    GdkWindow*preview;
    GdkWindow*chooser_window[N_COLOR];
//    GdkRGBA colors[N_COLOR];
    char*colors[N_COLOR];
//    GList*chooser_windows;
    int n_colors;    
    int current_color;
    int choosed_color;

    int border;
    int side;
    int width;
    int height;

};


static char* colortable[]={
    
    "#000000",
    "#ffffff",
    "#ff0000",
    "#7f0000",
    "#00ff00",
    "#007f00",
    "#0000ff",
    "#00007f",
    "#ffff00",
    "#00ffff",
    "#ff00ff",
    "#7f7f7f",
    NULL
};





//static gboolean sc_color_chooser_motion(GtkWidget*widget,GdkEventMotion*e);
static gboolean sc_color_chooser_enter(GtkWidget*widget,GdkEventCrossing*e);
static gboolean sc_color_chooser_leave(GtkWidget*widget,GdkEventCrossing*e);
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

//    wclass->motion_notify_event=sc_color_chooser_motion;
    wclass->enter_notify_event=sc_color_chooser_enter;
    wclass->leave_notify_event=sc_color_chooser_leave;
    wclass->button_press_event=sc_color_chooser_press;

    wclass->realize=sc_color_chooser_realize;
    wclass->unrealize=sc_color_chooser_unrealize;
    wclass->map=sc_color_chooser_map;
    wclass->unmap=sc_color_chooser_unmap;

    wclass->draw=sc_color_chooser_draw;

    wclass->size_allocate=sc_color_chooser_size_allocate;


    g_type_class_add_private(klass,sizeof(SCColorChooserPriv));


}




static void init_color(SCColorChooser*cc)
{

    SCColorChooserPriv* priv=cc->priv;

    int i;
    for(i=0;i<N_COLOR;i++){
        priv->colors[i]=g_strdup(colortable[i]);
    
    }


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

//    init_color((GdkRGBA*)&priv->colors);
    init_color(obj);

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(sc,GTK_STYLE_CLASS_FRAME);

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

//    g_message("height::%d",priv->height);
    if(min)
        *min=priv->height;
    if(nat)
        *nat=priv->height;


}


static void sc_color_chooser_realize(GtkWidget*widget)
{

//    g_message("Realize...");

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
    attributes.event_mask=gtk_widget_get_events(widget)|
        GDK_BUTTON_PRESS_MASK|
        GDK_POINTER_MOTION_MASK|
        GDK_LEAVE_NOTIFY_MASK|
        GDK_ENTER_NOTIFY_MASK;

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



static void render_border(cairo_t *cr,int x,int y, int width,int height)//, gboolean active)
{

    cairo_save(cr);

    cairo_set_line_join(cr,CAIRO_LINE_JOIN_ROUND);
    cairo_set_source_rgba(cr,0,0,0,1);

    cairo_set_line_width(cr,2.5);

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


    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    GdkRGBA c;
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget,&alloc);

//    cairo_set_source_rgba(cr,1,0,0,1);

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
//    gtk_style_context_add_class(sc,"button");
//    gtk_render_background(sc,cr,0,0,alloc.width,alloc.height);

    int side=priv->side;//(priv->height-priv->border*3)/2;

    int i;
    if(gtk_widget_get_realized(widget)){
        for(i=0;i<priv->n_colors;i++){
            cairo_save(cr);
            gtk_cairo_transform_to_window(cr,widget,priv->chooser_window[i]);
    
            render_border(cr,0,0,side,side);
    
            gdk_rgba_parse(&c,priv->colors[i]);
            gdk_cairo_set_source_rgba(cr,&c);
    
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
    int prev_x=priv->border;
    int prev_y=priv->border;

    gdk_rgba_parse(&c,priv->colors[priv->choosed_color]);
    gdk_cairo_set_source_rgba(cr,&c);

    render_border(cr,prev_x,prev_y,prev_side,prev_side);
    cairo_rectangle(cr,prev_x,prev_y,prev_side,prev_side);
    cairo_fill(cr);



    return FALSE;


}

static void sc_color_chooser_do_notify(SCColorChooser*chooser)
{
//    g_print("Do notify....ColorChooser..\n\n");
    SCColorChooserPriv*priv=chooser->priv;

    GtkWidget*wgt=priv->associated_operator;

    if(SC_IS_TEXT(wgt)){

        GtkWidget*tv=sc_text_get_view(SC_TEXT(wgt));
        
        if(!tv){
            g_warning("NULL TV..\n");
            return; 
        }
        GtkTextBuffer*buff=gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
        g_signal_emit_by_name(buff,"changed",wgt,NULL);
    
    }

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


static gboolean sc_color_chooser_enter(GtkWidget*widget,GdkEventCrossing*e)
{
    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    int nn=seek_num_of_window(priv,e->window);

    if(nn!=-1)
        priv->current_color=nn;

    gtk_widget_queue_draw(widget);

    return TRUE;

}



static gboolean sc_color_chooser_leave(GtkWidget*widget,GdkEventCrossing*e)
{
    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

    priv->current_color=-1;

    gtk_widget_queue_draw(widget);

    return TRUE;

}



static gboolean sc_color_chooser_press(GtkWidget*widget,GdkEventButton*e)
{

    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(widget)->priv;

     int nn=seek_num_of_window(priv,e->window);

    if(nn!=-1){
        priv->choosed_color=nn;
        /*Color changed Notify::*/
        sc_color_chooser_do_notify(SC_COLOR_CHOOSER(widget));
    }

    gtk_widget_queue_draw(widget);

    return TRUE;

}






GtkWidget* sc_color_chooser_new(GtkWidget*assoc)
{
    GtkWidget* w= (GtkWidget*)g_object_new(SC_TYPE_COLOR_CHOOSER,NULL);
    SCColorChooserPriv*priv=SC_COLOR_CHOOSER(w)->priv;
    priv->associated_operator=assoc;

    return w;

}



char* sc_color_chooser_get_color(SCColorChooser*cc)
{

    SCColorChooserPriv*priv=cc->priv;

    return priv->colors[priv->choosed_color];

}



int sc_color_chooser_get_table(SCColorChooser*cc,char***table)
{

char** colors=g_strdupv(colortable);
int i;
for(i=0;colortable[i]!=NULL; i++);


*table=colors;

return i;
}









