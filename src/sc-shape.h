#ifndef _SC_H_SHAPE_

#define _SC_H_SHAPE_

#include"sc-canvas.h"
#include"sc-operable.h"
#include<gtk/gtk.h>


#define SC_TYPE_SHAPE (sc_shape_get_type())
#define SC_SHAPE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_SHAPE,SCShape))
#define SC_IS_SHAPE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_SHAPE))





typedef struct _SCShape{

    GtkWidget parent;
    SCCanvas* canvas;

    GdkWindow* event_window;

    int shape_type;
    GdkRectangle rectangle;
    int line_width;
    GdkRGBA color;

    gboolean pressed;



}SCShape;



typedef struct _SCShapeClass{

    GtkWidgetClass parent_class;



}SCShapeClass;







GType sc_shape_get_type(void);


SCOperable* sc_shape_new(SCCanvas*canvas);

void sc_shape_reset(SCShape*shape);


#endif
