#include<gtk/gtk.h>
#include"sc-canvas.h"
#include"sc-window.h"
#include"sc-operable.h"
#include"sc-operators.h"
#include"sc-arrow.h"
#include"sc-shape.h"
#include"sc-text.h"
#include"sc-utils.h"



static SCCanvas* ref_canvas;
#define BUF_SIZE 100

#define X 0
#define Y 1
#define XY 2



enum{

    PROP_0,
    PROP_X,
    PROP_Y,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_PIXBUF

};






struct _SCCanvasPriv{

    GList* pixbufs;
    GtkWidget*appwin;
//    gint current_pixbuf;
//    GtkAllocation position;
    GdkRectangle position;
    
    GtkWidget*operator;
    GdkWindow*window;

//    GtkWidget*popup_menu;

    GtkWidget*menu;//GtkBox
    GdkWindow*menuwindow;
    GdkRectangle menu_position;
    int menu_dragpos[XY];

    GtkWidget*toolmenu;//GtkBox
    GdkWindow*toolmenuwindow;
    GdkRectangle toolmenu_position;
    int tolmenu_dragpos[XY];

    GtkWidget*operable_box;
    guint show_menu:1;
    guint show_toolmenu:1;
    guint menu_drag:1;


//    SCOperable* registered_operables[N_OPERABLES];
//    GtkWidget* act_buttons[N_OPERABLES];

};



static void sc_canvas_set_property(GObject*obj,guint prop_id,const GValue*value,GParamSpec*pspec);
static void sc_canvas_get_property(GObject*obj,guint prop_id,GValue*value,GParamSpec*pspec);

static void sc_canvas_realize(GtkWidget*widget);
static void sc_canvas_unrealize(GtkWidget*widget);
static void sc_canvas_size_allocate(GtkWidget*widget,GtkAllocation*allocation);
static void sc_canvas_get_preferred_width(GtkWidget*widget,gint *min,gint *nat);
static void sc_canvas_get_preferred_height(GtkWidget*widget,gint *min,gint *nat);

static gboolean sc_canvas_press(GtkWidget* widget, GdkEventButton*e);
static gboolean sc_canvas_release(GtkWidget* widget, GdkEventButton*e);
static gboolean sc_canvas_motion(GtkWidget* widget, GdkEventMotion*e);
static gboolean sc_canvas_popupmenu(GtkWidget* widget);


static void sc_canvas_map(GtkWidget*widget);
static void sc_canvas_unmap(GtkWidget*widget);


//static void sc_canvas_add(GtkContainer*container,GtkWidget*w);
//static void sc_canvas_remove(GtkContainer*container,GtkWidget*w);
//static void sc_canvas_forall(GtkContainer*container,gboolean include_internal,GtkCallback callback, gpointer callback_data);

static gboolean sc_canvas_draw(GtkWidget* widget,cairo_t*cr);



G_DEFINE_TYPE(SCCanvas,sc_canvas,GTK_TYPE_BOX)



