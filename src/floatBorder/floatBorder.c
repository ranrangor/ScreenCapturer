
#include"floatBorder.h"
#include<stdarg.h>




#define HWND_THICKNESS 4


enum{
    CHILD_PROP_0,
    CHILD_PROP_X,
    CHILD_PROP_Y,

    CHILD_PROP_HANDLE_MOVE,//TOP_LEFT,
    CHILD_PROP_HANDLE_RIGHT,
    CHILD_PROP_HANDLE_LEFT,
    CHILD_PROP_HANDLE_BOTTOM,
    CHILD_PROP_HANDLE_TOP,

    CHILD_PROP_HANDLE_BR_RESIZE,//BOTTOM_RIGHT,
    CHILD_PROP_HANDLE_BL_RESIZE,
    CHILD_PROP_HANDLE_TR_RESIZE,
    CHILD_PROP_HANDLE_TL_RESIZE,

    CHILD_PROP_ACTIVE,
    CHILD_PROP_SHRINK,
    CHILD_PROP_OVERLAP,

    N_CHILD_PROPS

};


enum{
X,
Y,
XY
};

enum{
PROP_0,
PROP_FROZEN,
PROP_MAY_OVERLAP,
PROP_MAY_SHRINK,
N_PROPS

};


enum{
REF_CORNER,
//REF_CORNER,
//BL_CORNER,
//TR_CORNER,
//TL_CORNER,
//MOVE_CORNER
};



typedef struct floatBorderChild FloatBorderChild;


struct floatBorderChild{
    FloatBorder*floatborder;

    GtkWidget*widget;
    GdkWindow*hwnds[N_HWNDS];

    gint x;
    gint y;

    GtkRequisition min_siz;
    GtkRequisition pref_siz;
    gboolean hwnded[N_HWNDS];
    GdkRectangle position[XY];
    int drag_position[XY];

    guint active:1;
    guint overlap:1;
    guint shrink:1;

    guint in_drag:1;


};




struct floatBorderPriv{

GList *children;
guint may_overlap:1;
guint may_shrink:1;
guint frozen:1;
guint show_border:1;

gboolean hwnd_mask[N_HWNDS];

guint is_dragging:1;

GHashTable* widget2child;
GHashTable* window2child;

};



/* GObject */
static void float_border_set_property(GObject*object,guint prop_id,const GValue*value,GParamSpec*psepc);
static void float_border_get_property(GObject*object,guint prop_id, GValue*value,GParamSpec*psepc);
static void float_border_finalize(GObject*object);


static void fb_set_frozen(FloatBorder*fb,gboolean set);
static void fb_set_may_shrink(FloatBorder*fb,gboolean set);

/* GtkWidget */

static void float_border_get_preferred_width(GtkWidget* widget,gint *minimum,gint *natural);
static void float_border_get_preferred_height(GtkWidget* widget,gint *minimum,gint *natural);



static void float_border_realize(GtkWidget*widget);
static void float_border_unrealize(GtkWidget*widget);

static void float_border_map(GtkWidget*widget);
static void float_border_unmap(GtkWidget*widget);


static void float_border_size_allocate(GtkWidget*widget, GtkAllocation *allocation);

static gboolean float_border_draw(GtkWidget*widget,cairo_t*cr);

static gboolean float_border_button_press(GtkWidget*widget,GdkEventButton*e);
static gboolean float_border_button_release(GtkWidget*widget,GdkEventButton*e);
static gboolean float_border_motion_notify(GtkWidget*widget,GdkEventMotion*e);


static void dispose_hwnds(FloatBorderChild*fbchild);




/* GtkConainer */
static GType float_border_child_type(GtkContainer*container);

static void _float_border_add(GtkContainer*container,GtkWidget*widget);
static void _float_border_remove(GtkContainer*container,GtkWidget*widget);
static void _float_border_forall(GtkContainer*container, gboolean include_internals,GtkCallback callback,gpointer callback_data);

static void float_border_set_child_property(GtkContainer*container,GtkWidget*widget,
        guint prop_id,const GValue*value,GParamSpec*pspec);
static void float_border_get_child_property(GtkContainer*container,GtkWidget*widget,
        guint prop_id, GValue*value,GParamSpec*pspec);








/**/
G_DEFINE_TYPE(FloatBorder,float_border,GTK_TYPE_CONTAINER);




static void float_border_class_init(FloatBorderClass *klass)
{

    GObjectClass*oclass=G_OBJECT_CLASS(klass);
    GtkWidgetClass*wclass= GTK_WIDGET_CLASS(klass);
    GtkContainerClass*cclass= GTK_CONTAINER_CLASS(klass);

    oclass->set_property=float_border_set_property;
    oclass->get_property=float_border_get_property;
    oclass->finalize=float_border_finalize;

    wclass->draw=float_border_draw;
    wclass->size_allocate=float_border_size_allocate;
    wclass->get_preferred_width=float_border_get_preferred_width;
    wclass->get_preferred_height=float_border_get_preferred_height;
    wclass->realize=float_border_realize;
    wclass->unrealize=float_border_unrealize;
    wclass->map=float_border_map;
    wclass->unmap=float_border_unmap;

    wclass->button_press_event=float_border_button_press;
    wclass->button_release_event=float_border_button_release;
    wclass->motion_notify_event=float_border_motion_notify;


    cclass->add=_float_border_add;
    cclass->remove=_float_border_remove;
    cclass->forall=_float_border_forall;
    cclass->child_type=float_border_child_type;
//    cclass->set_child_property=float_border_set_child_property;
//    cclass->get_child_property=float_border_get_child_property;

    gtk_container_class_handle_border_width(cclass);



    g_object_class_install_property(oclass,PROP_FROZEN,
            g_param_spec_boolean("frozen","Frozen","Frozen all widgets within FloatBorder,enfore them can not resize",
                FALSE,G_PARAM_READWRITE));

    /*TODO unimplemented */
    g_object_class_install_property(oclass,PROP_MAY_OVERLAP,
            g_param_spec_boolean("may-overlap","May Overlap","ensure whether a widget can be overlapped by another widget within FloatBorder",
                FALSE,G_PARAM_READWRITE));

    g_object_class_install_property(oclass,PROP_MAY_SHRINK,
            g_param_spec_boolean("may-shrink","May Shrink","ensure whether a widget can be shrinked within FloatBorder",
                FALSE,G_PARAM_READWRITE));


/*

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_X,
                                  g_param_spec_int ("x","X","x-axis position of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_Y,
                                  g_param_spec_int("y","Y","y-axis position of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));
gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_WIDTH,
                                  g_param_spec_int ("width","Width","width of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));
gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HEIGHT,
                                  g_param_spec_int ("height","Height","height of current widget.",
                                  0,G_MAXINT,0,
                                  G_PARAM_READWRITE));


gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_ACTIVE,
                                  g_param_spec_boolean ("active","Active","Whether a widget has any resize handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));


gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_OVERLAP,
                                  g_param_spec_boolean ("overlap","Canbe Overlapped","Whether current widget can be overlapped by another.",
                                  TRUE,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_SHRINK,
                                  g_param_spec_boolean ("shrink","Canbe shrinked","Whether current widget can be shrinked by another.",
                                  FALSE,
                                  G_PARAM_READWRITE));



gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_MOVE,
                                  g_param_spec_boolean ("move-hwnd","Move handle","Whether current widget wrapped by a move handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_BR_RESIZE,
                                  g_param_spec_boolean ("resize-hwnd","resize handle","Whether current widget wrapped by a resize(bottom right) handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_RIGHT,
                                  g_param_spec_boolean ("right-hwnd","Right handle","Whether current widget wrapped by a right handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));

gtk_container_class_install_child_property (cclass,
                                  CHILD_PROP_HANDLE_BOTTOM,
                                  g_param_spec_boolean ("bottom-hwnd","Bottom handle","Whether current widget wrapped by a bottom handle.",
                                  FALSE,
                                  G_PARAM_READWRITE));

*/


g_type_class_add_private(klass,sizeof(FloatBorderPriv));



}




