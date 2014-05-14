#include"sc-text.h"
#include"sc-canvas.h"
#include"sc-operable.h"
#include"floatBorder/floatBorder.h"
#include"menus/sc-colorchooser.h"
#include"menus/sc-fontsizechooser.h"
#include<math.h>
#include<gtk/gtk.h>





static void sc_operable_interface_init(SCOperableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(SCText,sc_text,TYPE_FLOAT_BORDER,
        G_IMPLEMENT_INTERFACE(SC_TYPE_OPERABLE,sc_operable_interface_init))




static GtkTextBuffer* text_view_buffer_init(GtkTextView*view, SCText*text);
static void text_changed(GtkTextBuffer*buffer,gpointer d);

static gboolean sc_text_press(GtkWidget*widget, GdkEventButton*e);



GtkWidget*text_obtain_toolmenu(SCOperable*operable)
{

    SCText* text=SC_TEXT(operable);

    GtkWidget*box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    text->fontsizechooser=sc_fontsize_chooser_new(12);
    text->colorchooser=sc_color_chooser_new();

    GtkWidget*sep=gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    gtk_combo_box_set_focus_on_click(GTK_COMBO_BOX(text->fontsizechooser),TRUE);

    gtk_box_pack_start(GTK_BOX(box),text->fontsizechooser,FALSE,FALSE,2);
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),text->colorchooser,FALSE,FALSE,0);


    return box;



}

static void sc_operable_interface_init(SCOperableInterface* iface)
{

    iface->toolbutton=gtk_button_new_with_label("text");

    iface->obtain_toolmenu=text_obtain_toolmenu;

}




static void sc_text_class_init(SCTextClass*klass)
{


    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(klass);

    wclass->button_press_event=sc_text_press;
//    wclass->button_release_event=sc_text_release;
//    wclass->motion_notify_event=sc_text_motion;


}


static void text_changed(GtkTextBuffer*buffer,gpointer d)
{

    SCText*text=SC_TEXT(d);

    GtkTextIter iter_start,iter_end;

//    GtkTextBuffer*buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(tview));
//gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    gtk_text_buffer_get_start_iter(buffer,&iter_start);
    gtk_text_buffer_get_end_iter(buffer,&iter_end);

    int fsiz=sc_fontsize_chooser_get_size(SC_FONTSIZE_CHOOSER(text->fontsizechooser));
    
    char*colorspec=sc_color_chooser_get_color(SC_COLOR_CHOOSER(text->colorchooser));


    char*fontsiz=g_strdup_printf("fsiz%d",fsiz);

    g_print("fontsize::[%s]\ncolor::[%s]..\n",fontsiz,colorspec);

    gtk_text_buffer_apply_tag_by_name(buffer,fontsiz,&iter_start,&iter_end);
    gtk_text_buffer_apply_tag_by_name(buffer,colorspec,&iter_start,&iter_end);

    g_free(fontsiz);

}





static GtkWidget* new_tview(SCText* text)
{
    GtkWidget*tview;


    //GtkTextIter iter_start,iter_end;


    tview=gtk_text_view_new();
    
    GtkTextBuffer*buffer=text_view_buffer_init(GTK_TEXT_VIEW(tview),text);
    
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tview),GTK_WRAP_NONE);
    GdkRGBA opacity={0,};
    gtk_widget_override_background_color(tview,0,&opacity);

    g_signal_connect(G_OBJECT(buffer),"changed",G_CALLBACK(text_changed),text);


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
//    g_message("oooooooooooooooooooooooooooooooooooooooooooooooooooooo");

    FloatBorder*fb=FLOAT_BORDER(widget);
    GdkWindow*window=gtk_widget_get_window(widget);

    SCText*text=SC_TEXT(widget);

    if(e->button==GDK_BUTTON_SECONDARY)
        return FALSE;

    if(e->window==window){
        text->position.x=(int)e->x;
        text->position.y=(int)e->y;

        float_border_show_border(fb,FALSE);
        if(text->text_view)
            gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text->text_view),FALSE);
