
#ifndef _SC_H_COLORCHOOSER_

#define _SC_H_COLORCHOOSER_


#include<gtk/gtk.h>



#define SC_TYPE_COLOR_CHOOSER (sc_color_chooser_get_type())
#define SC_COLOR_CHOOSER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_COLOR_CHOOSER,SCColorChooser))
#define SC_COLOR_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),SC_TYPE_COLOR_CHOOSER,SCColorChooserClass))

#define SC_COLOR_CHOOSER_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SC_TYPE_COLOR_CHOOSER,SCColorChooserPriv))






typedef struct _SCColorChooserPriv SCColorChooserPriv;

typedef struct _SCColorChooser{

GtkWidget parent;

SCColorChooserPriv* priv;

}SCColorChooser;




typedef struct _SCColorChooserClass{

GtkWidgetClass parent_class;


}SCColorChooserClass;







GType sc_color_chooser_get_type(void);


GtkWidget*sc_color_chooser_new(void);

char* sc_color_chooser_get_color(SCColorChooser*);



#endif
