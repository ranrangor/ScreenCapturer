#include<gtk/gtk.h>
#include"floatBorder.h"


static GtkWidget*widget0;
static GtkWidget*widget1;
static GtkWidget*widget2;
static GtkWidget*widget3;
static GtkWidget*widget4;


static gboolean seted=TRUE;


static gboolean press_cb(GtkWidget*w,GdkEventButton*e,gpointer d)
{

    if(gtk_widget_get_window(GTK_WIDGET(d))==e->window){
    widget4=gtk_button_new_with_label("000");
    gtk_widget_show(widget4);
    float_border_put_with_size(FLOAT_BORDER(d),widget4,(int)e->x,(int)e->y,100,100);

    return TRUE;
    }else{
    return FALSE;
    }
}



static void setorder_cb(GtkWidget*w, FloatBorder*fb)
{
    g_message("Set ReOrder..");

    float_border_reorder(fb,widget0,widget2,FALSE);

}

static void sethwnds_cb(GtkWidget*w, FloatBorder*fb)
{
float_border_set_hwnds(fb,widget3,HWND_MOVE,HWND_BR_RESIZE,HWND_RIGHT,HWND_BOTTOM,NULL);


}
static void setact_cb(GtkWidget*w, FloatBorder*fb)
{

    seted=!seted;

    float_border_set_active(fb,widget1,seted);


}


static void print_cb(GtkWidget*w, FloatBorder*fb)
{

    seted=!seted;
    g_object_set(fb,"frozen",seted,NULL);

}



static void resize_cb(GtkWidget*w,FloatBorder*fb)
{

    int num=float_border_count(fb);

    int width=100,height=100;
    int x,y;

    float_border_get_current_size(fb,widget0,&width,&height);
    float_border_get_current_position(fb,widget0,&x,&y);
    g_message("Origin  [width:%d, height:%d]",width,height);

//    float_border_resize(fb,w,width+5,height+5);
//    float_border_move(fb,w,x+2,y+5);
    float_border_move_resize(fb,widget0,x-1,y-1,width+5,height+5);

}

static void remove_cb(GtkWidget*w,FloatBorder*fb)
{

    g_message("remove_cb");

    float_border_remove(fb,widget2);

}


static void hide_cb(GtkWidget*w,FloatBorder*fb)
{

    float_border_show_border(fb,seted);
    seted=!seted;

}



static gboolean win_draw(GtkWidget*widget,cairo_t*cr,gpointer d)
{

    cairo_set_source_rgba(cr,0.7,0,0,1);

    cairo_paint(cr);

    return FALSE;

}


int main(int argc,char**argv)
{

    gtk_init(&argc,&argv);




    GtkWidget*win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_app_paintable(win,TRUE);

    gtk_widget_set_size_request(win,400,400);


    GtkWidget*vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    GtkWidget*hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);


    widget0=gtk_button_new_with_label("BuTToN0");
    widget1=gtk_button_new_with_label("BuTToN11");
    widget2=gtk_entry_new();
    widget3=gtk_text_view_new();


    gtk_widget_set_size_request(widget3,100,100);
    GtkWidget*button1=gtk_button_new_with_label("Resize");
    GtkWidget*button2=gtk_button_new_with_label("Frozen?");
    GtkWidget*button3=gtk_button_new_with_label("Active?");
    GtkWidget*button4=gtk_button_new_with_label("Remove");
    GtkWidget*button5=gtk_button_new_with_label("HWNDS");
    GtkWidget*button6=gtk_button_new_with_label("REOrder");
    GtkWidget*button7=gtk_button_new_with_label("HideBorder");


    GtkWidget* fb=float_border_new(FALSE,TRUE);
    gtk_widget_set_has_window(fb,TRUE);

    g_message("FloatBorder::%x::",fb);

    gtk_box_pack_start(GTK_BOX(hbox),button1,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(hbox),button2,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(hbox),button3,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(hbox),button4,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(hbox),button5,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(hbox),button6,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(hbox),button7,FALSE,FALSE,5);

    gtk_box_pack_start(GTK_BOX(vbox),fb,TRUE,TRUE,5);
   
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);

    gtk_container_add(GTK_CONTAINER(win),vbox);

    
    float_border_put(FLOAT_BORDER(fb),widget0,100,100);
    float_border_put_with_size(FLOAT_BORDER(fb),widget1,150,300,200,200);
    float_border_put(FLOAT_BORDER(fb),widget2,300,100);

    float_border_put(FLOAT_BORDER(fb),widget3,200,80);

    gtk_widget_grab_focus(widget3);

    GdkRGBA trans={0,0,0,0};
    gtk_widget_override_background_color(widget3,0,&trans);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(widget3),GTK_WRAP_CHAR);

    g_signal_connect(G_OBJECT(win),"draw",G_CALLBACK(win_draw),NULL);

    g_signal_connect(G_OBJECT(button1),"clicked",G_CALLBACK(resize_cb),fb);
    g_signal_connect(G_OBJECT(button2),"clicked",G_CALLBACK(print_cb),fb);
    g_signal_connect(G_OBJECT(button3),"clicked",G_CALLBACK(setact_cb),fb);
    g_signal_connect(G_OBJECT(button4),"clicked",G_CALLBACK(remove_cb),fb);
    g_signal_connect(G_OBJECT(button5),"clicked",G_CALLBACK(sethwnds_cb),fb);
    g_signal_connect(G_OBJECT(button6),"clicked",G_CALLBACK(setorder_cb),fb);
    g_signal_connect(G_OBJECT(button7),"clicked",G_CALLBACK(hide_cb),fb);
    g_signal_connect(G_OBJECT(fb),"button_press_event",G_CALLBACK(press_cb),fb);

//    float_border_set_hwnds(fb,entry,HWND_MOVE,HWND_BR_RESIZE,NULL);

    
    gtk_widget_show_all(win);

    gtk_main();
    
    return 0;




}
