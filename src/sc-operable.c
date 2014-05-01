
#include"sc-operable.h"






G_DEFINE_INTERFACE (SCOperable, sc_operble, GTK_TYPE_WIDGET);





static void
sc_operable_default_init(SCOperableInterface*iface)
{








}




void sc_operable_ready(SCOperable* operable)
{

    g_return_if_fail(SC_IS_OPERABLE(operable),NULL);

    SC_OPERABLE_GET_INTERFACE(operble)->ready(operable);


}




void sc_operable_done(SCOperable* operable)
{
    g_return_if_fail(SC_IS_OPERABLE(operable),NULL);

    SC_OPERABLE_GET_INTERFACE(operble)->done(operable);


}

GtkWidget* sc_operable_obtain_menu(SCOperable*operable)
{
    g_return_if_fail(SC_IS_OPERABLE(operable),NULL);

    SC_OPERABLE_GET_INTERFACE(operble)->obtain_menu(operable);

}
