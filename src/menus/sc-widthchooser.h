
#ifndef _SC_H_WIDTHCHOOSER_

#define _SC_H_WIDTHCHOOSER_


#include<gtk/gtk.h>



#define SC_TYPE_WIDTH_CHOOSER (sc_width_chooser_get_type())
#define SC_WIDTH_CHOOSER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_WIDTH_CHOOSER,SCWidthChooser))
#define SC_WIDTH_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),SC_TYPE_WIDTH_CHOOSER,SCWidthChooserClass))

#define SC_WIDTH_CHOOSER_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SC_TYPE_WIDTH_CHOOSER,SCWidthChooserPriv))







typedef struct _SCWidthChooserPriv SCWidthChooserPriv;

typedef struct _SCWidthChooser{

GtkWidget parent;

SCWidthChooserPriv* priv;

}SCWidthChooser;




typedef struct _SCWidthChooserClass{

GtkWidgetClass parent_class;


}SCWidthChooserClass;







GType sc_width_chooser_get_type(void);


GtkWidget*sc_width_chooser_new(int i);

int sc_width_chooser_get_width(SCWidthChooser*);



#endif
