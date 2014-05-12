#ifndef _SC_H_PAINTER_

#define _SC_H_PAINTER_



#include"sc-canvas.h"
#include"sc-operable.h"
#include<gtk/gtk.h>


#define SC_TYPE_PAINTER (sc_painter_get_type())
#define SC_PAINTER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_PAINTER,SCPainter))
#define SC_IS_PAINTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_PAINTER))




typedef struct _point{
int x;
int y;
}point;



point*new_point(int x,int y);

void destroy_point(point*p);







typedef struct _SCPainter{

    GtkWidget parent;
    SCCanvas* canvas;

    GdkWindow* event_window;

    GList*points;
    int line_width;
    GdkRGBA color;

    GtkWidget*colorchooser;
    GtkWidget*widthsetter;


    gboolean pressed;



}SCPainter;



typedef struct _SCPainterClass{

    GtkWidgetClass parent_class;



}SCPainterClass;







GType sc_painter_get_type(void);

SCOperable* sc_painter_new(SCCanvas*canvas);




#endif