static void float_border_init(FloatBorder* fb)
{
FloatBorderPriv* priv=FLOAT_BODER_GET_PRIV(fb);
fb->priv=priv;

GtkWidget*widget=GTK_WIDGET(fb);

gtk_widget_set_has_window(widget,FALSE);

priv->children=NULL;

priv->hwnd_mask[HWND_MOVE]=TRUE;
priv->hwnd_mask[HWND_BR_RESIZE]=TRUE;
priv->hwnd_mask[HWND_BL_RESIZE]=TRUE;
priv->hwnd_mask[HWND_TL_RESIZE]=TRUE;
priv->hwnd_mask[HWND_TR_RESIZE]=TRUE;

priv->hwnd_mask[HWND_RIGHT]=TRUE;
priv->hwnd_mask[HWND_BOTTOM]=TRUE;
priv->hwnd_mask[HWND_LEFT]=TRUE;
priv->hwnd_mask[HWND_TOP]=TRUE;
priv->show_border=TRUE;
/*
priv->frozen=FALSE;
priv->may_overlap=TRUE;
priv->may_shrink=TRUE;
*/
priv->widget2child=g_hash_table_new(g_direct_hash,g_direct_equal);
priv->window2child=g_hash_table_new(g_direct_hash,g_direct_equal);



}



static void float_border_set_property(GObject*object,guint prop_id,const GValue*value,GParamSpec*pspec)
{

    FloatBorder *fb=FLOAT_BORDER(object);
    FloatBorderPriv*priv=fb->priv;

    switch(prop_id){
    
        case PROP_FROZEN:
            fb_set_frozen(fb,g_value_get_boolean(value));

            break;
        case PROP_MAY_OVERLAP:
            priv->may_overlap=g_value_get_boolean(value);
//            fb_set_maybe_overlapped(fb,priv->maybe_overlapped);
            break;
        case PROP_MAY_SHRINK:
            fb_set_may_shrink(fb,g_value_get_boolean(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object,prop_id,pspec);
            break;
    
    }

}



static void float_border_get_property(GObject*object,guint prop_id,GValue*value,GParamSpec*pspec)
{

    FloatBorder* fb=FLOAT_BORDER(object);
    FloatBorderPriv*priv=fb->priv;

    switch(prop_id){
    
        case PROP_FROZEN:
            g_value_set_boolean(value,priv->frozen);
            break;
        case PROP_MAY_OVERLAP:
            g_value_set_boolean(value,priv->may_overlap);
            break;
        case PROP_MAY_SHRINK:
            g_value_set_boolean(value,priv->may_shrink);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object,prop_id,pspec);
            break;
    
    }

}


static void float_border_finalize(GObject*object)
{

FloatBorder* self=FLOAT_BORDER(object);

g_hash_table_destroy(self->priv->widget2child);
g_hash_table_destroy(self->priv->window2child);


G_OBJECT_CLASS(float_border_parent_class)->finalize(object);

}



typedef gboolean (*FBCallBack)(FloatBorderChild*,gpointer);


static void for_all_children(FloatBorder*fb,FBCallBack callback, gpointer data)
{

    FloatBorderPriv*priv=(fb)->priv;
    GList*children=priv->children;

    gboolean b;
    while(children){
        
        b=(*callback)(children->data,data);
        if(b)
            break;
        children=children->next;
    }

}





static FloatBorderChild*get_child_by_widget(FloatBorder*fb,GtkWidget*w)
{

    g_message("get_child_by_widget()");

    FloatBorderPriv*priv=fb->priv;
    FloatBorderChild*fbchild;
   
    g_return_val_if_fail(IS_FLOAT_BORDER(fb),NULL);
    g_return_val_if_fail(GTK_IS_WIDGET(w),NULL);

/*
    fbchild=g_hash_table_lookup(priv->widget2child,w);

    if(fbchild)
        return fbchild;
*/
    g_message("---->widget[%x] not cached",w);
    GList*children;

    children=priv->children;

    while(children)
    {
        fbchild=children->data;

            if(fbchild->widget == w){
//            g_hash_table_insert(priv->widget2child,w,fbchild);

                return fbchild;
            }
        children=children->next;
    }

    return NULL;

}





static FloatBorderChild*get_child_by_window(FloatBorder*fb,GdkWindow*window)
{

    FloatBorderPriv*priv=fb->priv;
    FloatBorderChild*fbchild;


    g_return_val_if_fail(IS_FLOAT_BORDER(fb),NULL);
    g_return_val_if_fail(GDK_IS_WINDOW(window),NULL);
/*
    fbchild=g_hash_table_lookup(priv->window2child,window);
    if(fbchild)
        return fbchild;
*/
    g_message("---->window[%x] not cached",window);
    GList*list;
    for (list=priv->children;list;list=list->next){
    
        fbchild=list->data;
        
        int i;
        for(i=0;i<N_HWNDS;i++){
            if(!fbchild->hwnded[i])
                continue;
            if(fbchild->hwnds[i] == window){
//            g_hash_table_insert(priv->window2child,window,fbchild);
                return fbchild;
            }
        }
    }
    return NULL;

}




static void
float_border_set_child_property (GtkContainer *container,GtkWidget    *child,
                            guint prop_id,const GValue *value,GParamSpec   *pspec)
{

    FloatBorder*fb=FLOAT_BORDER(container);
    FloatBorderChild*fbchild=get_child_by_widget(fb,child);

    switch(prop_id)
    {

        case CHILD_PROP_X:
            fbchild->x=g_value_get_int(value);
            break;

        case CHILD_PROP_Y:
            fbchild->y=g_value_get_int(value);
            break;

        case CHILD_PROP_HANDLE_MOVE:
            fbchild->hwnded[HWND_MOVE]=g_value_get_boolean(value);
            break;
        case CHILD_PROP_HANDLE_BR_RESIZE:
            fbchild->hwnded[HWND_BR_RESIZE]=g_value_get_boolean(value);
            break;

        case CHILD_PROP_HANDLE_RIGHT:
            fbchild->hwnded[HWND_RIGHT]=g_value_get_boolean(value);
            break;

        case CHILD_PROP_HANDLE_BOTTOM:
            fbchild->hwnded[HWND_BOTTOM]=g_value_get_boolean(value);
            break;

        case CHILD_PROP_ACTIVE:
            fbchild->active=g_value_get_boolean(value);
            break;

        case CHILD_PROP_OVERLAP:
            fbchild->overlap=g_value_get_boolean(value);
            break;

        default:

            GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID(container,prop_id,pspec);
            break;
    
    }
    
//    gtk_widget_queue_draw()

    if(gtk_widget_get_visible(child)&& gtk_widget_get_visible(GTK_WIDGET(fb)))
        gtk_widget_queue_resize(child);

}



static void
float_border_get_child_property (GtkContainer *container,GtkWidget    *child,
                            guint prop_id,GValue *value,GParamSpec   *pspec)
{


    FloatBorder*fb=FLOAT_BORDER(container);
    FloatBorderChild*fbchild=get_child_by_widget(fb,child);


    switch(prop_id)
    {

        case CHILD_PROP_X:
            g_value_set_int(value,fbchild->x);
            break;

        case CHILD_PROP_Y:
            g_value_set_int(value,fbchild->y);
            break;

        case CHILD_PROP_HANDLE_MOVE:
            g_value_set_boolean(value,fbchild->hwnded[HWND_MOVE]);

            break;
        case CHILD_PROP_HANDLE_BR_RESIZE:
            g_value_set_boolean(value,fbchild->hwnded[HWND_BR_RESIZE]);
            break;

        case CHILD_PROP_HANDLE_RIGHT:
            g_value_set_boolean(value,fbchild->hwnded[HWND_RIGHT]);
            break;

        case CHILD_PROP_HANDLE_BOTTOM:
            g_value_set_boolean(value,fbchild->hwnded[HWND_BOTTOM]);
            break;

        case CHILD_PROP_ACTIVE:
            g_value_set_boolean(value,fbchild->active);

            break;

        case CHILD_PROP_OVERLAP:
            g_value_set_boolean(value,fbchild->overlap);

            break;

        default:

            GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID(container,prop_id,pspec);
            break;
    
    }


}






static GType float_border_child_type(GtkContainer*container)
{
    return GTK_TYPE_WIDGET;
}



static void _get_next_position(FloatBorder*fb,gint* x,gint* y)
{

    int n=float_border_count(fb)+1;
    if(x)
        *x=n*HWND_THICKNESS;
    if(y)
        *y=n*HWND_THICKNESS;

}


static void _float_border_add(GtkContainer*container,GtkWidget*widget)
{

    FloatBorder*fb=FLOAT_BORDER(container);
    FloatBorderPriv*priv=fb->priv;

    int ix,iy;
    _get_next_position(fb,&ix,&iy);

    float_border_put(fb,widget,ix,iy);


}