static void sc_canvas_class_init(SCCanvasClass* scklass)
{

    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(scklass);

    GObjectClass*oclass=G_OBJECT_CLASS(scklass);

    GtkContainerClass*cclass=GTK_CONTAINER_CLASS(scklass);



    oclass->set_property=sc_canvas_set_property;
    oclass->get_property=sc_canvas_get_property;

    
    wclass->realize=sc_canvas_realize;
    wclass->unrealize=sc_canvas_unrealize;

    wclass->map=sc_canvas_map;
    wclass->unmap=sc_canvas_unmap;
    wclass->size_allocate=sc_canvas_size_allocate;
    wclass->draw=sc_canvas_draw;
    wclass->motion_notify_event=sc_canvas_motion;
    wclass->button_press_event=sc_canvas_press;
    wclass->button_release_event=sc_canvas_release;

    wclass->popup_menu=sc_canvas_popupmenu;
/*
    cclass->add=sc_canvas_add;
    cclass->remove=sc_canvas_remove;
    cclass->forall=sc_canvas_forall;
*/
    g_object_class_install_property(oclass,PROP_X,
            g_param_spec_int("x","X","Position.X",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_Y,
            g_param_spec_int("y","Y","Position.Y",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_WIDTH,
            g_param_spec_int("width","Width","Position.width",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_HEIGHT,
            g_param_spec_int("height","Height","Position.height",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_PIXBUF,
            g_param_spec_pointer("pixbuf","Pixbuf","pixbuf of current selected rectangle",G_PARAM_READWRITE));

    g_type_class_add_private(scklass,sizeof(SCCanvasPriv));

}





static void sc_canvas_init(SCCanvas* scobj)
{
    
    SCCanvasPriv*priv=SC_CANVAS_GET_PRIV(scobj);

    scobj->priv=priv;

    GtkWidget*widget=GTK_WIDGET(scobj);

    gtk_widget_set_has_window(widget,FALSE);


    priv->pixbufs=NULL;
//    priv->current_pixbuf=0;
    priv->operator=NULL;
    priv->menu=NULL;
    priv->show_menu=TRUE;

//    priv->menu_position.y=200;

//    priv->popup_menu=sc_canvas_get_popup_menu(scobj);

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(sc,GTK_STYLE_CLASS_MENU);


}



static void sc_canvas_set_property(GObject*obj,guint prop_id,const GValue*value,GParamSpec*pspec)
{

    SCCanvas*canvas=SC_CANVAS(obj);
    SCCanvasPriv*priv=canvas->priv;

    GdkPixbuf*pf;

    switch(prop_id)
    {
    
        case PROP_X:
            priv->position.x=g_value_get_int(value);
            break;

        case PROP_Y:
            priv->position.y=g_value_get_int(value);

            break;

        case PROP_WIDTH:
            priv->position.width=g_value_get_int(value);

            break;
        case PROP_HEIGHT:
            priv->position.height=g_value_get_int(value);

            break;

        case PROP_PIXBUF:
            pf=gdk_pixbuf_copy((GdkPixbuf*)g_value_get_pointer(value));
            priv->pixbufs=g_list_prepend(priv->pixbufs,pf);
            
            break;
        default:

            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj,prop_id,pspec);

    }

    gtk_widget_queue_resize(GTK_WIDGET(obj));


}


static void sc_canvas_get_property(GObject*obj,guint prop_id,GValue*value,GParamSpec*pspec)
{


    SCCanvas*canvas=SC_CANVAS(obj);

    SCCanvasPriv*priv=canvas->priv;

    GdkPixbuf*pf;
    GList*lastl;


    switch(prop_id)
    {
    
        case PROP_X:
            g_value_set_int(value,priv->position.x);
            break;

        case PROP_Y:
            g_value_set_int(value,priv->position.y);

            break;

        case PROP_WIDTH:
            g_value_set_int(value,priv->position.width);

            break;
        case PROP_HEIGHT:
            g_value_set_int(value,priv->position.height);

            break;
        case PROP_PIXBUF:
//            lastl=g_list_first(priv->pixbufs);
            pf=gdk_pixbuf_copy(priv->pixbufs->data);
            g_value_set_pointer(value,pf);
            break;

        default:

            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj,prop_id,pspec);

    
    }


}


static void sc_canvas_realize(GtkWidget*widget)
{
    gtk_widget_set_realized(widget,TRUE);

    GTK_WIDGET_CLASS(sc_canvas_parent_class)->realize(widget);


    GtkAllocation allocation;

    GdkWindow* parent_window;
    GdkWindow* window;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GdkScreen* screen;

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

    gtk_widget_get_allocation(widget,&allocation);


    attributes.window_type=GDK_WINDOW_CHILD;

        attributes.x=priv->position.x;
        attributes.y=priv->position.y;
        attributes.width=priv->position.width;
        attributes.height=priv->position.height;
    
    
    attributes.wclass=GDK_INPUT_OUTPUT;
    attributes.visual=gtk_widget_get_visual(widget);
    attributes.event_mask=gtk_widget_get_events(widget)|
        GDK_EXPOSURE_MASK|
        GDK_BUTTON_PRESS_MASK|
        GDK_BUTTON_RELEASE_MASK|
        GDK_POINTER_MOTION_MASK;

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_TCROSS);

    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL|GDK_WA_CURSOR;

    parent_window=gtk_widget_get_parent_window(widget);

    window=gdk_window_new(parent_window,&attributes,attributes_mask);


    gtk_widget_register_window(widget,window);
    priv->window=window;

    g_object_unref(attributes.cursor);
    if(priv->operator){
        gtk_widget_set_parent_window(priv->operator,priv->window);
//        gtk_widget_set_parent(priv->operator,widget);
    }

    GtkStyleContext*sc=gtk_widget_get_style_context (widget);
    gtk_style_context_set_background (sc, priv->window);

//    gtk_container_forall(GTK_CONTAINER(widget),set_parent_win,widget);

/* menuwindow */

    attributes.window_type=GDK_WINDOW_TEMP;
    attributes.x=0;
    attributes.y=0;
    attributes.width=1;
    attributes.height=1;

    screen=gtk_widget_get_screen(widget);
//    attributes.visual=gdk_screen_get_rgba_visual(screen);
    attributes.visual=gtk_widget_get_visual(widget);
    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_FLEUR);
    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL|GDK_WA_CURSOR;


    parent_window=gdk_screen_get_root_window(screen);

    priv->menuwindow=gdk_window_new(parent_window,&attributes,attributes_mask);
    gtk_widget_register_window(widget,priv->menuwindow);
//    priv->menu_cr=gdk_cairo_create(priv->menuwindow);

    gtk_style_context_set_background(sc,priv->menuwindow);

    if(priv->menu){
        gtk_widget_set_parent_window(priv->menu,priv->menuwindow);
//        gtk_widget_set_parent(priv->menu,widget);
    }



    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL;

    priv->toolmenuwindow=gdk_window_new(parent_window,&attributes,attributes_mask);
    gtk_widget_register_window(widget,priv->toolmenuwindow);

    gtk_style_context_set_background(sc,priv->toolmenuwindow);

    if(priv->toolmenu){
        gtk_widget_set_parent_window(priv->toolmenu,priv->toolmenuwindow);
    }


}



