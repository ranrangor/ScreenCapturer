#include <gtk/gtk.h>

#include "sc-app.h"
#include "sc-window.h"

struct _SCApp {
    GtkApplication parent;
};

struct _SCAppClass {
    GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(SCApp, sc_app, GTK_TYPE_APPLICATION);

static void sc_app_init(SCApp * app)
{
}



static void exit_action_cb(GSimpleAction*simple,GVariant* parameter, gpointer user_data)
{
    
    g_message("win.exit Invoked");

    GApplication*application=G_APPLICATION(user_data);
 
    g_application_quit (application);

}



static void sc_app_activate(GApplication * app)
{
    SCWindow *scwin;

    scwin = sc_window_new(SC_APP(app));


    GSimpleAction*exit_action=g_simple_action_new("exit",NULL);

    g_signal_connect(exit_action,"activate",G_CALLBACK(exit_action_cb),app);

    g_action_map_add_action(G_ACTION_MAP(app),G_ACTION(exit_action));

    gtk_application_add_accelerator(GTK_APPLICATION(app),"Escape","app.exit",NULL);









    gtk_window_present(GTK_WINDOW(scwin));


}

static void
sc_app_open(GApplication * app,
		 GFile ** files, gint n_files, const gchar * hint)
{
    GList *windows;
    SCWindow *win;
    int i;

    windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows)
	win = SC_WINDOW(windows->data);
//    else
//	win = sc_window_new();

//    for (i = 0; i < n_files; i++)
//	sc_window_open(win, files[i]);


    gtk_window_present(GTK_WINDOW(win));
}

static void sc_app_class_init(SCAppClass * class)
{
    G_APPLICATION_CLASS(class)->activate = sc_app_activate;
//    G_APPLICATION_CLASS(class)->open = sc_app_open;
}

SCApp *sc_app_new(void)
{
    return g_object_new(SC_APP_TYPE,
			"application-id", "org.gtk.screencapturer",
			"flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