static void _float_border_remove(GtkContainer*container,GtkWidget*widget)
{

    float_border_remove(FLOAT_BORDER(container),widget);

}



static void _float_border_forall(GtkContainer*container, gboolean include_internals,GtkCallback callback,gpointer callback_data)
{

    FloatBorder* fb=FLOAT_BORDER(container);
    FloatBorderPriv* priv=fb->priv;

    FloatBorderChild*fbchild;

    GList*list=priv->children;
    g_print("=-list::%x\n\n",list);
    while(list){
    fbchild=list->data;
   (*callback)(fbchild->widget,callback_data); 
    list=list->next;
    }

}





/**/

typedef struct size_info{

    int expect_width;
    int expect_height;

}SizeInfo;


static gboolean size_negotiation(FloatBorderChild*fbc,gpointer d){

    SizeInfo*info=(SizeInfo*)d;

    int siz_x,siz_y;

    GtkRequisition min;

    if(fbc->min_siz.width==-1 || fbc->min_siz.width==-1){

    GtkRequisition childmin;
    gtk_widget_get_preferred_size(fbc->widget,&childmin,NULL);

    fbc->min_siz.width=childmin.width;
    fbc->min_siz.height=childmin.height;


    siz_x=(fbc->min_siz.width < fbc->pref_siz.width)?fbc->pref_siz.width:fbc->min_siz.width;
    siz_y=(fbc->min_siz.height < fbc->pref_siz.height)?fbc->pref_siz.height:fbc->min_siz.height;

    fbc->position[REF_CORNER].x=siz_x;
    fbc->position[REF_CORNER].y=siz_y;

    }



   /* traverse preferred size of each child widget*/ 
    min.width=fbc->position[REF_CORNER].x+fbc->x;
    min.height=fbc->position[REF_CORNER].y+fbc->y;

    if(fbc->active){
        min.width+=HWND_THICKNESS;
        min.height+=HWND_THICKNESS;
    }


    if(min.width > info->expect_width)
        info->expect_width=min.width;

    if(min.height> info->expect_height)
        info->expect_height=min.height;


    return FALSE;

}



static void float_border_get_preferred_size(GtkWidget*widget, GtkRequisition *minimum_req,GtkRequisition* natural_req)
{

    FloatBorder*fb=FLOAT_BORDER(widget);


    SizeInfo expect_size={0,};
    for_all_children(fb,size_negotiation,&expect_size);

    if(minimum_req){
        minimum_req->width=expect_size.expect_width;
        minimum_req->height=expect_size.expect_height;
    }

    if(natural_req){
        natural_req->width=expect_size.expect_width;
        natural_req->height=expect_size.expect_height;
    }

}





static void float_border_get_preferred_width(GtkWidget* widget,gint *minimum,gint *natural)
{

    GtkRequisition minimum_req;
    GtkRequisition natural_req;

    float_border_get_preferred_size(widget,&minimum_req,&natural_req);

    if(minimum)
        (*minimum)=minimum_req.width;
    if(natural)
        (*natural)=natural_req.width;
}






static void float_border_get_preferred_height(GtkWidget* widget,gint *minimum,gint *natural)
{

    GtkRequisition minimum_req;
    GtkRequisition natural_req;

    float_border_get_preferred_size(widget,&minimum_req,&natural_req);

    if(minimum)
        (*minimum)=minimum_req.height;
    if(natural)
        (*natural)=natural_req.height;

}




static gboolean float_border_button_press(GtkWidget*widget,GdkEventButton*e)
{

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv=fb->priv;

    FloatBorderChild*fbchild=get_child_by_window(fb,e->window);

    if(!fbchild){
        return FALSE;
    }




    if(e->type==GDK_BUTTON_PRESS&&e->button==GDK_BUTTON_PRIMARY){

        if(e->window == fbchild->hwnds[HWND_RIGHT]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=0;
            return TRUE;
        }
    
        if(e->window == fbchild->hwnds[HWND_BOTTOM]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=0;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }
     

        if(e->window == fbchild->hwnds[HWND_LEFT]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=0;
            return TRUE;
        }
    
        if(e->window == fbchild->hwnds[HWND_TOP]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=0;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }


        if(e->window == fbchild->hwnds[HWND_BR_RESIZE]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_TR_RESIZE]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_BL_RESIZE]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_TL_RESIZE]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }



        if(e->window == fbchild->hwnds[HWND_MOVE]){
            priv->is_dragging=TRUE;
            fbchild->in_drag=TRUE;
            fbchild->drag_position[X]=e->x;
            fbchild->drag_position[Y]=e->y;
            return TRUE;
        }



    }
    return FALSE;

}



static gboolean float_border_button_release(GtkWidget*widget,GdkEventButton*e)
{

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv=fb->priv;
    FloatBorderChild*fbchild=get_child_by_window(fb,e->window);

    if(!fbchild){
        return FALSE;
    }

    if(e->type==GDK_BUTTON_RELEASE&&e->button==GDK_BUTTON_PRIMARY){

        if(e->window == fbchild->hwnds[HWND_RIGHT]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_BOTTOM]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_LEFT]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_TOP]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_BR_RESIZE]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_TR_RESIZE]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_BL_RESIZE]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }

        if(e->window == fbchild->hwnds[HWND_TL_RESIZE]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }


 
        if(e->window == fbchild->hwnds[HWND_MOVE]){
            priv->is_dragging=FALSE;
            fbchild->in_drag=FALSE;
            return TRUE;
        }
   
    }
    return FALSE;

}






