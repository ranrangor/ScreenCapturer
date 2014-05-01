#include<gtk/gtk.h>
#include"sc-canvas.h"




enum{

    PROP_0,
    PROP_X,
    PROP_Y,
    PROP_WIDTH,
    PROP_HEIGHT

};






struct _SCCanvasPriv{


    GList* pixbufs;
    gint current_pixbuf;
    GtkAllocation position;
    
    GtkWidget*operator;
    GdkWindow*window;

    GtkWidget*toolbar;//GtkBox
    GdkWindow*tool_window;


};



static void sc_canvas_set_property(GObject*obj,guint prop_id,const GValue*value,GParamSpec*pspec);
static void sc_canvas_get_property(GObject*obj,guint prop_id,GValue*value,GParamSpec*pspec);

static void sc_canvas_realize(GtkWidget*widget);
static void sc_canvas_unrealize(GtkWidget*widget);
static void sc_canvas_size_allocate(GtkWidget*widget,GtkAllocation*allocation);
static void sc_canvas_get_preferred_width(GtkWidget*widget,gint *min,gint *nat);
static void sc_canvas_get_preferred_height(GtkWidget*widget,gint *min,gint *nat);

static void sc_canvas_map(GtkWidget*widget);
static void sc_canvas_unmap(GtkWidget*widget);


static void sc_canvas_add(GtkContainer*container,GtkWidget*w);
static void sc_canvas_remove(GtkContainer*container,GtkWidget*w);
//static void sc_canvas_forall(GtkContainer*container,gboolean include_internal,GtkCallback callback, gpointer callback_data);

static gboolean sc_canvas_draw(GtkWidget* widget,cairo_t*cr);

void print_allocation(GtkAllocation * allo)
{

    g_print("     Allocation::\n");
    g_print("x    :%5d, y     :%5d\n", allo->x, allo->y);
    g_print("width:%5d, height:%5d\n", allo->width, allo->height);



}




G_DEFINE_TYPE(SCCanvas,sc_canvas,GTK_TYPE_BIN)



static void sc_canvas_class_init(SCCanvasClass* scklass)
{

    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(scklass);

    GObjectClass*oclass=G_OBJECT_CLASS(scklass);

    GtkContainerClass*cclass=GTK_CONTAINER_CLASS(scklass);



    oclass->set_property=sc_canvas_set_property;
    oclass->get_property=sc_canvas_get_property;

    
    wclass->realize=sc_canvas_realize;
    wclass->unrealize=sc_canvas_unrealize;
//    wclass->get_preferred_width=sc_canvas_get_preferred_width;
//    wclass->get_preferred_height=sc_canvas_get_preferred_height;
    wclass->map=sc_canvas_map;
    wclass->unmap=sc_canvas_unmap;
    wclass->size_allocate=sc_canvas_size_allocate;
    wclass->draw=sc_canvas_draw;

    cclass->add=sc_canvas_add;
    cclass->remove=sc_canvas_remove;
 //   cclass->forall=sc_canvas_forall;

    g_object_class_install_property(oclass,PROP_X,
            g_param_spec_int("x","X","Position.X",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_Y,
            g_param_spec_int("y","Y","Position.Y",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_WIDTH,
            g_param_spec_int("width","Width","Position.width",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_HEIGHT,
            g_param_spec_int("height","Height","Position.height",0,G_MAXINT,0,G_PARAM_READWRITE));





    g_type_class_add_private(scklass,sizeof(SCCanvasPriv));

}





static void sc_canvas_init(SCCanvas* scobj)
{
    
    SCCanvasPriv*priv=SC_CANVAS_GET_PRIV(scobj);

    scobj->priv=priv;

    GtkWidget*widget=GTK_WIDGET(scobj);

    gtk_widget_set_has_window(widget,FALSE);


    priv->pixbufs=NULL;
    priv->current_pixbuf=0;
    priv->operator=NULL;
    priv->toolbar=NULL;





}



static void sc_canvas_set_property(GObject*obj,guint prop_id,const GValue*value,GParamSpec*pspec)
{

    SCCanvas*canvas=SC_CANVAS(obj);
    SCCanvasPriv*priv=canvas->priv;

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

        default:

            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj,prop_id,pspec);

    }

    gtk_widget_queue_resize(GTK_WIDGET(obj));



}


static void sc_canvas_get_property(GObject*obj,guint prop_id,GValue*value,GParamSpec*pspec)
{


    SCCanvas*canvas=SC_CANVAS(obj);

    SCCanvasPriv*priv=canvas->priv;



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

        default:

            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj,prop_id,pspec);

    
    }







}




