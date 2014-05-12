#include"sc-text.h"
#include"sc-canvas.h"
#include"sc-operable.h"
#include"floatBorder/floatBorder.h"
#include<math.h>
#include<gtk/gtk.h>







static void sc_operable_interface_init(SCOperableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(SCText,sc_text,TYPE_FLOAT_BORDER,
        G_IMPLEMENT_INTERFACE(SC_TYPE_OPERABLE,sc_operable_interface_init))






static gboolean sc_text_press(GtkWidget*widget, GdkEventButton*e);



GtkWidget*text_obtain_menu(SCOperable*operable)
{

    GtkWidget*box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
    GtkWidget*but_2=gtk_button_new_with_label("2");
    GtkWidget*but_5=gtk_button_new_with_label("5");

//    g_signal_connect(G_OBJECT(but_2),"clicked",G_CALLBACK(but2_clicked),operable);
//    g_signal_connect(G_OBJECT(but_5),"clicked",G_CALLBACK(but5_clicked),operable);


    gtk_box_pack_start(GTK_BOX(box),but_2,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),but_5,FALSE,FALSE,0);

    gtk_widget_show(but_2);
    gtk_widget_show(but_5);
    gtk_widget_show(box);

    return box;



}

static void sc_operable_interface_init(SCOperableInterface* iface)
{

    iface->toolbutton=gtk_button_new_with_label("text");

    iface->obtain_menu=text_obtain_menu;

}




static void sc_text_class_init(SCTextClass*klass)
{


    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->button_press_event=sc_text_press;
//    wclass->button_release_event=sc_text_release;
//    wclass->motion_notify_event=sc_text_motion;


}


static GtkWidget* new_tview()
{
    GtkWidget*tview;

    tview=gtk_text_view_new();
    
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tview),GTK_WRAP_NONE);
    GdkRGBA opacity={0,};
    gtk_widget_override_background_color(tview,0,&opacity);


    return tview;

}


static void sc_text_init(SCText*obj)
{

    GtkWidget* wobj=GTK_WIDGET(obj);

//    priv->text_view=new_tview();
   
    gtk_widget_set_has_window(wobj,TRUE);

    obj->color.red=1;
    obj->color.alpha=1;

}


static gboolean sc_text_press(GtkWidget*widget, GdkEventButton*e)
{
    g_message("oooooooooooooooooooooooooooooooooooooooooooooooooooooo");

    FloatBorder*fb=FLOAT_BORDER(widget);
    GdkWindow*window=gtk_widget_get_window(widget);

    SCText*text=SC_TEXT(widget);

    if(e->button==GDK_BUTTON_SECONDARY)
        return FALSE;

    if(e->window==window){
        text->position.x=(int)e->x;
        text->position.y=(int)e->y;

        float_border_show_border(fb,FALSE);
        gtk_text_view_set_cursor_visible(text->text_view,FALSE);
//forcing hide border,Before step_done().        
        while(gtk_events_pending()){
            gdk_flush();//before gtk_main_iteration()
            gtk_main_iteration();
        }

        if(sc_text_has_view(text)){
            SCCanvas* canvas=sc_operable_get_canvas(SC_OPERABLE(widget));

            if(sc_text_is_focus(text)){
                gdk_flush();//
                sc_canvas_step_done(canvas);
                text->is_focus=FALSE;
                
                if(text->text_view){
                    float_border_remove(widget,text->text_view);
                    text->text_view=NULL;
                }
            }else{
                sc_text_reset(SC_TEXT(widget));
            } 
        }else{
            sc_text_reset(SC_TEXT(widget));
        
        }

        return TRUE;
    }

    GTK_WIDGET_CLASS(sc_text_parent_class)->button_press_event(widget,e);
//        return FALSE;
    


}


gboolean sc_text_has_view(SCText*text)
{

    return (text->text_view!=NULL)?TRUE:FALSE;

}
 
gboolean sc_text_is_focus(SCText*text)
{

    return (text->is_focus!=FALSE)?TRUE:FALSE;

}
   

void sc_text_reset(SCText*text)
{
    
    FloatBorder*fb=FLOAT_BORDER(text);
   
//    if(text->text_view)
//        float_border_remove(fb,text->text_view);

    float_border_show_border(fb,TRUE);

    text->text_view=new_tview();
    text->is_focus=TRUE;
    gtk_widget_show(text->text_view);
//    gtk_widget_grab_focus(text->text_view);

    float_border_put_with_size(fb,text->text_view,text->position.x,text->position.y,50,22);

    g_message("Reset..");
}




SCOperable* sc_text_new(SCCanvas*canvas)
{
    SCOperable*operable=(SCOperable*)g_object_new(SC_TYPE_TEXT,NULL);
    sc_operable_set_canvas(operable,canvas);

    return operable;

}


static void text_view_buffer_init(SCText*text)
{

    GtkTextIter iter_start,iter_end;
    GtkTextTag*tag=gtk_text_tag_new("tt");


    GtkTextBuffer*buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text->text_view));
    gtk_text_buffer_get_start_iter(buffer,&iter_start);
    gtk_text_buffer_get_end_iter(buffer,&iter_end);




}

void sc_text_set_font_size(SCText*text, guint size)
{

//    GtkTextBuffer*buffer=gtk_text_view_get_buffer(text->text_view);




}