static gboolean float_border_motion_notify(GtkWidget*widget,GdkEventMotion*e)
{

//    g_print(":: Motion Notify(window:%x)\n",e->window);

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv=fb->priv;
    FloatBorderChild*fbchild=get_child_by_window(fb,e->window);

    if(fbchild==NULL){
        return FALSE;
    }

    GtkAllocation allocation;
    gtk_widget_get_allocation(widget,&allocation);

    int biasx,biasy;
    int originx,originpx;
    int originy,originpy;


    if(priv->is_dragging){

    if(e->window == fbchild->hwnds[HWND_RIGHT]){
        biasx=e->x - fbchild->drag_position[X];
        fbchild->position[REF_CORNER].x  += biasx;
        if(fbchild->shrink && priv->may_shrink){
        fbchild->position[REF_CORNER].x = CLAMP(fbchild->position[REF_CORNER].x,
                0,
                allocation.width-HWND_THICKNESS-fbchild->x);
        }else{
        fbchild->position[REF_CORNER].x = CLAMP(fbchild->position[REF_CORNER].x,
                fbchild->min_siz.width,
                allocation.width-HWND_THICKNESS-fbchild->x);
        }
    }

    if(e->window == fbchild->hwnds[HWND_BOTTOM]){
        biasy=e->y - fbchild->drag_position[Y];
        fbchild->position[REF_CORNER].y += biasy;
        if(fbchild->shrink && priv->may_shrink){
         fbchild->position[REF_CORNER].y = CLAMP(fbchild->position[REF_CORNER].y,
                0,
                allocation.height-HWND_THICKNESS-fbchild->y);
        }else{
        fbchild->position[REF_CORNER].y = CLAMP(fbchild->position[REF_CORNER].y,
                fbchild->min_siz.height,
                allocation.height-HWND_THICKNESS-fbchild->y);
        }
    }


    if(e->window == fbchild->hwnds[HWND_LEFT]){

        biasx=e->x-fbchild->drag_position[X];
        originx=fbchild->x;
        originpx=fbchild->position[REF_CORNER].x;

        fbchild->x += biasx;
        fbchild->position[REF_CORNER].x -= biasx;

        if(fbchild->shrink && priv->may_shrink){
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                originx+originpx);
        fbchild->position[REF_CORNER].x=originx+originpx-fbchild->x;

        }else{
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                originx+originpx-fbchild->min_siz.width);
        fbchild->position[REF_CORNER].x=originx+originpx-fbchild->x;

        }
    }

    if(e->window == fbchild->hwnds[HWND_TOP]){

        biasy=e->y - fbchild->drag_position[Y];
        originy=fbchild->y;
        originpy=fbchild->position[REF_CORNER].y;

        fbchild->y += biasy;
        fbchild->position[REF_CORNER].y -= biasy;

        if(fbchild->shrink && priv->may_shrink){
        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                originy+originpy);
        fbchild->position[REF_CORNER].y=originy+originpy-fbchild->y;
 
        }else{
        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                originy+originpy-fbchild->min_siz.height);
        fbchild->position[REF_CORNER].y=originy+originpy-fbchild->y;
 
        }
    }



    if(e->window == fbchild->hwnds[HWND_BR_RESIZE]){
        biasx=e->x-fbchild->drag_position[X];
        biasy=e->y-fbchild->drag_position[Y];
        fbchild->position[REF_CORNER].x  += biasx;
        fbchild->position[REF_CORNER].y += biasy;

        if(fbchild->shrink && priv->may_shrink){
        fbchild->position[REF_CORNER].x = CLAMP(fbchild->position[REF_CORNER].x,
                0,
                allocation.width-HWND_THICKNESS-fbchild->x);
        fbchild->position[REF_CORNER].y = CLAMP(fbchild->position[REF_CORNER].y,
                0,
                allocation.height-HWND_THICKNESS-fbchild->y);
        
        }else{
        fbchild->position[REF_CORNER].x = CLAMP(fbchild->position[REF_CORNER].x,
                fbchild->min_siz.width,
                allocation.width-HWND_THICKNESS-fbchild->x);
        fbchild->position[REF_CORNER].y = CLAMP(fbchild->position[REF_CORNER].y,
                fbchild->min_siz.height,
                allocation.height-HWND_THICKNESS-fbchild->y);
        }
    }


    if(e->window == fbchild->hwnds[HWND_TR_RESIZE]){
        biasx=e->x-fbchild->drag_position[X];
        biasy=e->y-fbchild->drag_position[Y];

        originy=fbchild->y;
        originpy=fbchild->position[REF_CORNER].y;

        fbchild->y += biasy;
        fbchild->position[REF_CORNER].x  += biasx;
        fbchild->position[REF_CORNER].y -= biasy;

        if(fbchild->shrink && priv->may_shrink){
        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                originy+originpy);
        fbchild->position[REF_CORNER].y=originy+originpy-fbchild->y;
         fbchild->position[REF_CORNER].x = CLAMP(fbchild->position[REF_CORNER].x,
                0,
                allocation.width-HWND_THICKNESS-fbchild->x);
 
        }else{
        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                originy+originpy-fbchild->min_siz.height);
        fbchild->position[REF_CORNER].y=originy+originpy-fbchild->y;
        fbchild->position[REF_CORNER].x = CLAMP(fbchild->position[REF_CORNER].x,
                fbchild->min_siz.width,
                allocation.width-HWND_THICKNESS-fbchild->x);
  
        }
 
    }


    if(e->window == fbchild->hwnds[HWND_BL_RESIZE]){
        biasx=e->x-fbchild->drag_position[X];
        biasy=e->y-fbchild->drag_position[Y];
        originx=fbchild->x;
        originpx=fbchild->position[REF_CORNER].x;

        fbchild->x += biasx;
        fbchild->position[REF_CORNER].x -= biasx;
        fbchild->position[REF_CORNER].y += biasy;

        if(fbchild->shrink && priv->may_shrink){
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                originx+originpx);
        fbchild->position[REF_CORNER].x=originx+originpx-fbchild->x;
         fbchild->position[REF_CORNER].y = CLAMP(fbchild->position[REF_CORNER].y,
                0,
                allocation.height-HWND_THICKNESS-fbchild->y);
 
        }else{
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                originx+originpx-fbchild->min_siz.width);
        fbchild->position[REF_CORNER].x=originx+originpx-fbchild->x;
        fbchild->position[REF_CORNER].y = CLAMP(fbchild->position[REF_CORNER].y,
                fbchild->min_siz.height,
                allocation.height-HWND_THICKNESS-fbchild->y);
 
        }
    }


    if(e->window == fbchild->hwnds[HWND_TL_RESIZE]){
        biasx=e->x - fbchild->drag_position[X];
        biasy=e->y - fbchild->drag_position[Y];
        originx=fbchild->x;
        originy=fbchild->y;
        originpx=fbchild->position[REF_CORNER].x;
        originpy=fbchild->position[REF_CORNER].y;

        fbchild->x += biasx;
        fbchild->y += biasy;
        fbchild->position[REF_CORNER].x -= biasx;
        fbchild->position[REF_CORNER].y -= biasy;

        if(fbchild->shrink && priv->may_shrink){
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                originx+originpx);
        fbchild->position[REF_CORNER].x=originx+originpx-fbchild->x;


        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                originy+originpy);
        fbchild->position[REF_CORNER].y=originy+originpy-fbchild->y;
 
        }else{
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                originx+originpx-fbchild->min_siz.width);
        fbchild->position[REF_CORNER].x=originx+originpx-fbchild->x;

        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                originy+originpy-fbchild->min_siz.height);
        fbchild->position[REF_CORNER].y=originy+originpy-fbchild->y;
 
        }

    }





    if(e->window == fbchild->hwnds[HWND_MOVE]){

        biasx=e->x - fbchild->drag_position[X];
        biasy=e->y - fbchild->drag_position[Y];

        fbchild->x  += biasx;// e->x - fbchild->drag_position[X];
        fbchild->x = CLAMP(fbchild->x,
                HWND_THICKNESS,
                allocation.width-fbchild->position[REF_CORNER].x-HWND_THICKNESS);

        fbchild->y += biasy;// e->y - fbchild->drag_position[Y];
        fbchild->y = CLAMP(fbchild->y,
                HWND_THICKNESS,
                allocation.height-fbchild->position[REF_CORNER].y-HWND_THICKNESS);

    }

    gtk_widget_queue_resize(widget);

    }

    return TRUE;
}






static gboolean create_hwnds(FloatBorderChild*fbc,gpointer d)
{

//    g_print(":: create hwnds \n");

    FloatBorder*fb=fbc->floatborder;//FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
    GtkWidget*widget=GTK_WIDGET(fb);

    GdkWindow*parent_win;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkAllocation allocation;

    if(fbc->active && !priv->frozen && gtk_widget_is_sensitive(fbc->widget)){

   /* need to create hwnds*/ 
    parent_win=gtk_widget_get_window(widget);


    gtk_widget_get_allocation(widget,&allocation);

    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.wclass=GDK_INPUT_ONLY;
    attributes.event_mask=gtk_widget_get_events(widget);
    attributes.event_mask|=(GDK_BUTTON_PRESS_MASK|
            GDK_BUTTON_RELEASE_MASK|
            GDK_ENTER_NOTIFY_MASK|
            GDK_LEAVE_NOTIFY_MASK|
            GDK_POINTER_MOTION_MASK);
    attributes_mask=GDK_WA_X|GDK_WA_Y;



    attributes.x=0;//fbc->width+allocation.x;
    attributes.y=0;//allocation.y;
    attributes.width=1;//HWND_THICKNESS;
    attributes.height=1;//fbc->height;

    if(fbc->hwnded[HWND_RIGHT]&&fbc->hwnds[HWND_RIGHT]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_SB_H_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_RIGHT]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_RIGHT]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_RIGHT]);
    }
 
    if(fbc->hwnded[HWND_BOTTOM]&&fbc->hwnds[HWND_BOTTOM]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_SB_V_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_BOTTOM]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_BOTTOM]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_BOTTOM]);
    }
    

    if(fbc->hwnded[HWND_LEFT]&&fbc->hwnds[HWND_LEFT]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_SB_H_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_LEFT]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_LEFT]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_LEFT]);
    }
 
    if(fbc->hwnded[HWND_TOP]&&fbc->hwnds[HWND_TOP]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_SB_V_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_TOP]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_TOP]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_TOP]);
    }

 
    if(fbc->hwnded[HWND_BL_RESIZE]&&fbc->hwnds[HWND_BL_RESIZE]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_BOTTOM_LEFT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_BL_RESIZE]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_BL_RESIZE]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_BL_RESIZE]);
    }

 
    if(fbc->hwnded[HWND_BR_RESIZE]&&fbc->hwnds[HWND_BR_RESIZE]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_BOTTOM_RIGHT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_BR_RESIZE]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_BR_RESIZE]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_BR_RESIZE]);
    }

    if(fbc->hwnded[HWND_TL_RESIZE]&&fbc->hwnds[HWND_TL_RESIZE]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_TOP_LEFT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_TL_RESIZE]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_TL_RESIZE]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_TL_RESIZE]);
    }


    if(fbc->hwnded[HWND_TR_RESIZE]&&fbc->hwnds[HWND_TR_RESIZE]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_TOP_RIGHT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_TR_RESIZE]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_TR_RESIZE]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_TR_RESIZE]);
    }


    if(fbc->hwnded[HWND_MOVE]&&fbc->hwnds[HWND_MOVE]==0){

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_FLEUR);
    attributes_mask|=GDK_WA_CURSOR;

    fbc->hwnds[HWND_MOVE]=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,fbc->hwnds[HWND_MOVE]);
    g_object_unref(attributes.cursor);
