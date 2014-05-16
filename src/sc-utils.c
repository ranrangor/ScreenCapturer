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
    g_object_unref(pxf);
    }

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




GtkWidget*sc_image_toggle_button_new_by_size(const guint8* data,int siz)
{


    GtkWidget*image;
    GtkWidget*button=gtk_toggle_button_new();

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







GtkWidget*sc_image_text_item_new(const guint8*data,const char*text,int siz)
{


    GtkWidget*image;
    GtkWidget*item=gtk_menu_item_new();
    GtkWidget*box=gtk_grid_new();//gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);


    GtkWidget*label=gtk_label_new(text);
    gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_RIGHT);

    gtk_widget_set_hexpand(label,TRUE);
    gtk_widget_set_halign(label,GTK_ALIGN_START);

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

//    gtk_widget_set_hexpand(image,FALSE);
//    gtk_widget_set_halign(image,GTK_ALIGN_START);
    

    gtk_grid_attach(GTK_GRID(box),image,0,0,1,1);
    gtk_grid_attach(GTK_GRID(box),label,1,0,1,1);

    gtk_grid_set_column_spacing(GTK_GRID(box),20);

//    gtk_box_pack_start(GTK_BOX(box),image,TRUE,FALSE,0);
//    gtk_box_pack_end(GTK_BOX(box),label,TRUE,TRUE,0);

    gtk_container_add(GTK_CONTAINER(item),box);
    gtk_container_set_border_width(GTK_CONTAINER(box),0);
    gtk_container_set_border_width(GTK_CONTAINER(item),0);

    return item;

}



GtkWidget*sc_image_new(const guint8* data)
{

    GtkWidget*image;

    GdkPixbuf*pxf=gdk_pixbuf_new_from_inline(-1,data,FALSE,NULL);


    if(!pxf){
    image=gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_BUTTON);

    }else{
    image=gtk_image_new_from_pixbuf(pxf);
    g_object_unref(pxf);
    }

    
    return image;

}


GtkWidget*sc_image_new_by_size(const guint8* data,int width,int height)
{


    GtkWidget*image;

    GdkPixbuf*pxf=gdk_pixbuf_new_from_inline(-1,data,FALSE,NULL);
    GdkPixbuf*real_pxf;


    if(!pxf){
    image=gtk_image_new_from_icon_name("image-x-generic",GTK_ICON_SIZE_BUTTON);

    }else{
    real_pxf=gdk_pixbuf_scale_simple(pxf,width,height,GDK_INTERP_HYPER);
    g_object_unref(pxf);
    image=gtk_image_new_from_pixbuf(real_pxf);
    g_object_set(image,"pixel-size",GTK_IMAGE_PIXBUF,NULL);
    }
    g_object_unref(real_pxf);



    return image;


}