static void sc_canvas_unrealize(GtkWidget*widget)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;
    gtk_widget_set_realized(widget,FALSE);


    gtk_widget_unregister_window(widget,priv->window);
    gdk_window_destroy(priv->window);
    priv->window=NULL;


    gtk_widget_unregister_window(widget,priv->menuwindow);
    gdk_window_destroy(priv->menuwindow);
    priv->menuwindow=NULL;



    GTK_WIDGET_CLASS(sc_canvas_parent_class)->unrealize(widget);


}






static void sc_canvas_size_allocate(GtkWidget*widget,GtkAllocation*allocation)
{

    SCCanvasPriv* priv=SC_CANVAS(widget)->priv;
    
    gtk_widget_set_allocation(widget,allocation);

//    gtk_container_forall(GTK_CONTAINER(widget), allocate_child,widget);
    GTK_WIDGET_CLASS(sc_canvas_parent_class)->size_allocate(widget,allocation);

    if(!gtk_widget_get_realized(widget))
        return;

     gdk_window_move_resize(priv->window,
                 allocation->x+priv->position.x,
                 allocation->y+priv->position.y,
                 priv->position.width,
                 priv->position.height);


     GtkAllocation child_allocation;

     child_allocation.x=0;//allocation->x+priv->position.x;
     child_allocation.y=0;//allocation->y+priv->position.y;
     child_allocation.width= priv->position.width;
     child_allocation.height= priv->position.height;

     if(priv->operator)
         gtk_widget_size_allocate(priv->operator,&child_allocation);
//            gtk_widget_size_allocate(priv->operator,&child_allocation);
 


    GtkRequisition menuR;
    GtkRequisition toolmenuR;
    GtkAllocation menuA;
    GtkAllocation toolmenuA;


    if(priv->menu)
    {
        /*
        if(!priv->show_menu && gdk_window_is_visible(priv->menuwindow)){
            gdk_window_hide(priv->menuwindow);
            gdk_window_hide(priv->toolmenuwindow);
            return;
        }
        if(priv->show_menu && !gdk_window_is_visible(priv->menuwindow)){
            gdk_window_show(priv->menuwindow);
            gdk_window_show(priv->toolmenuwindow);

        }
*/
        g_message("Allocate menu_window");
        gtk_widget_get_preferred_size(priv->menu,&menuR,NULL);

    gdk_window_move_resize(priv->menuwindow,
            priv->menu_position.x, priv->menu_position.y,
            menuR.width, menuR.height);
    
    
    menuA.x=0;//allocation->x;
    menuA.y=0;//allocation->y;
    menuA.width=menuR.width;
    menuA.height=menuR.height;

    priv->menu_position.width=menuA.width;
    priv->menu_position.height=menuA.height;

    gtk_widget_size_allocate(priv->menu,&menuA);
    }

    if(priv->toolmenu){
    
        g_message("Allocate toolmenu_window");
        gtk_widget_get_preferred_size(priv->toolmenu,&toolmenuR,NULL);

    priv->toolmenu_position.x=priv->menu_position.x+5;
    priv->toolmenu_position.y=priv->menu_position.y+menuR.height+2;

    gdk_window_move_resize(priv->toolmenuwindow,
            priv->toolmenu_position.x, priv->toolmenu_position.y,
            toolmenuR.width, toolmenuR.height);
    
    
    toolmenuA.x=0;//allocation->x;
    toolmenuA.y=0;//allocation->y;
    toolmenuA.width=toolmenuR.width;
    toolmenuA.height=toolmenuR.height;

    priv->toolmenu_position.width=toolmenuA.width;
    priv->toolmenu_position.height=toolmenuA.height;

    gtk_widget_size_allocate(priv->toolmenu,&toolmenuA);
    
    }

}


