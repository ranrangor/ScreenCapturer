
#ifndef _SC_H_SHAPECHOOSER_

#define _SC_H_SHAPECHOOSER_


#include<gtk/gtk.h>



#define SC_TYPE_SHAPE_CHOOSER (sc_shape_chooser_get_type())
#define SC_SHAPE_CHOOSER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_SHAPE_CHOOSER,SCShapeChooser))
#define SC_SHAPE_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),SC_TYPE_SHAPE_CHOOSER,SCShapeChooserClass))

#define SC_SHAPE_CHOOSER_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SC_TYPE_SHAPE_CHOOSER,SCShapeChooserPriv))




enum {
    SHAPE_RECT,
    SHAPE_CIRCLE,
    N_SHAPE
};






typedef struct _SCShapeChooserPriv SCShapeChooserPriv;

typedef struct _SCShapeChooser{

GtkWidget parent;

SCShapeChooserPriv* priv;

}SCShapeChooser;




typedef struct _SCShapeChooserClass{

GtkWidgetClass parent_class;


}SCShapeChooserClass;







GType sc_shape_chooser_get_type(void);


GtkWidget* sc_shape_chooser_new(int type);


int sc_shape_chooser_get_shape(SCShapeChooser*);



#endif
