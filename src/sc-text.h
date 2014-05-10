#ifndef _SC_H_TEXT_
#define _SC_H_TEXT_


#include"sc-canvas.h"
#include"sc-operable.h"
#include<gtk/gtk.h>
#include"floatBorder/floatBorder.h"


#define SC_TYPE_TEXT (sc_text_get_type())
#define SC_TEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_TEXT,SCText))
#define SC_IS_TEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_TEXT))





typedef struct _SCText{

    FloatBorder parent;
    SCCanvas*canvas;

    GdkWindow* event_window;
    GtkWidget* text_view;


//    int text_type;
    GdkRectangle position;

    //font  color
    GdkRGBA color;


    guint pressed:1;
    guint has_text:1;
    guint is_focus:1;


}SCText;



typedef struct _SCTextClass{

    FloatBorderClass parent_class;



}SCTextClass;







GType sc_text_get_type(void);


SCOperable* sc_text_new(SCCanvas*canvas);

void sc_text_reset(SCText*text);
gboolean sc_text_has_view(SCText*text);







#endif