static gboolean sc_canvas_draw(GtkWidget* widget,cairo_t*cr)
{

//    g_message("_Draw()");
    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
    
    if(gtk_cairo_should_draw_window(cr,priv->menuwindow)){
    cairo_save(cr);
    gtk_cairo_transform_to_window(cr,widget,priv->menuwindow);
//    cairo_set_source_rgba(cr,0.9,0.9,0.9,0.9);
    gtk_render_background(sc,cr,0,0,priv->menu_position.width,priv->menu_position.height);
//    cairo_rectangle(cr,0,0,priv->menu_position.width,priv->menu_position.height);
//    cairo_fill(cr);
    cairo_restore(cr);
    
    }
/*
    g_print("menu.x:%d,menu.y:%d  menu.width:%d,menu.height:%d\n",
            priv->menu_position.x,priv->menu_position.y,
            priv->menu_position.width,priv->menu_position.height);
*/
    if(gtk_cairo_should_draw_window(cr,priv->toolmenuwindow)){
    cairo_save(cr);
    gtk_cairo_transform_to_window(cr,widget,priv->toolmenuwindow);
//    cairo_set_source_rgba(cr,0.9,0.9,0.9,0.9);
    gtk_render_background(sc,cr,0,0,priv->toolmenu_position.width,priv->toolmenu_position.height);
//    cairo_rectangle(cr,0,0,priv->toolmenu_position.width,priv->toolmenu_position.height);
//    cairo_fill(cr);
    cairo_restore(cr);
    }

     GList*l=priv->pixbufs;
    if(l){
        gdk_cairo_set_source_pixbuf(cr,l->data,priv->position.x,priv->position.y);
        cairo_paint(cr);
    }else{
        sc_canvas_step_done(SC_CANVAS(widget));

    }

    return GTK_WIDGET_CLASS(sc_canvas_parent_class)->draw(widget,cr);

}



static void sc_canvas_map(GtkWidget*widget)
{

    g_message("Mapping...");
    GTK_WIDGET_CLASS(sc_canvas_parent_class)->map(widget);


    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

//    gtk_container_forall(GTK_CONTAINER(widget),map_child,widget);

    if(priv->window)
        gdk_window_show(priv->window);

    if(priv->menuwindow)
        gdk_window_show(priv->menuwindow);

    if(priv->toolmenuwindow)
        gdk_window_show(priv->toolmenuwindow);

}


static void sc_canvas_unmap(GtkWidget*widget)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

//    gtk_container_forall(GTK_CONTAINER(widget),unmap_child,widget);

    if(priv->window)
        gdk_window_hide(priv->window);

    if(priv->menuwindow)
        gdk_window_hide(priv->menuwindow);

    if(priv->toolmenuwindow)
        gdk_window_hide(priv->toolmenuwindow);

    GTK_WIDGET_CLASS(sc_canvas_parent_class)->unmap(widget);


}


static gboolean sc_canvas_popupmenu(GtkWidget* widget)
{


    sc_canvas_do_popup_menu(widget,NULL);

    return TRUE;
}

static gboolean sc_canvas_press(GtkWidget* widget, GdkEventButton*e)
{

    g_message("Pressed..");

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;


    if(e->button==GDK_BUTTON_SECONDARY && e->window==priv->window){
    
        sc_canvas_do_popup_menu(widget,e);
        return TRUE;
    }


    if(e->button==GDK_BUTTON_PRIMARY && e->window==priv->menuwindow){
    
        priv->menu_drag=TRUE;

        priv->menu_dragpos[X]=(int)e->x;
        priv->menu_dragpos[Y]=(int)e->y;

        return TRUE;
    }

    return TRUE;
}
static gboolean sc_canvas_release(GtkWidget* widget, GdkEventButton*e)
{
    g_message("Release..");

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

    if(e->button==GDK_BUTTON_PRIMARY && e->window==priv->menuwindow){
    
        priv->menu_drag=FALSE;
        return TRUE;
    }

    return TRUE;
}

static gboolean sc_canvas_motion(GtkWidget* widget, GdkEventMotion*e)
{

//    g_print("Motion.");
    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

    int biasx,biasy;
    int wx,wy;

    if(priv->menu_drag ){
//        g_print("==========\n");
//        g_print("menu_dragpos[X]=%d, drag-pos[Y]=%d\ne->x=%d, e->y=%d\n",
//                priv->menu_dragpos[X],priv->menu_dragpos[Y],(int)e->x,(int)e->y);
//        g_print("----------\n");
        biasx=(int)(e->x)-priv->menu_dragpos[X];//-priv->menu_position.x;
        biasy=(int)(e->y)-priv->menu_dragpos[Y];//-priv->menu_position.y;
        gdk_window_get_position(priv->menuwindow,&wx,&wy);

        priv->menu_position.x=wx+biasx;
        priv->menu_position.y=wy+biasy;

        gtk_widget_queue_resize(widget);

    }
    return TRUE;


}



