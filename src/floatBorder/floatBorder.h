#ifndef _H_FLOAT_BORDER_
#define _H_FLOAT_BORDER_



#include<gtk/gtk.h>






#define TYPE_FLOAT_BORDER (float_border_get_type())
#define FLOAT_BORDER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),TYPE_FLOAT_BORDER,FloatBorder))
#define FLOAT_BORDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),TYPE_FLOAT_BORDER,FloatBorderClass))

#define IS_FLOAT_BORDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),TYPE_FLOAT_BORDER))
#define IS_FLOAT_BORDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),TYPE_FLOAT_BORDER))

#define FLOAT_BODER_GET_CLASS(obj) (G_TYPE_CHECK_INSTANCE_GET_CLASS((obj),TYPE_FLOAT_BORDER,FloatBorderClass))
#define FLOAT_BODER_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),TYPE_FLOAT_BORDER,FloatBorderPriv))



enum{
    HWND_0,//for padding,init_fbchild();
HWND_MOVE,//TOP_LEFT,
HWND_BR_RESIZE,
HWND_TR_RESIZE,
HWND_BL_RESIZE,
HWND_TL_RESIZE,
HWND_RIGHT,
HWND_BOTTOM,
HWND_TOP,
HWND_LEFT,
N_HWNDS

};





typedef struct floatBorderPriv FloatBorderPriv;

typedef struct floatBorder{

    GtkContainer parent;
    FloatBorderPriv*priv;

}FloatBorder;

typedef struct floatBorderClass{

    GtkContainerClass parent_class;

    void (*for_reserved0) (void);
    void (*for_reserved1) (void);


}FloatBorderClass;








GType float_border_get_type(void);



GtkWidget* float_border_new(gboolean frozen,gboolean shrink);

void float_border_put(FloatBorder*fb,GtkWidget*w,int x,int y);
void float_border_put_with_size(FloatBorder*fb,GtkWidget*w,int x,int y,int width,int height);
void float_border_remove(FloatBorder*fb,GtkWidget*w);

void float_border_get_current_size(FloatBorder*fb,GtkWidget*w,int* width,int* height);
void float_border_get_current_position(FloatBorder*fb,GtkWidget*w,int* x,int* y);
int float_border_count(FloatBorder*fb);

void float_border_move(FloatBorder*fb,GtkWidget*w,int x,int y);
void float_border_resize(FloatBorder*fb,GtkWidget*w,int width,int height);
void float_border_move_resize(FloatBorder*fb,GtkWidget*w,int x,int y,int width,int height);


void float_border_set_active(FloatBorder*fb,GtkWidget*w,gboolean set);

void float_border_reorder(FloatBorder*fb, GtkWidget*w, GtkWidget*sibling, gboolean above);

void float_border_show_border(FloatBorder*fb,gboolean show);

void float_border_set_hwnds(FloatBorder*fb, GtkWidget*w,...);
void float_border_add_hwnds(FloatBorder*fb, GtkWidget*w,...);
void float_border_remove_hwnds(FloatBorder*fb, GtkWidget*w,...);







#endif
