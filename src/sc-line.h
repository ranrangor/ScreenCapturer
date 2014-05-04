#ifndef _SC_H_LINE_

#define _SC_H_LINE_



#include<gtk/gtk.h>


#define SC_TYPE_LINE (sc_line_get_type())
#define SC_LINE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_LINE,SCLine))
#define SC_IS_LINE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_LINE))





typedef struct _SCLine{

    GtkWidget parent;

    GdkWindow* event_window;

    int line_type;
//    GdkRectangle rectangle;
    int x0,y0;
    int x1,y1;

    GdkRGBA color;
    gint line_width;

    gboolean pressed;


}SCLine;



typedef struct _SCLineClass{

    GtkWidgetClass parent_class;



}SCLineClass;







GType sc_line_get_type(void);





#endif