//    gdk_window_show(fbc->hwnds[HWND_MOVE]);
    }

    }else{
    //need not to create hwnds for it;
    }

    return FALSE;
}


static gboolean destroy_hwnds(FloatBorderChild*fbc,gpointer d){

    g_print("Desstroy HWNDS...%x\n\n",fbc);
    FloatBorder*fb=fbc->floatborder;//FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
 
    GtkWidget*widget=GTK_WIDGET(fb);

   
    if(fbc->hwnded[HWND_RIGHT]&&fbc->hwnds[HWND_RIGHT]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_RIGHT]);
        gdk_window_destroy(fbc->hwnds[HWND_RIGHT]);
    }

    if(fbc->hwnded[HWND_BOTTOM]&&fbc->hwnds[HWND_BOTTOM]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_BOTTOM]);
        gdk_window_destroy(fbc->hwnds[HWND_BOTTOM]);
    }

    if(fbc->hwnded[HWND_LEFT]&&fbc->hwnds[HWND_LEFT]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_LEFT]);
        gdk_window_destroy(fbc->hwnds[HWND_LEFT]);
    }

    if(fbc->hwnded[HWND_TOP]&&fbc->hwnds[HWND_TOP]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_TOP]);
        gdk_window_destroy(fbc->hwnds[HWND_TOP]);
    }


    if(fbc->hwnded[HWND_BR_RESIZE]&&fbc->hwnds[HWND_BR_RESIZE]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_BR_RESIZE]);
        gdk_window_destroy(fbc->hwnds[HWND_BR_RESIZE]);
    }

    if(fbc->hwnded[HWND_BL_RESIZE]&&fbc->hwnds[HWND_BL_RESIZE]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_BL_RESIZE]);
        gdk_window_destroy(fbc->hwnds[HWND_BL_RESIZE]);
    }
    if(fbc->hwnded[HWND_TR_RESIZE]&&fbc->hwnds[HWND_TR_RESIZE]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_TR_RESIZE]);
        gdk_window_destroy(fbc->hwnds[HWND_TR_RESIZE]);
    }

    if(fbc->hwnded[HWND_TL_RESIZE]&&fbc->hwnds[HWND_TL_RESIZE]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_TL_RESIZE]);
        gdk_window_destroy(fbc->hwnds[HWND_TL_RESIZE]);
    }


    if(fbc->hwnded[HWND_MOVE]&&fbc->hwnds[HWND_MOVE]!=0){
    
        gtk_widget_unregister_window(widget,fbc->hwnds[HWND_MOVE]);
        gdk_window_destroy(fbc->hwnds[HWND_MOVE]);
    }


    return FALSE;

}



static gboolean map_hwnds(FloatBorderChild*fbc,gpointer d){

    FloatBorder*fb=fbc->floatborder;//FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
 //   GtkWidget*widget=GTK_WIDGET(fb);

    if(gtk_widget_get_visible(fbc->widget)&& !priv->frozen){

    if(fbc->hwnds[HWND_RIGHT] && fbc->hwnded[HWND_RIGHT]){
        gdk_window_show(fbc->hwnds[HWND_RIGHT]);
    }
    if(fbc->hwnds[HWND_BOTTOM] && fbc->hwnded[HWND_BOTTOM]){
        gdk_window_show(fbc->hwnds[HWND_BOTTOM]);
    }

    if(fbc->hwnds[HWND_LEFT] && fbc->hwnded[HWND_LEFT]){
        gdk_window_show(fbc->hwnds[HWND_LEFT]);
    }
    if(fbc->hwnds[HWND_TOP] && fbc->hwnded[HWND_TOP]){
        gdk_window_show(fbc->hwnds[HWND_TOP]);
    }

    if(fbc->hwnds[HWND_BR_RESIZE] && fbc->hwnded[HWND_BR_RESIZE]){
        gdk_window_show(fbc->hwnds[HWND_BR_RESIZE]);
    }
    if(fbc->hwnds[HWND_BL_RESIZE] && fbc->hwnded[HWND_BL_RESIZE]){
        gdk_window_show(fbc->hwnds[HWND_BL_RESIZE]);
    }
     if(fbc->hwnds[HWND_TR_RESIZE] && fbc->hwnded[HWND_TR_RESIZE]){
        gdk_window_show(fbc->hwnds[HWND_TR_RESIZE]);
    }
    
     if(fbc->hwnds[HWND_TL_RESIZE] && fbc->hwnded[HWND_TL_RESIZE]){
        gdk_window_show(fbc->hwnds[HWND_TL_RESIZE]);
    }

    if(fbc->hwnds[HWND_MOVE] && fbc->hwnded[HWND_MOVE]){
        gdk_window_show(fbc->hwnds[HWND_MOVE]);
    }

    }
    return FALSE;
}


static gboolean unmap_hwnds(FloatBorderChild*fbc,gpointer d){

    FloatBorder*fb=fbc->floatborder;//FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;


    if(!priv->frozen){


    if(fbc->hwnds[HWND_RIGHT] && fbc->hwnded[HWND_RIGHT]){
        gdk_window_hide(fbc->hwnds[HWND_RIGHT]);
    }
    if(fbc->hwnds[HWND_BOTTOM] && fbc->hwnded[HWND_BOTTOM]){
        gdk_window_hide(fbc->hwnds[HWND_BOTTOM]);
    }

    if(fbc->hwnds[HWND_LEFT] && fbc->hwnded[HWND_LEFT]){
        gdk_window_hide(fbc->hwnds[HWND_LEFT]);
    }
    if(fbc->hwnds[HWND_TOP] && fbc->hwnded[HWND_TOP]){
        gdk_window_hide(fbc->hwnds[HWND_TOP]);
    }

    if(fbc->hwnds[HWND_BR_RESIZE] && fbc->hwnded[HWND_BR_RESIZE]){
        gdk_window_hide(fbc->hwnds[HWND_BR_RESIZE]);
    }
    if(fbc->hwnds[HWND_BL_RESIZE] && fbc->hwnded[HWND_BL_RESIZE]){
        gdk_window_hide(fbc->hwnds[HWND_BL_RESIZE]);
    }
     if(fbc->hwnds[HWND_TR_RESIZE] && fbc->hwnded[HWND_TR_RESIZE]){
        gdk_window_hide(fbc->hwnds[HWND_TR_RESIZE]);
    }
    
     if(fbc->hwnds[HWND_TL_RESIZE] && fbc->hwnded[HWND_TL_RESIZE]){
        gdk_window_hide(fbc->hwnds[HWND_TL_RESIZE]);
    }

    if(fbc->hwnds[HWND_MOVE] && fbc->hwnded[HWND_MOVE]){
        gdk_window_hide(fbc->hwnds[HWND_MOVE]);
    }

    }

    return FALSE;

}


static void float_border_realize(GtkWidget*widget)
{

    GtkAllocation allocation;
    GdkWindow * window;
    GdkWindowAttr attributes;
    gint attributes_mask;

    gtk_widget_set_realized(widget,TRUE);


    if(!gtk_widget_get_has_window(widget)){
        window=gtk_widget_get_parent_window(widget);
//        g_print("window ::%x\n",window);
        gtk_widget_set_window(widget,window);
        g_object_ref(window);
//        GTK_WIDGET_CLASS(float_border_parent_class)->realize(widget);

    }else{
        gtk_widget_get_allocation(widget,&allocation);

        attributes.window_type=GDK_WINDOW_CHILD;
        attributes.x=allocation.x;
        attributes.y=allocation.y;
        attributes.width=allocation.width;
        attributes.height=allocation.height;

        attributes.wclass=GDK_INPUT_OUTPUT;
        attributes.visual=gtk_widget_get_visual(widget);
        attributes.event_mask=gtk_widget_get_events(widget)|
            GDK_EXPOSURE_MASK|
            GDK_BUTTON_PRESS_MASK|
            GDK_BUTTON_RELEASE_MASK|
            GDK_POINTER_MOTION_MASK;

        attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL;

        window=gdk_window_new(gtk_widget_get_parent_window(widget),&attributes,attributes_mask);
        gtk_widget_set_window(widget,window);
        gtk_widget_register_window(widget,window);
        gtk_style_context_set_background (gtk_widget_get_style_context (widget), window);


    }


    //create hwnds for every child;

    FloatBorder*fb=FLOAT_BORDER(widget);

    for_all_children(fb,create_hwnds,fb);


}




