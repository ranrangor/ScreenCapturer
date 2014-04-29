
#include"sc-window.h"
#include"sc-app.h"
#include<gdk-pixbuf/gdk-pixbuf.h>


#define VIEW_Width 120
#define VIEW_Height 80
#define VIEW_Detail 30
#define SCALE 5
#define VIEW_bias 10






struct _SCWindowPriv{

    GdkPixbuf*fullpf;
    GdkWindow*root_win;
    //list of windows
    GList*rects;
    //number of windows;
    int n_rects;
    int px,py;
    GdkRectangle current_rect;
    GdkRectangle saved_rect;
    gint rect_selected:1;
    gint button_down:1;
    gint moved :1;


};






static gboolean sc_window_motion(GtkWidget*widget,GdkEventMotion*e);
static gboolean sc_window_button_press(GtkWidget*widget,GdkEventButton*e);
static gboolean sc_window_button_release(GtkWidget*widget,GdkEventButton*e);

static gboolean sc_window_draw(GtkWidget*widget,cairo_t*cr);


static gboolean point_in_rect(GdkRectangle*rect,int x,int y)
{
    
        
        if(x>rect->x && x<rect->x+rect->width &&
                y>rect->y && y<rect->y+rect->height )
        {
            return TRUE;

        }
    return FALSE;

}



void sc_window_setup(SCWindow*scwin);


G_DEFINE_TYPE(SCWindow,sc_window,GTK_TYPE_APPLICATION_WINDOW)



static void sc_window_class_init(SCWindowClass* klass)
{

    GtkWidgetClass* wklass=GTK_WIDGET_CLASS(klass);

    wklass->draw=sc_window_draw;
    wklass->motion_notify_event=sc_window_motion;
    wklass->button_press_event=sc_window_button_press;
    wklass->button_release_event=sc_window_button_release;




    g_type_class_add_private(klass,sizeof(SCWindowPriv));


}



static void sc_window_init(SCWindow* scwin)
{

//    SCWindow*scwin=SC_WINDOW(object);

    SCWindowPriv*priv=SC_WINDOW_GET_PRIV(scwin);

    gtk_widget_set_has_window(GTK_WIDGET(scwin),TRUE);
    scwin->priv=priv;

    priv->n_rects=0;
    priv->rects=NULL;


    sc_window_setup(scwin);


}

/*
static gboolean rect_selected(SCWindow*scwin)
{


}


*/



static void get_rect_under_pointer(SCWindow*scwin,int x,int y)
{

    g_message("Get Rectangle under pointer");
    SCWindowPriv*priv=SC_WINDOW(scwin)->priv;

    GList*rectlst=priv->rects;

    while(rectlst){
    
        GdkRectangle* rect=rectlst->data;
       /* 
        if(x>rect->x && x<rect->x+rect->width &&
                y>rect->y && y<rect->y+rect->height )
        */
                
        if(point_in_rect(rect,x,y))
        {
            priv->current_rect.x=rect->x;
            priv->current_rect.y=rect->y;
            priv->current_rect.width=rect->width;
            priv->current_rect.height=rect->height;
            break;

        }
        
        
        rectlst=rectlst->next; 
    }

}





static void get_all_rects(SCWindow* scwin)
{

    g_message("Get all Rectangle");

    GtkWidget*win=GTK_WIDGET(scwin);
    SCWindowPriv*priv=scwin->priv;

    GList*windows=gdk_screen_get_window_stack(gtk_widget_get_screen(win));
//    GList*windows=gdk_screen_get_toplevel_windows(gtk_widget_get_screen(win));

//    GdkWindow* root=gdk_screen_get_root_window(gtk_widget_get_screen(win));
    GdkWindow*root=priv->root_win;
    GdkRectangle*rootrect=g_slice_new0(GdkRectangle);

    rootrect->x=rootrect->y=0;
    rootrect->width=gdk_window_get_width(root);
    rootrect->height=gdk_window_get_height(root);

    priv->rects= g_list_prepend(priv->rects,rootrect);
    priv->n_rects+=1;


    GList*lst=windows;

    GdkWindow*window;

    int i=0;
    while(lst){
    
        window=lst->data;
        int x,y;
        GdkRectangle*rect=g_slice_new0(GdkRectangle);
        GdkRectangle*rectw=g_slice_new0(GdkRectangle);


        gdk_window_get_origin(window,&rectw->x,&rectw->y);
        rectw->width=gdk_window_get_width(window);
        rectw->height=gdk_window_get_height(window);
        

        gdk_window_get_frame_extents(window,rect);

        priv->rects= g_list_prepend(priv->rects,rect);
        priv->rects= g_list_prepend(priv->rects,rectw);

        priv->n_rects+=2;
       

        lst=lst->next;
    }
    g_list_free_full(windows,(GDestroyNotify)g_object_unref);

    g_message("All: %d windows..",priv->n_rects);

}



















