#ifndef _SC_H_OPERABLE_

#define _SC_H_OPERABLE_



#include<gtk/gtk.h>
#include"sc-canvas.h"

#define SC_TYPE_OPERABLE (sc_operable_get_type())
#define SC_OPERABLE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_OPERABLE,SCOperable))
#define SC_IS_OPERABLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_OPERABLE))

#define SC_OPERABLE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst),SC_TYPE_OPERABLE,SCOperableInterface))



typedef struct _SCCanvas SCCanvas;


typedef struct _scOperable SCOperable;



typedef struct _scOperableIface{

    GTypeInterface parent_iface;

    /* member */
    SCCanvas* canvas;
    GtkWidget* toolmenu;


    /* virtual functions */
    GtkWidget* (*obtain_toolmenu) (SCOperable* operable);


}SCOperableInterface;





GType sc_operable_get_type(void);









GtkWidget* sc_operable_obtain_toolmenu(SCOperable* operable);

void sc_operable_set_canvas(SCOperable*operable,SCCanvas*canvas);
SCCanvas* sc_operable_get_canvas(SCOperable*operable);

#endif
