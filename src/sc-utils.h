#ifndef _SC_H_UTILTS_
#define _SC_H_UTILTS_

#include"../icons/icons.h"



GtkWidget*sc_image_button_new(const guint8* data);
GtkWidget*sc_image_button_new_by_size(const guint8* data,int siz);

GtkWidget*sc_image_toggle_button_new_by_size(const guint8* data,int siz);

GtkWidget*sc_image_text_item_new(const guint8*data,const char*text,int siz);


GtkWidget*sc_image_new(const guint8* data);
GtkWidget*sc_image_new_by_size(const guint8* data,int width,int height);


char* get_pixel (GdkPixbuf *pixbuf, int x, int y, GdkRGBA *color);

















#endif