static void set_parent_win(GtkWidget*w,gpointer d)
{

    SCCanvasPriv*priv=SC_CANVAS(d)->priv;
    gtk_widget_set_parent_window(w,priv->window);


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

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

    gtk_widget_get_allocation(widget,&allocation);


    attributes.window_type=GDK_WINDOW_CHILD;

        attributes.x=priv->position.x;
        attributes.y=priv->position.y;
        attributes.width=priv->position.width;
        attributes.height=priv->position.height;
    
//print_allocation(&allocation);

    
    attributes.wclass=GDK_INPUT_OUTPUT;
    attributes.visual=gtk_widget_get_visual(widget);
    attributes.event_mask=gtk_widget_get_events(widget)|
        GDK_EXPOSURE_MASK|
        GDK_BUTTON_PRESS_MASK|
        GDK_BUTTON_RELEASE_MASK|
        GDK_POINTER_MOTION_MASK;

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_FLEUR);

    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL|GDK_WA_CURSOR;

    parent_window=gtk_widget_get_parent_window(widget);

    window=gdk_window_new(parent_window,&attributes,attributes_mask);


    gtk_widget_register_window(widget,window);
    priv->window=window;

    g_object_unref(attributes.cursor);

//    gtk_style_context_set_background (gtk_widget_get_style_context (widget), window);
    

    gtk_container_forall(GTK_CONTAINER(widget),set_parent_win,widget);

/* ToolBar window */

        attributes.x=10;
        attributes.y=10;
        attributes.width=1;
        attributes.height=1;

    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL;

    priv->tool_window=gdk_window_new(parent_window,&attributes,attributes_mask);
    gtk_widget_register_window(widget,priv->tool_window);

    if(priv->toolbar)
        gtk_widget_set_parent_window(priv->toolbar,priv->tool_window);





}



static void sc_canvas_unrealize(GtkWidget*widget)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;
    gtk_widget_set_realized(widget,FALSE);


    gtk_widget_unregister_window(widget,priv->window);
    gdk_window_destroy(priv->window);
    priv->window=NULL;


    gtk_widget_unregister_window(widget,priv->tool_window);
    gdk_window_destroy(priv->tool_window);
    priv->tool_window=NULL;



    GTK_WIDGET_CLASS(sc_canvas_parent_class)->unrealize(widget);


}








static void allocate_child(GtkWidget*w,gpointer d)
{
    g_message("Size Allocated..Child");

    SCCanvas* canvas=SC_CANVAS(d);
    SCCanvasPriv* priv=SC_CANVAS(d)->priv;

    GtkAllocation alloc;
    gtk_widget_get_allocation(GTK_WIDGET(canvas), &alloc);

print_allocation(&priv->position);


//    if(gtk_widget_get_visible(w)){
    
        if(gtk_widget_get_realized(w)){

            g_message("GDK Window Move& Resize");
            gdk_window_move_resize(priv->window,
                    alloc.x+priv->position.x,
                    alloc.y+priv->position.y,
                    priv->position.width,
                    priv->position.height);

        }


        GtkAllocation child_allocation;

        child_allocation.x=child_allocation.y=0;
        child_allocation.width= priv->position.width;
        child_allocation.height= priv->position.height;
   
        GdkWindow* pw=gtk_widget_get_window(w);
        g_message("child's GdkWindow: %x",pw);
        gtk_widget_size_allocate(w,&child_allocation);


    
//    }

}



static void sc_canvas_size_allocate(GtkWidget*widget,GtkAllocation*allocation)
{

    SCCanvasPriv* priv=SC_CANVAS(widget)->priv;
    
    gtk_widget_set_allocation(widget,allocation);

    gtk_container_forall(GTK_CONTAINER(widget), allocate_child,widget);


    GtkRequisition toolbarR;
    GtkAllocation toolbarA;


    if(priv->toolbar)
    {
        g_message("Allocate Toolbar_window");
    gtk_widget_get_preferred_size(priv->toolbar,&toolbarR,NULL);

             gdk_window_move_resize(priv->tool_window,
                    allocation->x+10,//+priv->position.x,
                    allocation->y+10,//+priv->position.y,
                    toolbarR.width,
                    toolbarR.height);
    
    
    toolbarA.x=toolbarA.y=0;
    toolbarA.width=toolbarR.width;
    toolbarA.height=toolbarR.height;

    gtk_widget_size_allocate(priv->toolbar,&toolbarA);
    }

}

/*

static void map_child(GtkWidget*w,gpointer d)
{
    SCCanvasPriv*priv=SC_CANVAS(d)->priv;

    if(gtk_widget_is_visible(w)){
        g_print("to Map...\n");
        gdk_window_show(priv->window);
    }
}



static void unmap_child(GtkWidget*w,gpointer d)
{

    SCCanvasPriv*priv=SC_CANVAS(d)->priv;

    if(gdk_window_is_visible(priv->window)){
        gdk_window_hide(priv->window);
    }
}

*/