static gboolean sc_window_motion(GtkWidget*widget,GdkEventMotion*e)
{
    g_message("Motion.....");

    SCWindow *scwin=SC_WINDOW(widget);
    SCWindowPriv*priv=scwin->priv;

    priv->px=(int)e->x;
    priv->py=(int)e->y;

    if(priv->button_down){
//        priv->rect_selected=TRUE;
        priv->current_rect.x=priv->saved_rect.x;
        priv->current_rect.y=priv->saved_rect.y;

        priv->current_rect.width=(int)e->x-priv->current_rect.x;
        priv->current_rect.height=(int)e->y-priv->current_rect.y;


    }else if(!priv->rect_selected){
        get_rect_under_pointer(scwin,(int)e->x,(int)e->y);

    }else{
    //do nothing.
    }


    gtk_widget_queue_draw(widget);

    return TRUE;

}


static gboolean sc_window_button_press(GtkWidget*widget,GdkEventButton*e)
{

    g_message("Button Press");

    SCWindowPriv*priv=SC_WINDOW(widget)->priv;



    if(e->type==GDK_BUTTON_PRESS && e->button==GDK_BUTTON_PRIMARY){
    
        priv->button_down=TRUE;
        priv->saved_rect.x=(int)e->x;
        priv->saved_rect.y=(int)e->y;
    
    }
    
    return TRUE;

}

static gboolean sc_window_button_release(GtkWidget*widget,GdkEventButton*e)
{

    g_message("Button Release");

    SCWindowPriv*priv=SC_WINDOW(widget)->priv;


    if(e->type==GDK_BUTTON_RELEASE && e->button==GDK_BUTTON_PRIMARY){
        
        priv->button_down=FALSE;
        priv->rect_selected=TRUE;

    }

    if(e->type==GDK_BUTTON_RELEASE && e->button==GDK_BUTTON_SECONDARY){
    
        if(priv->rect_selected && !point_in_rect(&priv->current_rect,(int)e->x,(int)e->y)){
            priv->rect_selected=FALSE;
        
        }else
        if(!priv->rect_selected){
           //EEXIT         
           g_message("EXIT....");
//            gtk_main_quit();        
            exit(0);
        }

    }

    return TRUE;

}




static gboolean sc_window_draw(GtkWidget*widget,cairo_t*cr)
{

    SCWindow*scwin=SC_WINDOW(widget);
    SCWindowPriv*priv=scwin->priv;



    g_message("window draw()");

  
    GdkRectangle*cur_rect=&priv->current_rect;

  
    gdk_cairo_set_source_pixbuf(cr,priv->fullpf,0,0);
    cairo_paint(cr);

    cairo_save(cr);

    GdkWindow*root=priv->root_win;

    cairo_rectangle(cr,0,0,
            gdk_window_get_width(root),
            gdk_window_get_height(root)
            );


//    get_area(&Rects,px,py,&rectangle);
    cairo_rectangle(cr,cur_rect->x,cur_rect->y,cur_rect->width,cur_rect->height);

    cairo_set_fill_rule(cr,CAIRO_FILL_RULE_EVEN_ODD);

    

    cairo_clip(cr);
//    cairo_clip_preserve(cr);


    //shadow unselected area
    cairo_set_source_rgba(cr,0.1,0.1,0.1,0.5);
    cairo_paint(cr);

    cairo_restore(cr);

    // Current rectangle's Decorator
    cairo_set_source_rgba(cr,0,0.9,0.0,0.9);
    cairo_set_line_width(cr,SCALE);
    cairo_rectangle(cr,cur_rect->x,cur_rect->y,cur_rect->width,cur_rect->height);
    cairo_stroke(cr);


    cairo_set_source_rgba(cr,0,0.3,0.9,0.8);
    cairo_set_line_width(cr,SCALE);



    GdkPixbuf *pfview=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,VIEW_Width,VIEW_Height);

    gdk_pixbuf_scale(priv->fullpf,pfview,0,0,VIEW_Width,VIEW_Height,
            -priv->px*SCALE+VIEW_Width/2, -priv->py*SCALE+VIEW_Height/2,
            SCALE,SCALE,GDK_INTERP_TILES);

    
    cairo_save(cr);





