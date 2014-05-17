
#ifndef _SC_H_WINDOW_

#define _SC_H_WINDOW_


#include<gtk/gtk.h>

#include"sc-app.h"



#define SC_TYPE_WINDOW (sc_window_get_type())
#define SC_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_WINDOW,SCWindow))

#define SC_WINDOW_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SC_TYPE_WINDOW,SCWindowPriv))




typedef struct _SCWindowPriv SCWindowPriv;


typedef struct _SCWindow{

    GtkApplicationWindow parent;
    
    SCWindowPriv*priv;


}SCWindow;



typedef struct _SCWindowClass{

    GtkApplicationWindowClass parent_class;
    


}SCWindowClass;








GType sc_window_get_type(void);



SCWindow* sc_window_new(SCApp*app);
GdkPixbuf*sc_window_get_pixbuf(SCWindow*win);

void sc_window_exit(SCWindow*win);
void sc_window_reselect(SCWindow*win);
gboolean sc_window_is_selected(SCWindow*win);

void sc_window_get_size(SCWindow*win,int *width, int* height);
//void sc_window_open(SCWindow* win, GFile* file);



#endif