static void float_border_unrealize(GtkWidget*widget)
{

    FloatBorder*fb=FLOAT_BORDER(widget);

    for_all_children(fb,destroy_hwnds,fb);

    GTK_WIDGET_CLASS(float_border_parent_class)->unrealize(widget);


}

static void float_border_map(GtkWidget*widget)
{

    FloatBorder*fb=FLOAT_BORDER(widget);


    GTK_WIDGET_CLASS(float_border_parent_class)->map(widget);

    for_all_children(fb,map_hwnds,fb);


}



static void float_border_unmap(GtkWidget*widget)
{


    FloatBorder*fb=FLOAT_BORDER(widget);
    for_all_children(fb,unmap_hwnds,fb);

    GTK_WIDGET_CLASS(float_border_parent_class)->unmap(widget);


}



static gboolean child_sizeallocate(FloatBorderChild*fbchild,gpointer d)
{

//    g_print(":: child_sizeallocate()\n");

    FloatBorder*fb=FLOAT_BORDER(d);
    FloatBorderPriv*priv=fb->priv;
    GtkWidget*widget=GTK_WIDGET(fb);

    GtkRequisition child_requisition;
    GtkAllocation child_allocation;
    GtkAllocation allocation;

    gtk_widget_get_allocation(widget,&allocation);


    if(!gtk_widget_get_visible(fbchild->widget))
        return FALSE;
    
    child_allocation.x=fbchild->x;
    child_allocation.y=fbchild->y;

    if (!gtk_widget_get_has_window (widget)){
        child_allocation.x += allocation.x;
        child_allocation.y += allocation.y;
    }

    child_allocation.width = fbchild->position[REF_CORNER].x;
    child_allocation.height = fbchild->position[REF_CORNER].y;
    gtk_widget_size_allocate (fbchild->widget, &child_allocation);

    fbchild->position[REF_CORNER].width=child_allocation.width;
    fbchild->position[REF_CORNER].height=child_allocation.height;


/*    size allocation for handles.*/

    if(fbchild->hwnded[HWND_RIGHT] &&fbchild->hwnds[HWND_RIGHT]){
        gdk_window_move_resize(fbchild->hwnds[HWND_RIGHT],
                child_allocation.x+fbchild->position[REF_CORNER].x,
                child_allocation.y,
                HWND_THICKNESS,
                child_allocation.height
                );
    }

    if(fbchild->hwnded[HWND_BOTTOM] &&fbchild->hwnds[HWND_BOTTOM]){
        gdk_window_move_resize(fbchild->hwnds[HWND_BOTTOM],
                child_allocation.x,
                child_allocation.y+fbchild->position[REF_CORNER].y,
                child_allocation.width,
                HWND_THICKNESS
                );
    }

    if(fbchild->hwnded[HWND_LEFT]&&fbchild->hwnds[HWND_LEFT]){
        gdk_window_move_resize(fbchild->hwnds[HWND_LEFT],
                child_allocation.x-HWND_THICKNESS,
                child_allocation.y,
                HWND_THICKNESS,
                child_allocation.height
                );
    }

    if(fbchild->hwnded[HWND_TOP]&&fbchild->hwnds[HWND_TOP]){
        gdk_window_move_resize(fbchild->hwnds[HWND_TOP],
                child_allocation.x,
                child_allocation.y-HWND_THICKNESS,
                child_allocation.width,
                HWND_THICKNESS
                );
    }


    if(fbchild->hwnded[HWND_BR_RESIZE]&&fbchild->hwnds[HWND_BR_RESIZE]){
        gdk_window_move_resize(fbchild->hwnds[HWND_BR_RESIZE],
                child_allocation.x+fbchild->position[REF_CORNER].x,
                child_allocation.y+fbchild->position[REF_CORNER].y,
                HWND_THICKNESS,
                HWND_THICKNESS
                );

    }

    if(fbchild->hwnded[HWND_BL_RESIZE]&&fbchild->hwnds[HWND_BL_RESIZE]){
        gdk_window_move_resize(fbchild->hwnds[HWND_BL_RESIZE],
                child_allocation.x-HWND_THICKNESS,
                child_allocation.y+fbchild->position[REF_CORNER].y,
                HWND_THICKNESS,
                HWND_THICKNESS
                );
    }

    if(fbchild->hwnded[HWND_TR_RESIZE]&&fbchild->hwnds[HWND_TR_RESIZE]){
        gdk_window_move_resize(fbchild->hwnds[HWND_TR_RESIZE],
                child_allocation.x+fbchild->position[REF_CORNER].x,
                child_allocation.y-HWND_THICKNESS,
                HWND_THICKNESS,
                HWND_THICKNESS
                );
    }

    if(fbchild->hwnded[HWND_TL_RESIZE]&&fbchild->hwnds[HWND_TL_RESIZE]){
        gdk_window_move_resize(fbchild->hwnds[HWND_TL_RESIZE],
                child_allocation.x-HWND_THICKNESS,
                child_allocation.y-HWND_THICKNESS,
                HWND_THICKNESS,
                HWND_THICKNESS
                );
    }

    if(fbchild->hwnded[HWND_MOVE]&&fbchild->hwnds[HWND_MOVE]){
        gdk_window_move_resize(fbchild->hwnds[HWND_MOVE],
                child_allocation.x-HWND_THICKNESS,
                child_allocation.y-HWND_THICKNESS,
//                child_allocation.width,
//                child_allocation.height
                HWND_THICKNESS,
                HWND_THICKNESS
                );

    }

    return FALSE;

}


static gboolean draw_child(FloatBorderChild*fbc,gpointer d)
{
    FloatBorder*fb=fbc->floatborder;
    GtkWidget*widget=GTK_WIDGET(fb);
  

    cairo_t*cr=(cairo_t*)d;

    gtk_container_propagate_draw(GTK_CONTAINER(fb),fbc->widget,cr);
//    cairo_set_source_rgba(cr,0.9,0.9,0.9,0.1);

    GtkStyleContext*sc=gtk_widget_get_style_context(widget);
//    gtk_style_context_add_class(sc,GTK_STYLE_CLASS_OSD);
//    cairo_save(cr);
//    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_MOVE]);
    if(fb->priv->show_border){
    gtk_render_focus(sc,cr,
  //          HWND_THICKNESS-2,
//            HWND_THICKNESS-2,
            fbc->x-(HWND_THICKNESS>>1),
            fbc->y-(HWND_THICKNESS>>1),
            fbc->position[REF_CORNER].x+HWND_THICKNESS,
            fbc->position[REF_CORNER].y+HWND_THICKNESS);

//    cairo_restore(cr);
    cairo_set_source_rgba(cr,0.9,0.9,0.9,0.1);
    cairo_set_line_width(cr,HWND_THICKNESS);
   
    cairo_rectangle(cr,fbc->x - (HWND_THICKNESS>>1),
            fbc->y - (HWND_THICKNESS>>1),
            fbc->position[REF_CORNER].x + HWND_THICKNESS,
            fbc->position[REF_CORNER].y + HWND_THICKNESS);

    cairo_stroke(cr);
    }


    return FALSE;

    /*
    cairo_save(cr);
    if(fbc->hwnded[HWND_RIGHT]){
//        g_print("draw:: HWND_RIGHT\n");
    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_RIGHT]);
    cairo_rectangle(cr,0,0,
            HWND_THICKNESS,
            fbc->position[REF_CORNER].height);
    cairo_fill(cr);
    }
    cairo_restore(cr);
    cairo_save(cr);
    if(fbc->hwnded[HWND_BOTTOM]){
//        g_print("draw:: HWND_BOTTOM\n");
    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_BOTTOM]);
    cairo_rectangle(cr,0,0,
            fbc->position[REF_CORNER].width,
            HWND_THICKNESS);
    cairo_fill(cr);
    }
    cairo_restore(cr);
    cairo_save(cr);
    if(fbc->hwnded[HWND_LEFT]){
//        g_print("draw:: HWND_BOTTOM\n");
    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_LEFT]);
    cairo_rectangle(cr,0,0,
            HWND_THICKNESS,
            fbc->position[REF_CORNER].height);
    cairo_fill(cr);
    }
    cairo_restore(cr);
    cairo_save(cr);
    if(fbc->hwnded[HWND_TOP]){
//        g_print("draw:: HWND_BOTTOM\n");
    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_TOP]);
    cairo_rectangle(cr,0,0,
            fbc->position[REF_CORNER].width,
            HWND_THICKNESS);
    cairo_fill(cr);
    }
    cairo_restore(cr);

    cairo_save(cr);
    if(fbc->hwnded[HWND_BR_RESIZE]){

    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_BR_RESIZE]);
    cairo_rectangle(cr,0,0,HWND_THICKNESS,HWND_THICKNESS);
    cairo_fill(cr);
    }
    cairo_restore(cr);
    cairo_save(cr);
    if(fbc->hwnded[HWND_TR_RESIZE]){

    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_TR_RESIZE]);
    cairo_rectangle(cr,0,0,HWND_THICKNESS,HWND_THICKNESS);
    cairo_fill(cr);
    }
    cairo_restore(cr);
    cairo_save(cr);
    if(fbc->hwnded[HWND_BL_RESIZE]){

    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_BL_RESIZE]);
    cairo_rectangle(cr,0,0,HWND_THICKNESS,HWND_THICKNESS);
    cairo_fill(cr);
    }
    cairo_restore(cr);
    cairo_save(cr);
    if(fbc->hwnded[HWND_TL_RESIZE]){

    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_TL_RESIZE]);
    cairo_rectangle(cr,0,0,HWND_THICKNESS,HWND_THICKNESS);
    cairo_fill(cr);
    }
    cairo_restore(cr);
    cairo_save(cr);
    if(fbc->hwnded[HWND_MOVE]){

    gtk_cairo_transform_to_window(cr,widget,fbc->hwnds[HWND_MOVE]);
    cairo_rectangle(cr,0,0,HWND_THICKNESS,HWND_THICKNESS);
    cairo_fill(cr);
    }
    cairo_restore(cr);
 
    return FALSE;
*/
}



