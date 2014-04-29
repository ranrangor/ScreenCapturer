#ifndef _SC_H_APP_
#define _SC_H_APP_

#include <gtk/gtk.h>


#define SC_APP_TYPE (sc_app_get_type ())
#define SC_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_APP_TYPE, SCApp))


typedef struct _SCApp       SCApp;
typedef struct _SCAppClass  SCAppClass;


GType sc_app_get_type    (void);
SCApp* sc_app_new         (void);



#endif 