/* Draw Preview Area*/

    cairo_rectangle(cr,priv->px+VIEW_bias,priv->py+VIEW_bias,VIEW_Width,VIEW_Height);


    cairo_set_source_rgba(cr,0,0,0,0.8);
    cairo_set_line_width(cr,1);

    cairo_stroke_preserve(cr);
    gdk_cairo_set_source_pixbuf(cr,pfview,priv->px+VIEW_bias,priv->py+VIEW_bias);
    cairo_fill(cr);


    cairo_set_source_rgba(cr,0,1,1,0.4);
    cairo_set_line_width(cr,SCALE);
    cairo_move_to(cr,priv->px+VIEW_bias+VIEW_Width/2,priv->py+VIEW_bias);
    cairo_line_to(cr,priv->px+VIEW_bias+VIEW_Width/2,priv->py+VIEW_bias+VIEW_Height);
    cairo_stroke(cr);

    cairo_move_to(cr,priv->px+VIEW_bias, priv->py+VIEW_bias+VIEW_Height/2);
    cairo_line_to(cr,priv->px+VIEW_bias+VIEW_Width,priv->py+VIEW_bias+VIEW_Height/2);
    cairo_stroke(cr);

/* Draw Detail Area*/
    //draw frame
    cairo_rectangle(cr,priv->px+VIEW_bias,priv->py+VIEW_bias+VIEW_Height,VIEW_Width,VIEW_Detail);
    cairo_set_source_rgba(cr,0,0,0,0.8);
    cairo_set_line_width(cr,1);
//    cairo_stroke_preserve(cr);

    cairo_set_source_rgba(cr,0,0,0,0.7);
    cairo_fill(cr);
    //draw desc
    cairo_select_font_face(cr,"Sans",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr,10);
    cairo_set_source_rgb(cr,1,0,0);
    gchar*desc=g_strdup_printf(" %dx%d ",cur_rect->width,cur_rect->height);
    cairo_move_to(cr,priv->px+VIEW_bias, priv->py+VIEW_bias+VIEW_Height+10);
    cairo_show_text(cr,desc);
    g_free(desc);





    cairo_restore(cr);




    
    return FALSE;

}






void sc_window_setup(SCWindow*scwin)
{

    g_message("SCWindow Setup...");

    SCWindowPriv*priv=scwin->priv;

    GtkWindow*win=GTK_WINDOW(scwin);
    GtkWidget*widget=GTK_WIDGET(scwin);

    //gtk_window_maximize(win);
    gtk_window_fullscreen(win);
    gtk_window_set_keep_above(win,TRUE);

    gtk_widget_add_events(widget,GDK_POINTER_MOTION_MASK|GDK_BUTTON_RELEASE_MASK);


//    GdkDisplay*display=gdk_display_get_default();
//    GdkScreen*screen=gdk_display_get_screen(display);

    priv->root_win=gdk_screen_get_root_window(gtk_widget_get_screen(widget));

   priv->fullpf=gdk_pixbuf_get_from_window(priv->root_win,0,0,
           gdk_window_get_width(priv->root_win),
           gdk_window_get_height(priv->root_win));

    get_all_rects(scwin);

    GdkRGBA color={0,0,0,0};
    gtk_widget_override_background_color(widget,0,&color);


    g_message(" ENd SCWindow Setup...");


}


SCWindow* sc_window_new( SCApp*app)
{
    return g_object_new(SC_TYPE_WINDOW,"application",app,NULL);


}




