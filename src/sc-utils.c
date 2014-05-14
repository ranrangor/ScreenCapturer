#include<gtk/gtk.h>
#include"sc-utils.h"
#include"../icons/icons.h"




static void sc_button_set_style(GtkButton* button)
{

GtkCssProvider* provider;
GtkStyleContext*context;
provider=gtk_css_provider_new();
context=gtk_widget_get_style_context(GTK_WIDGET(button));
 
gtk_style_context_add_provider(context,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);
 
gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(provider),"GtkButton {border-radius:0px ;padding:0}",-1,NULL);

g_object_unref(provider);

}


GtkWidget*sc_image_button_new(const guint8* data)
{

    GtkWidget*image;
    GtkWidget*button=gtk_button_new();

    GdkPixbuf*pxf=gdk_pixbuf_new_from_inline(-1,data,FALSE,NULL);


    if(!pxf){
    image=gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_BUTTON);

    }else{
    image=gtk_image_new_from_pixbuf(pxf);
    }
    g_object_unref(pxf);

    gtk_button_set_image(GTK_BUTTON(button),image);
    gtk_button_set_always_show_image(GTK_BUTTON(button),TRUE);
    
    sc_button_set_style(GTK_BUTTON(button));


    return button;

}




GtkWidget*sc_image_button_new_by_size(const guint8* data,int siz)
{


    GtkWidget*image;
    GtkWidget*button=gtk_button_new();

    GdkPixbuf*pxf=gdk_pixbuf_new_from_inline(-1,data,FALSE,NULL);
    GdkPixbuf*real_pxf;


    if(!pxf){
    image=gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_BUTTON);

    }else{
    real_pxf=gdk_pixbuf_scale_simple(pxf,siz,siz,GDK_INTERP_HYPER);
    g_object_unref(pxf);
    image=gtk_image_new_from_pixbuf(real_pxf);
    g_object_set(image,"pixel-size",GTK_IMAGE_PIXBUF,NULL);
    }
    g_object_unref(real_pxf);

    gtk_button_set_image(GTK_BUTTON(button),image);
    gtk_button_set_always_show_image(GTK_BUTTON(button),TRUE);
    
    sc_button_set_style(GTK_BUTTON(button));


    return button;


}