static gboolean sc_canvas_draw(GtkWidget* widget,cairo_t*cr)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;


    gtk_container_propagate_draw(GTK_CONTAINER(widget),priv->toolbar,cr);
    GList*child= gtk_container_get_children(GTK_CONTAINER(widget));

    gtk_container_propagate_draw(GTK_CONTAINER(widget),GTK_WIDGET(child->data),cr);

    g_list_free(child);

//    GTK_WIDGET_GET_CLASS(widget)->draw(widget,cr);

    return FALSE;

}



static void sc_canvas_map(GtkWidget*widget)
{

    g_message("Mapping...");
    GTK_WIDGET_CLASS(sc_canvas_parent_class)->map(widget);


    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

//    gtk_container_forall(GTK_CONTAINER(widget),map_child,widget);

    if(priv->window)
        gdk_window_show(priv->window);

    if(priv->tool_window)
        gdk_window_show(priv->tool_window);


}


static void sc_canvas_unmap(GtkWidget*widget)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

//    gtk_container_forall(GTK_CONTAINER(widget),unmap_child,widget);

    if(priv->window)
        gdk_window_hide(priv->window);

    if(priv->tool_window)
        gdk_window_hide(priv->tool_window);


    GTK_WIDGET_CLASS(sc_canvas_parent_class)->unmap(widget);


}







static void sc_canvas_add(GtkContainer*container,GtkWidget*w)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;
   
    gtk_widget_set_parent_window(w,priv->window);
    GTK_CONTAINER_CLASS(sc_canvas_parent_class)->add(container,w);
    
    gtk_widget_queue_resize(GTK_WIDGET(container));

}





static void sc_canvas_remove(GtkContainer*container,GtkWidget*w)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;
 
    GTK_CONTAINER_CLASS(sc_canvas_parent_class)->remove(container,w);

    gtk_widget_set_parent_window(w,NULL);

}



GtkWidget*sc_canvas_new(int x,int y,int width, int height)
{

    return (GtkWidget*)g_object_new(SC_TYPE_CANVAS,"x",x,"y",y,"width",width,"height",height,NULL);

}


void
comput_position(GtkMenu *menu,
gint *x,
gint *y,
gboolean *push_in,
gpointer user_data)
{


    SCCanvasPriv*priv=SC_CANVAS(user_data)->priv;

    *x=priv->position.x+priv->position.width;
    *y=priv->position.y+priv->position.height;
    *push_in=TRUE;



}

void sc_canvas_add_menu(SCCanvas*canvas)//,GtkWidget*menu)//SCOperator* op)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    priv->toolbar=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,3);
//    g_signal_connect (priv->toolbar, "deactivate",G_CALLBACK (gtk_widget_destroy), NULL);

    /* Add MENU ITEMS */
    GtkWidget*item_undo= gtk_button_new_with_label("Undo");
    GtkWidget*item_save= gtk_button_new_with_label("Save");
    GtkWidget*item_done= gtk_button_new_with_label("Done");
    GtkWidget*item_exit= gtk_button_new_with_label("Exit");
//    GtkWidget*item_sep= gtk_separator_menu_item_new();

//    gtk_menu_attach(GTK_MENU(priv->toolbar),item_sep,1,1,1,1);


    gtk_box_pack_start(GTK_BOX(priv->toolbar),item_undo,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(priv->toolbar),item_save,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(priv->toolbar),item_done,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(priv->toolbar),item_exit,FALSE,FALSE,0);

    gtk_widget_show(item_undo);
    gtk_widget_show(item_save);
    gtk_widget_show(item_done);
    gtk_widget_show(item_exit);

    gtk_widget_show_all(priv->toolbar);
//    gtk_menu_attach_to_widget (GTK_MENU (priv->toolbar), GTK_WIDGET(canvas), NULL);
//    gtk_menu_popup (GTK_MENU (priv->toolbar), NULL, NULL, comput_position, canvas, 0, GDK_CURRENT_TIME);

    gtk_widget_set_parent_window(priv->toolbar,priv->tool_window);
    gtk_widget_set_parent(priv->toolbar,GTK_WIDGET(canvas));


}





void sc_canvas_destroy(SCCanvas *canvas)
{
    g_object_unref(G_OBJECT(canvas));

}


void sc_canvas_set(SCCanvas*canvas,int x, int y, int width, int height)
{

g_object_set(G_OBJECT(canvas),"x",x,"y",y,"width",width,"height",height,NULL);



}


void sc_canvas_get(SCCanvas*canvas,int* x, int* y, int* width, int* height)
{

g_object_get(G_OBJECT(canvas),"x",x,"y",y,"width",width,"height",height,NULL);



}




