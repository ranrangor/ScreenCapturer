
#include"sc-operable.h"






G_DEFINE_INTERFACE (SCOperable, sc_operable, GTK_TYPE_WIDGET);





static void
sc_operable_default_init(SCOperableInterface*iface)
{





}




void sc_operable_ready(SCOperable* operable)
{

    g_return_if_fail(SC_IS_OPERABLE(operable));

    SC_OPERABLE_GET_INTERFACE(operable)->ready(operable);


}




void sc_operable_done(SCOperable* operable)
{
    g_return_if_fail(SC_IS_OPERABLE(operable));

    SC_OPERABLE_GET_INTERFACE(operable)->done(operable);


}


void sc_operable_reset(SCOperable* operable)
{
    g_return_if_fail(SC_IS_OPERABLE(operable));

    SC_OPERABLE_GET_INTERFACE(operable)->reset(operable);


}






GtkWidget* sc_operable_obtain_menu(SCOperable*operable)
{
    g_return_val_if_fail(SC_IS_OPERABLE(operable),NULL);

    return SC_OPERABLE_GET_INTERFACE(operable)->obtain_menu(operable);

}


GtkWidget* sc_operable_get_toolbutton(SCOperable* operable)
{
    g_return_val_if_fail(SC_IS_OPERABLE(operable),NULL);

    return SC_OPERABLE_GET_INTERFACE(operable)->get_toolbutton(operable);



}