static void sc_canvas_set_child(SCCanvas*canvas,GtkWidget**child,GdkWindow*childwin,GtkWidget*widget)
{

    GtkWidget*oldchild=*child;
//    g_message("SET CHILD....");

    if(oldchild){
        g_message("OldChild...%x..",oldchild);
        g_object_ref(oldchild);
        gtk_container_remove(GTK_CONTAINER(canvas),oldchild);
    }

    *child=widget;
    if(*child){
        gtk_widget_set_parent_window(widget,childwin);
        gtk_container_add(GTK_CONTAINER(canvas),*child);
    
    }
    if(oldchild){
        g_object_unref(oldchild);
    }

}



/*
static void sc_canvas_add(GtkContainer*container,GtkWidget*w)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;
   
//    gtk_widget_set_parent_window(w,priv->window);
//    GTK_CONTAINER_CLASS(sc_canvas_parent_class)->add(container,w);
  priv->operator=w;
  gtk_widget_set_parent(w,GTK_WIDGET(container));

    gtk_widget_queue_resize(GTK_WIDGET(container));

}





static void sc_canvas_remove(GtkContainer*container,GtkWidget*w)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;
 
//    GTK_CONTAINER_CLASS(sc_canvas_parent_class)->remove(container,w);

    gtk_widget_unparent(w);
    priv->operator=NULL;
//    gtk_widget_set_parent_window(w,NULL);

    gtk_widget_queue_resize(GTK_WIDGET(container));

}




static void sc_canvas_forall(GtkContainer*container,gboolean include_internal,GtkCallback callback, gpointer callback_data)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;

    if(priv->operator)
        (*callback)(priv->operator,callback_data);

    if(priv->menu)
        (*callback)(priv->menu,callback_data);

}



void sc_canvas_reset(SCCanvas*canvas,GtkWidget*w){

    //GtkWidget*child=gtk_bin_get_child(GTK_BIN(canvas));
    SCCanvasPriv*priv=canvas->priv;
    
    
    if(priv->operator){
    gtk_widget_set_parent_window(priv->operator,NULL);
    }
    
    gtk_widget_set_parent(w,GTK_WIDGET(canvas));
    gtk_widget_set_parent_window(w,priv->window);
    priv->operator=w;

    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}
*/


void sc_canvas_set_operator(SCCanvas*canvas,GtkWidget* op)
{

    SCCanvasPriv*priv=canvas->priv;

    sc_canvas_set_child(canvas,&priv->operator,priv->window,op);
    sc_canvas_set_toolmenu(canvas);

    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}

void sc_canvas_set_menu(SCCanvas*canvas,GtkWidget* me)
{
    SCCanvasPriv*priv=canvas->priv;

//    priv->menu=NULL;
//    g_message("Before set child priv->menu:%x..",priv->menu);
    sc_canvas_set_child(canvas,&priv->menu,priv->menuwindow,me);
    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}


void sc_canvas_set_toolmenu(SCCanvas*canvas)
{
    SCCanvasPriv* priv=canvas->priv;

    if(!priv->operator)
        return;
    GtkWidget*toolmenu=sc_operable_obtain_toolmenu(priv->operator);
    
    sc_canvas_set_child(canvas,&priv->toolmenu,priv->toolmenuwindow,toolmenu);
}




GtkWidget*sc_canvas_new(int x,int y,int width, int height)
{
    return (GtkWidget*)g_object_new(SC_TYPE_CANVAS,"x",x,"y",y,"width",width,"height",height,NULL);

}




void canvas_undo_cb(GtkWidget*widget,gpointer d)
{
    SCCanvas* canvas=SC_CANVAS(d);
    g_message("Undo operator..");
    sc_canvas_undo(canvas);

}


void canvas_done_cb(GtkWidget*widget,gpointer d)
{
    SCCanvas* canvas=SC_CANVAS(d);
    g_message("Copy to clipborder..");
    sc_canvas_done(canvas);

}


void canvas_save_cb(GtkWidget*widget,gpointer d)
{
    
    SCCanvas* canvas=SC_CANVAS(d);

    g_message("Save to file..");
//    sc_canvas_step_done(canvas);
    if(sc_canvas_save(canvas))
        sc_canvas_exit(canvas);

}


void canvas_exit_cb(GtkWidget*widget,gpointer d)
{
    SCCanvas* canvas=SC_CANVAS(d);

    g_message("Exit ScreenCapturer..");
    sc_canvas_exit(canvas);

}


void canvas_xmenu_act(GtkWidget* widget, gpointer d)
{

    SCCanvas* canvas=SC_CANVAS(d);
    SCCanvasPriv* priv=canvas->priv;
    if(priv->show_menu){
        sc_canvas_hide_menu(canvas);
    }else{
        sc_canvas_show_menu(canvas);
    }
    return;

}




void canvas_reselect_act(GtkWidget* widget, gpointer d)
{

    SCCanvas* canvas=SC_CANVAS(d);
    SCCanvasPriv* priv=canvas->priv;

    g_message("Reselect Region..");
    sc_window_reselect(SC_WINDOW(priv->appwin));

}