//forcing hide border,Before step_done().        
        while(gtk_events_pending()){
            gdk_flush();//before gtk_main_iteration()
            gtk_main_iteration();
        }
        
        if(sc_text_has_view(text)){
            SCCanvas* canvas=sc_operable_get_canvas(SC_OPERABLE(widget));

            if(sc_text_is_focus(text)){
                while(gtk_events_pending()){
                    gdk_flush();//before gtk_main_iteration()
                    gtk_main_iteration();
                }

                sc_canvas_step_done(canvas);
                text->is_focus=FALSE;
                
                if(text->text_view){
                    float_border_remove(FLOAT_BORDER(widget),text->text_view);
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

    return GTK_WIDGET_CLASS(sc_text_parent_class)->button_press_event(widget,e);
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

    text->text_view=new_tview(text);
    text->is_focus=TRUE;
    gtk_widget_show(text->text_view);
//    gtk_widget_grab_focus(text->text_view);

    float_border_put_with_size(fb,text->text_view,text->position.x,text->position.y,50,22);

    g_message("Reset..");
}



void sc_text_remove(SCText*text)
{

    float_border_remove(FLOAT_BORDER(text),text->text_view);
    text->text_view=NULL;

}

SCOperable* sc_text_new(SCCanvas*canvas)
{
    SCOperable*operable=(SCOperable*)g_object_new(SC_TYPE_TEXT,NULL);
    sc_operable_set_canvas(operable,canvas);

    return operable;

}



static void create_color_tags(GtkTextBuffer*buffer,char** table,int num_colors)
{

    GdkRGBA color;

    int i;
    for(i=0; i<num_colors; i++){

        g_print("Color Tag:: %s \n",table[i]);
        gdk_rgba_parse(&color,table[i]);
        gtk_text_buffer_create_tag(buffer,table[i],"foreground-rgba",&color,NULL);

    }

}

static void create_fsiz_tags(GtkTextBuffer*buffer,int* table,int num_fsiz)
{
    
    char*tagname;

    int i;
    for(i=0; i<num_fsiz; i++){

        tagname=g_strdup_printf("fsiz%d",table[i]);
        g_print("Fontsize Tag:: %s \n",tagname);
        gtk_text_buffer_create_tag(buffer,tagname,"size",table[i]*PANGO_SCALE,NULL);
  //      g_free(tagname);

    }

}




static GtkTextBuffer* text_view_buffer_init(GtkTextView* view,SCText*text)
{


    GtkTextIter iter_start,iter_end;
//    GtkTextTag*tag=gtk_text_tag_new("tt");


    GtkTextBuffer*buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_get_start_iter(buffer,&iter_start);
    gtk_text_buffer_get_end_iter(buffer,&iter_end);

    char**colortable;
    int length;
    length=sc_color_chooser_get_table(text->colorchooser,&colortable);

    
    int i;
    for(i=0;i<length;i++){
    
        g_print("[%d] :: {%s} \n",i,colortable[i]);

    }

    create_color_tags(buffer, colortable,length);

    int*fsiztable;
    length=sc_fontsize_chooser_get_table(text->fontsizechooser,&fsiztable);
    for(i=0;i<length;i++){
    
        g_print("[%d] :: {%d} \n",i,fsiztable[i]);

    }

    create_fsiz_tags(buffer, fsiztable,length);

    g_print("CREATE TAGS OK>>>>.>>>>>\n");

//    gtk_text_buffer_apply_tag_by_name(buffer,"fsiz18",&iter_start,&iter_end);
//    gtk_text_buffer_apply_tag_by_name(buffer,"#ff0000",&iter_start,&iter_end);

    g_free(fsiztable);
    g_strfreev(colortable);

    return buffer;

}










