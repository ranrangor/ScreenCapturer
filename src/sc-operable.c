
#include"sc-operable.h"
#include"sc-canvas.h"
#include<gtk/gtk.h>






G_DEFINE_INTERFACE (SCOperable, sc_operable, GTK_TYPE_WIDGET);





static void
sc_operable_default_init(SCOperableInterface*iface)
{





}



GtkWidget* sc_operable_obtain_toolmenu(SCOperable*operable)
{
    g_return_val_if_fail(SC_IS_OPERABLE(operable),NULL);

    return SC_OPERABLE_GET_INTERFACE(operable)->obtain_toolmenu(operable);

}


void sc_operable_set_canvas(SCOperable*operable,SCCanvas*canvas)
{

    SC_OPERABLE_GET_INTERFACE(operable)->canvas=canvas;

}


SCCanvas* sc_operable_get_canvas(SCOperable*operable)
{

    return SC_OPERABLE_GET_INTERFACE(operable)->canvas;

}