GtkWidget* sc_canvas_get_operator(SCCanvas*canvas)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;
    return priv->operator;

}

GtkWidget* sc_canvas_get_menu(SCCanvas*canvas)//,GtkWidget*menu)//SCOperator* op)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;


//    if(priv->menu)
//        return priv->menu;

//    priv->show_menu=TRUE;

    GtkWidget*menu=gtk_grid_new();

    //Add MENU ITEMS 
    priv->operable_box= gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
    

    GtkWidget*item_undo=sc_image_button_new_by_size(icon_undo,20);
    GtkWidget*item_save=sc_image_button_new_by_size(icon_save,20);
    GtkWidget*item_done=sc_image_button_new_by_size(icon_done,20);
    GtkWidget*item_exit=sc_image_button_new_by_size(icon_exit,20);

    g_signal_connect(G_OBJECT(item_undo),"clicked",G_CALLBACK(canvas_undo_cb),canvas);
    g_signal_connect(G_OBJECT(item_done),"clicked",G_CALLBACK(canvas_done_cb),canvas);
    g_signal_connect(G_OBJECT(item_save),"clicked",G_CALLBACK(canvas_save_cb),canvas);
    g_signal_connect(G_OBJECT(item_exit),"clicked",G_CALLBACK(canvas_exit_cb),canvas);

//    gtk_box_pack_start(GTK_BOX(priv->menu),priv->operable_box,TRUE,TRUE,0);

    GtkWidget*movehandle=gtk_label_new(" ");

    gtk_grid_attach(GTK_GRID(menu), movehandle,0,0,1,1);
//    gtk_grid_attach(GTK_GRID(priv->menu), priv->operable_box,0,0,1,1);
    gtk_grid_attach_next_to(GTK_GRID(menu),priv->operable_box,movehandle,GTK_POS_RIGHT,1,1);
    gtk_grid_attach_next_to(GTK_GRID(menu),item_undo,priv->operable_box,GTK_POS_RIGHT,1,1);
    gtk_grid_attach_next_to(GTK_GRID(menu),item_save,item_undo,GTK_POS_RIGHT,1,1);
    gtk_grid_attach_next_to(GTK_GRID(menu),item_done,item_save,GTK_POS_RIGHT,1,1);
    gtk_grid_attach_next_to(GTK_GRID(menu),item_exit,item_done,GTK_POS_RIGHT,1,1);

//    GdkRGBA opacity={0,};
//    gtk_widget_override_background_color(priv->menu,0,&opacity);
/*
    gtk_widget_show(priv->operable_box);
    gtk_widget_show(item_undo);
    gtk_widget_show(item_save);
    gtk_widget_show(item_done);
    gtk_widget_show(item_exit);
    gtk_widget_show_all(priv->menu);
*/
    //for propagate_draw();
//    gtk_widget_set_parent(priv->menu,GTK_WIDGET(canvas));
//    gtk_widget_set_parent_window(priv->menu,priv->menuwindow);

    //g_message("Returning Menu");
    return menu;

}



GtkWidget*sc_canvas_get_popup_menu(SCCanvas*canvas)
{
    SCCanvasPriv* priv=canvas->priv;


    GtkWidget*menu=gtk_menu_new();
    //gtk_widget_set_opacity(menu,0.2);

    GtkWidget*item_shape=gtk_menu_item_new_with_label("shape");
    GtkWidget*item_arrow=gtk_menu_item_new_with_label("arrow");
    GtkWidget*item_painter=gtk_menu_item_new_with_label("painter");
    GtkWidget*item_text=gtk_menu_item_new_with_label("text");
    
    char*xmenu_label;
    if(priv->show_menu)
        xmenu_label="hide menu";
    else
        xmenu_label="show menu";

    GtkWidget*item_xmenu=gtk_menu_item_new_with_label(xmenu_label);

    GtkWidget*item_reselect=gtk_menu_item_new_with_label("reselect");
    GtkWidget*item_sep=gtk_separator_menu_item_new();

    GtkWidget*item_undo=gtk_menu_item_new_with_label("undo");
    GtkWidget*item_done=gtk_menu_item_new_with_mnemonic("_done");
    GtkWidget*item_save=gtk_menu_item_new_with_mnemonic("_save");
    GtkWidget*item_exit=gtk_menu_item_new_with_label("exit");


    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_shape);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_arrow);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_painter);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_text);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_reselect);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_xmenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_sep);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_undo);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_done);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_save);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu),item_exit);



    g_signal_connect(G_OBJECT(item_undo),"activate",G_CALLBACK(canvas_undo_cb),canvas);
    g_signal_connect(G_OBJECT(item_save),"activate",G_CALLBACK(canvas_save_cb),canvas);
    g_signal_connect(G_OBJECT(item_exit),"activate",G_CALLBACK(canvas_exit_cb),canvas);

    g_signal_connect(G_OBJECT(item_shape),"activate",G_CALLBACK(operator_shape_act),canvas);
    g_signal_connect(G_OBJECT(item_arrow),"activate",G_CALLBACK(operator_arrow_act),canvas);
    g_signal_connect(G_OBJECT(item_painter),"activate",G_CALLBACK(operator_painter_act),canvas);
    g_signal_connect(G_OBJECT(item_text),"activate",G_CALLBACK(operator_text_act),canvas);
    g_signal_connect(G_OBJECT(item_xmenu),"activate",G_CALLBACK(canvas_xmenu_act),canvas);
    g_signal_connect(G_OBJECT(item_reselect),"activate",G_CALLBACK(canvas_reselect_act),canvas);


    return menu;

}