static gboolean float_border_draw(GtkWidget*widget,cairo_t*cr)
{

    FloatBorder*fb=FLOAT_BORDER(widget);

//GTK_WIDGET_CLASS(float_border_parent_class)->draw(widget,cr);
    for_all_children(fb,draw_child,cr);


    return FALSE;

}









static void float_border_size_allocate(GtkWidget*widget, GtkAllocation *allocation)
{

    FloatBorder*fb=FLOAT_BORDER(widget);
    FloatBorderPriv* priv = fb->priv;
    
    GtkRequisition child_requisition;
    GtkAllocation child_allocation;

    gtk_widget_set_allocation(widget,allocation);

    if(gtk_widget_get_has_window(widget))
    {
        if(gtk_widget_get_realized(widget)){
        gdk_window_move_resize(gtk_widget_get_window(widget),
                    allocation->x,
                    allocation->y,
                    allocation->width,
                    allocation->height);
        
        }
    
    }

    if(gtk_widget_get_realized(widget))
        for_all_children(fb,child_sizeallocate,fb);


}




static void mask_props(FloatBorder*fb,FloatBorderChild*fbc)
{

    FloatBorderPriv*priv=fb->priv;

    fbc->shrink=priv->may_shrink&fbc->shrink;
    fbc->overlap=priv->may_overlap&fbc->overlap;
    fbc->floatborder=fb;

}


static FloatBorderChild* init_fbchild(GtkWidget*widget,int x,int y,int w,int h,gboolean active,gboolean shrink,...)
{
    FloatBorderChild*new_child;

    new_child=g_slice_new0(FloatBorderChild);

    new_child->widget=widget;

    new_child->x=x;
    new_child->y=y;

    new_child->min_siz.width=-1;
    new_child->min_siz.height=-1;
    
    new_child->pref_siz.width=w;
    new_child->pref_siz.height=h;

    new_child->active=active;
    new_child->shrink=shrink;

    va_list ap;
    va_start(ap,shrink);

    int hwnd_on;
    while((hwnd_on=va_arg(ap,int))){
    new_child->hwnded[hwnd_on]=TRUE;
    }

    va_end(ap);

    return new_child;
}


static void fini_fbchild(FloatBorderChild*child)
{

    g_slice_free(FloatBorderChild,child);

}




static void _float_border_put(FloatBorder*fb,GtkWidget*widget,int x,int y,int w,int h,int act,int shr)
{
    g_message("f_put(fb:%x,w:%x)...",fb,widget);

    FloatBorderPriv*priv=fb->priv;
    FloatBorderChild*new_child;


    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_IS_WIDGET(widget));
 
    new_child=init_fbchild(widget,x,y,w,h,act,shr,
            HWND_RIGHT,HWND_BOTTOM,HWND_LEFT,HWND_TOP,
            HWND_BR_RESIZE,HWND_TR_RESIZE,HWND_BL_RESIZE,HWND_MOVE,NULL);
    mask_props(fb,new_child);

    gtk_widget_set_parent(widget,GTK_WIDGET(fb));
    priv->children=g_list_append(priv->children,new_child);

    gtk_widget_child_focus(GTK_WIDGET(widget),GTK_DIR_TAB_FORWARD);

    dispose_hwnds(new_child);
//    create_hwnds(new_child,NULL);
//    map_hwnds(new_child,NULL);
    gtk_widget_queue_resize(GTK_WIDGET(fb));

}



void float_border_put(FloatBorder*fb, GtkWidget*w, int x,int y)
{
    _float_border_put(fb,w,x,y,-1,-1,TRUE,FALSE);

}




void float_border_put_with_size(FloatBorder*fb, GtkWidget*widget, int x,int y,int w,int h)
{
    _float_border_put(fb,widget,x,y,w,h,TRUE,TRUE);

}



static void remove_cache(FloatBorderChild*fbchild)
{
    FloatBorder* fb=fbchild->floatborder;
    g_print("fb.:%x...\n",fb);
    FloatBorderPriv*priv=fb->priv;
    g_print("fb:%x.....fb->priv:%x...window2child:%x...\n",fb,priv,priv->window2child);

    int i;
    for(i=0;i<N_HWNDS;i++){
        if(fbchild->hwnded[i]&& fbchild->hwnds[i])
            g_hash_table_remove(priv->window2child,fbchild->hwnds[i]);
    }

}


void float_border_remove(FloatBorder*fb,GtkWidget*w)
{

    g_message("f_remove(fb:%x,w:%x)...",fb,w);

    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    FloatBorderPriv*priv=fb->priv;
    FloatBorderChild* fbchild;

    fbchild=get_child_by_widget(fb,w);
    if(fbchild){
    
        gboolean was_visible=gtk_widget_get_visible(w);
    
        gtk_widget_unparent(w);

        priv->children=g_list_remove(priv->children,fbchild);
        unmap_hwnds(fbchild,NULL);
        destroy_hwnds(fbchild,NULL);
        remove_cache(fbchild);
        fini_fbchild(fbchild);


        if(was_visible && gtk_widget_get_visible(GTK_WIDGET(fb)))
            gtk_widget_queue_resize(GTK_WIDGET(fb));
    
    }

}





void float_border_move(FloatBorder*fb,GtkWidget*w,int tox,int toy)
{
    //g_print("FloatBorder*::%x\n\n",fb);
    
    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    GtkWidget*widget=GTK_WIDGET(fb);
    FloatBorderChild*fbchild=get_child_by_widget(fb,w);

    if(fbchild==NULL)
        g_warning(" The Widget you specified is not contained by FloatBorder.");

    fbchild->x=tox;
    fbchild->y=toy;

    gtk_widget_queue_resize(widget);

}


void float_border_resize(FloatBorder*fb,GtkWidget*w,int towidth,int toheight)
{

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);

    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    if(fbchild==NULL)
        g_warning(" The Widget you specified is not contained by FloatBorder.");

    fbchild->position[REF_CORNER].x=towidth;
    fbchild->position[REF_CORNER].y=toheight;

//    g_message("Before queue resize");
    gtk_widget_queue_resize(GTK_WIDGET(fb));

}



void float_border_get_current_size(FloatBorder*fb,GtkWidget*w,int* width,int* height)
{


    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);  

    if(fbchild==NULL)
        g_warning(" The Widget you specified is not contained by FloatBorder.");

    if(width)
        *width=fbchild->position[REF_CORNER].x;
    if(height)
        *height=fbchild->position[REF_CORNER].y;


}

