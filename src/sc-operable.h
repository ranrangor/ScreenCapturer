#ifndef _SC_H_OPERABLE_

#define _SC_H_OPERABLE_



#include<gtk/gtk.h>

#define SC_TYPE_OPERABLE (sc_operable_get_type())
#define SC_OPERABLE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_OPERABLE,SCOperable))
#define SC_IS_OPERABLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_OPERABLE))

#define SC_OPERABLE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst),SC_TYPE_OPERABLE,SCOperableInterface))







typedef struct _scOperable SCOperable;


typedef struct _scOperableIface{

    GTypeInterface parent_iface;

    void (*ready)(SCOperable* operable);
    void (*done) (SCOperable* operable);
    void (*obtain_menu) (SCOperable* operable);


}SCOperableInterface;







static GType sc_operable_get_type(void);









void sc_operable_ready(SCOperable* operable);
void sc_operable_done(SCOperable* operable);
void sc_operable_obtain_menu(SCOperable* operable);


#endif