void sc_canvas_do_popup_menu(GtkWidget*widget, GdkEventButton* event)
{

    SCCanvas*canvas=SC_CANVAS(widget);
    SCCanvasPriv*priv=canvas->priv;

    int button,event_time;

    GtkWidget*menu=sc_canvas_get_popup_menu(canvas);
    gtk_widget_show_all(menu);

//    g_signal_connect(menu,"deactivate",G_CALLBACK(gtk_widget_destroy),NULL);

    if(event){
    
        button=event->button;
        event_time=event->time;
    
    }else{
        button=0;
        event_time=gtk_get_current_event_time();
    
    }

    gtk_menu_attach_to_widget(GTK_MENU(menu),widget,NULL);
    gtk_menu_popup(GTK_MENU(menu),NULL,NULL,NULL,NULL,button,event_time);

}










/*
static void operable_clicked(GtkWidget*widget,gpointer d)
{


    SCCanvasPriv*priv=ref_canvas->priv;

    int pos=GPOINTER_TO_INT(d);

    sc_canvas_add_op(ref_canvas,GTK_WIDGET(priv->registered_operables[pos]));

    gtk_widget_show(GTK_WIDGET(priv->registered_operables[pos]));


}

GtkWidget* sc_canvas_add_operater(SCCanvas*canvas, GtkWidget*operable,int pos)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    if(priv->menu==NULL){
        sc_canvas_get_menu(canvas);
//        g_message("priv->menu:%x,   priv->operble:%x",priv->menu,priv->operable_box);
    }
//  xxx/x* 
    if(priv->registered_operables[pos])
        g_object_unref(priv->registered_operables[pos]);
    
    priv->registered_operables[pos]=operable;

//x* /
//    g_object_ref(operable);
 //   gtk_widget_show(GTK_WIDGET(operable));

//    GtkWidget*toolbutton=SC_OPERABLE_GET_INTERFACE(operable)->toolbutton;
//    gtk_widget_show(toolbutton);

//    priv->act_buttons[pos]=toolbutton;


    ref_canvas=canvas;


    gtk_box_pack_end(GTK_BOX(priv->operable_box),toolbutton,FALSE,FALSE,5);

    g_signal_connect(G_OBJECT(toolbutton),"clicked",G_CALLBACK(operable_clicked),GINT_TO_POINTER(pos));//&info);

//    gtk_widget_queue_resize(GTK_WIDGET(canvas));
    return toolbutton;

}
*/

/*


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

    GtkWidget*arrow=sc_painter_new(canvas);

    sc_canvas_add_op(canvas,arrow);
    gtk_widget_show_all(canvas);

}



void sc_canvas_register_operables(SCCanvas*canvas)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    if(!priv->menu){
    //    sc_canvas_get_menu(canvas);
    sc_canvas_add_me(canvas,sc_canvas_get_menu(canvas));
    
    }
//    ref_canvas=canvas;
//    SCOperable*operable=sc_arrow_new();
//    GtkWidget*bt=sc_canvas_add_operater(canvas,operable,OP_ARROW);


//    operable=sc_shape_new(canvas);

//    bt=sc_canvas_add_operater(canvas,operable,OP_RECT);

    GtkWidget*shape=gtk_button_new_with_label("shape");
    gtk_box_pack_end(GTK_BOX(priv->operable_box),shape,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(shape),"clicked",G_CALLBACK(shape_clicked),canvas);//&info);
/ *
    GtkWidget*arrow=gtk_button_new_with_label("arrow");
    gtk_box_pack_end(GTK_BOX(priv->operable_box),arrow,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(arrow),"clicked",G_CALLBACK(arrow_clicked),canvas);//&info);
* /
    GtkWidget*painter=gtk_button_new_with_label("painter");
    gtk_box_pack_end(GTK_BOX(priv->operable_box),painter,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(painter),"clicked",G_CALLBACK(painter_clicked),canvas);//&info);


}


*/