void float_border_get_current_position(FloatBorder*fb,GtkWidget*w,int* x,int* y)
{


    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);  

    if(fbchild==NULL)
        g_warning(" The Widget you specified is not contained by FloatBorder.");

    if(x)
        *x=fbchild->x;
    if(y)
        *y=fbchild->y;


}






void float_border_move_resize(FloatBorder*fb,GtkWidget*w,int tox,int toy,int towidth,int toheight)
{

    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    GtkWidget*widget=GTK_WIDGET(fb);
    FloatBorderChild*fbchild=get_child_by_widget(fb,w);
    

    if(fbchild==NULL)
        g_warning(" The Widget you specified is not contained by FloatBorder.");

    fbchild->x=tox;
    fbchild->y=toy;
   
    fbchild->position[REF_CORNER].x=towidth;
    fbchild->position[REF_CORNER].y=toheight;

    gtk_widget_queue_resize(widget);


}

static gboolean counter_children(FloatBorderChild*fbc,gpointer d)
{

    int* n=(int*)d;

    (*n)++;
    return FALSE;
}


static GdkWindow* create_hwin(FloatBorderChild* fbc, gint type)
{


    GtkWidget*widget=GTK_WIDGET(fbc->floatborder);

    GdkWindow*parent_win;
    GdkWindow*return_win;
    GdkDisplay* display;

    GdkWindowAttr attributes;
    gint attributes_mask;

    parent_win=gtk_widget_get_window(widget);
//    g_message("window ::%x\n",parent_win);
    display=gtk_widget_get_display(widget);

    attributes.window_type=GDK_WINDOW_CHILD;
    attributes.wclass=GDK_INPUT_ONLY;
    attributes.event_mask=gtk_widget_get_events(widget);
    attributes.event_mask|=(GDK_BUTTON_PRESS_MASK|
            GDK_BUTTON_RELEASE_MASK|
            GDK_ENTER_NOTIFY_MASK|
            GDK_LEAVE_NOTIFY_MASK|
            GDK_POINTER_MOTION_MASK);
    attributes_mask=GDK_WA_X|GDK_WA_Y;

    attributes.x=0;//fbc->width+allocation.x;
    attributes.y=0;//allocation.y;
    attributes.width=1;//HWND_THICKNESS;
    attributes.height=1;//fbc->height;


    switch(type){

        case HWND_RIGHT:

    attributes.cursor=gdk_cursor_new_for_display(display,GDK_SB_H_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;
 
        case HWND_BOTTOM:

    attributes.cursor=gdk_cursor_new_for_display(display, GDK_SB_V_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;

        case HWND_LEFT:

    attributes.cursor=gdk_cursor_new_for_display( display, GDK_SB_H_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;
 
        case HWND_TOP:

    attributes.cursor=gdk_cursor_new_for_display(display ,GDK_SB_V_DOUBLE_ARROW);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;

        case HWND_BL_RESIZE:

    attributes.cursor=gdk_cursor_new_for_display(display, GDK_BOTTOM_LEFT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;

        case HWND_BR_RESIZE:

    attributes.cursor=gdk_cursor_new_for_display(display, GDK_BOTTOM_RIGHT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    return_win=gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;

        case HWND_TL_RESIZE:

    attributes.cursor=gdk_cursor_new_for_display(display, GDK_TOP_LEFT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;

        case HWND_TR_RESIZE:

    attributes.cursor=gdk_cursor_new_for_display(display, GDK_TOP_RIGHT_CORNER);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

        break;
        case HWND_MOVE:

    attributes.cursor=gdk_cursor_new_for_display(display, GDK_FLEUR);
    attributes_mask|=GDK_WA_CURSOR;

    return_win =gdk_window_new(parent_win,&attributes,attributes_mask);
    gtk_widget_register_window(widget,return_win);
    g_object_unref(attributes.cursor);

    }


    return return_win;

    
}





static void dispose_hwnds(FloatBorderChild*fbchild)
{
    
    if(!gtk_widget_get_realized(GTK_WIDGET(fbchild->floatborder)))
        return;
    int i;
    for(i=1;i<N_HWNDS;i++){
    
        if(fbchild->hwnded[i]){
            if(!fbchild->hwnds[i]){
                fbchild->hwnds[i]=create_hwin(fbchild,i);
            }
                gdk_window_show(fbchild->hwnds[i]);
        
        }else{
            if(fbchild->hwnds[i]){
                gdk_window_hide(fbchild->hwnds[i]);
            }
        }
    }
}





void float_border_set_hwnds(FloatBorder*fb, GtkWidget*w,...)
{   
   
    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);
    

    int i;
    for(i=0;i<N_HWNDS;i++){
        fbchild->hwnded[i]=FALSE;
    }
 
    va_list ap;
    va_start(ap,w);

    int hwnd_on;
    while((hwnd_on=va_arg(ap,int))){
        fbchild->hwnded[hwnd_on]=TRUE;
    }
    va_end(ap);

    dispose_hwnds(fbchild);

}



void float_border_add_hwnds(FloatBorder*fb, GtkWidget*w,...)
{

    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);

 
    va_list ap;
    va_start(ap,w);

    int hwnd_on;
    while((hwnd_on=va_arg(ap,int))){
        fbchild->hwnded[hwnd_on]=TRUE;
    }
    va_end(ap);

    dispose_hwnds(fbchild);


}

void float_border_remove_hwnds(FloatBorder*fb, GtkWidget*w,...)
{

    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);

 
    va_list ap;
    va_start(ap,w);

    int hwnd_on;
    while((hwnd_on=va_arg(ap,int))){
        fbchild->hwnds[hwnd_on]=FALSE;
    }
    va_end(ap);

    dispose_hwnds(fbchild);


}




int float_border_count(FloatBorder*fb)
{

    int num_of_children=0;

    for_all_children(fb,counter_children,&num_of_children);
    
    return num_of_children;

}



void float_border_reorder(FloatBorder*fb, GtkWidget*w, GtkWidget*sibling, gboolean above)
{


    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(sibling));


    FloatBorderPriv*priv=fb->priv;

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);
    FloatBorderChild*sibchild=get_child_by_widget(fb,sibling);

    priv->children=g_list_remove(priv->children,fbchild);
    int pos=g_list_index(priv->children,sibchild);
    g_print("Pos   ::%d\n",pos);


    if(above)
        priv->children=g_list_insert(priv->children,fbchild,pos+1);
    else
        priv->children=g_list_insert(priv->children,fbchild,pos);


    for_all_children(fb,unmap_hwnds,fb);
    for_all_children(fb,map_hwnds,fb);
    gtk_widget_queue_resize(GTK_WIDGET(fb));


}




static gboolean set_active(FloatBorderChild*fbc,gpointer d)
{

    gboolean set= GPOINTER_TO_INT(d);

    if(fbc->active==set)
        return FALSE;
    
    int i;
        for(i=1; i< N_HWNDS; i++){
        if(fbc->hwnds[i]){
            fbc->hwnded[i]=set;
        }
        }
        fbc->active=set;
        dispose_hwnds(fbc);

        return FALSE;

}






void float_border_set_active(FloatBorder*fb,GtkWidget*w,gboolean set)
{

    g_return_if_fail(IS_FLOAT_BORDER(fb));
    g_return_if_fail(GTK_WIDGET(fb) == gtk_widget_get_parent(w));

    FloatBorderChild*fbchild=get_child_by_widget(fb,w);
    set_active(fbchild,GINT_TO_POINTER(set));


}




void float_border_show_border(FloatBorder*fb,gboolean show)
{

    FloatBorderPriv* priv=fb->priv;
    priv->show_border=show;


    gtk_widget_queue_resize(GTK_WIDGET(fb));
}





static void fb_set_frozen(FloatBorder*fb,gboolean set)
{

//    g_message("Set Frozen>>> [%d]",set);
    
    FloatBorderPriv* priv=fb->priv;
    if(priv->frozen==set)
        return;
    priv->frozen=set;

    for_all_children(fb, set_active,GINT_TO_POINTER(!set));

}


static void fb_set_may_shrink(FloatBorder*fb,gboolean set)
{

    FloatBorderPriv* priv=fb->priv;
    if(priv->may_overlap==set)
        return;
    priv->may_overlap=set;


}









GtkWidget* float_border_new(gboolean frozen,gboolean shrink)
{
return (GtkWidget*)g_object_new(TYPE_FLOAT_BORDER,"frozen",frozen,"may-shrink",shrink,NULL);
}
