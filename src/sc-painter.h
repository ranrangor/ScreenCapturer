#ifndef _SC_H_PAINTER_

#define _SC_H_PAINTER_



#include<gtk/gtk.h>


#define SC_TYPE_PAINTER (sc_painter_get_type())
#define SC_PAINTER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_PAINTER,SCPainter))
#define SC_IS_PAINTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_PAINTER))





typedef struct _SCPainter{

    GtkWidget parent;

    GdkWindow* event_window;

    int shape_type;
    GdkRectangle rectangle;
    int line_width;
    GdkRGBA color;

    gboolean pressed;



}SCPainter;



typedef struct _SCPainterClass{

    GtkWidgetClass parent_class;



}SCPainterClass;







GType sc_painter_get_type(void);





#endif
