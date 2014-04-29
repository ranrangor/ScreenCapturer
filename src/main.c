#include <gtk/gtk.h>
#include "sc-app.h"

int
main (int argc, char *argv[])
{
  return g_application_run (G_APPLICATION (sc_app_new()), argc, argv);
}