void sc_canvas_destroy(SCCanvas *canvas)
{
    g_object_unref(G_OBJECT(canvas));

}


void sc_canvas_set(SCCanvas*canvas,int x, int y, int width, int height)
{

g_object_set(G_OBJECT(canvas),"x",x,"y",y,"width",width,"height",height,NULL);


}

void sc_canvas_set_appwin(SCCanvas*canvas,GtkWidget*appwin)
{
    g_assert(GTK_IS_APPLICATION_WINDOW(appwin));

    SCCanvasPriv*priv=canvas->priv;

    priv->appwin=appwin;

}

void sc_canvas_get(SCCanvas*canvas,int* x, int* y, int* width, int* height)
{

g_object_get(G_OBJECT(canvas),"x",x,"y",y,"width",width,"height",height,NULL);


}



void sc_canvas_set_pixbuf(SCCanvas*canvas ,GdkPixbuf*pf)
{
g_object_set(G_OBJECT(canvas),"pixbuf",pf,NULL);


}


GdkPixbuf* sc_canvas_get_pixbuf(SCCanvas*canvas)
{
    GdkPixbuf* pf;
g_object_get(G_OBJECT(canvas),"pixbuf",&pf,NULL);

return pf;

}


void sc_canvas_step_done(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;

    GdkPixbuf*pf=gdk_pixbuf_get_from_window(priv->window,0,0,
            priv->position.width, priv->position.height);

    if(pf){
        sc_canvas_set_pixbuf(canvas,pf);
        g_object_unref(pf);
    }

    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}


void sc_canvas_hide_menu(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;
    priv->show_menu=FALSE;
    gdk_window_hide(priv->menuwindow);
    gdk_window_hide(priv->toolmenuwindow);
    gtk_widget_queue_resize(GTK_WIDGET(canvas));
}


void sc_canvas_show_menu(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;
    priv->show_menu=TRUE;
    gdk_window_show(priv->menuwindow);
    gdk_window_show(priv->toolmenuwindow);
    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}

gboolean sc_canvas_menu_is_show(SCCanvas* canvas)
{
return canvas->priv->show_menu;
}



void sc_canvas_undo(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;

    /*Special Case:: SCText..*/

    if(SC_IS_TEXT(priv->operator)&&
        float_border_count(FLOAT_BORDER(priv->operator))!=0 ){
        sc_text_remove(SC_TEXT(priv->operator));
    
        gtk_widget_queue_resize(GTK_WIDGET(canvas));
        return;
    }



    GList*llink=priv->pixbufs;
    //at least one pixbuf rerquired for saving.
    if(llink==NULL || g_list_length(llink)<=1)
        return;
    priv->pixbufs=g_list_remove_link(priv->pixbufs,llink);

    //free pixbuf;
    g_object_unref(llink->data);
    g_list_free(llink);


    gtk_widget_queue_resize(GTK_WIDGET(canvas));


}

void sc_canvas_done(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;
//TODO 
//save to clipborder


}

gboolean sc_canvas_save(SCCanvas* canvas)
{
    SCCanvasPriv*priv=canvas->priv;

    extern time_t Timeval;
    struct tm* tmm=localtime(&Timeval);
    char timespec[BUF_SIZE];
    gboolean rtval;

    strftime(timespec,BUF_SIZE,"%Y%m%d%H%M%S",tmm);
    
    sc_canvas_hide_menu(canvas);

    GtkWidget*dialog=gtk_file_chooser_dialog_new("Save Picture",GTK_WINDOW(priv->appwin),GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Save",GTK_RESPONSE_ACCEPT,"_Cancel",GTK_RESPONSE_CANCEL,NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),TRUE);

    char*filename=g_strdup_printf("screencaputer-%s.png",timespec);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),filename);
    g_free(filename);

    char*savfilename;

    int res=gtk_dialog_run(GTK_DIALOG(dialog));

    rtval=FALSE;
//    g_message("RESPONSE %d..",res);
    if(res==GTK_RESPONSE_ACCEPT){
    
        savfilename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_message("Filename::%s",savfilename);

        GdkPixbuf*savepf=sc_canvas_get_pixbuf(canvas);
        gdk_pixbuf_save(savepf,savfilename,"png",NULL,NULL);

        g_free(savfilename);
        g_object_unref(savepf);

    rtval=TRUE;

    }else{
        
    sc_canvas_show_menu(canvas);
    
    }

    gtk_widget_destroy(dialog);


    return rtval;

}

void sc_canvas_exit(SCCanvas* canvas)
{
    SCCanvasPriv*priv=canvas->priv;

    GtkApplication*app=gtk_window_get_application(GTK_WINDOW(priv->appwin));
    g_application_quit(G_APPLICATION(app));

}



GtkWidget*sc_canvas_get_operable_box(SCCanvas*canvas)
{
    return canvas->priv->operable_box;

}



