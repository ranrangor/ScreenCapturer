
#include<gtk/gtk.h>
#include"sc-fontsizechooser.h"
#include"../sc-text.h"
#include<stdlib.h>


static int fontsiz_table[]={8,9,10,12,14,16,18,20,22};



static void changed_notify(GtkWidget*widget,GParamSpec *pspec,gpointer d)
{
    g_print("In ChangedNotify:[]%x[] \n",d);

    GtkTextView*tv=sc_text_get_view(d);
    g_print("Notify::Changed...itv{%x}.\n",tv);
    if(!tv)
    {
        g_print("NULL TV..\n");
        return;
    
    }
    GtkTextBuffer*buff=gtk_text_view_get_buffer(tv);

    g_signal_emit_by_name(buff,"changed",d,NULL);


}


GtkWidget* sc_fontsize_chooser_new(int defaultsize,SCText*text)
{

    g_print("SCText is [%x]\n\n",text);

    GtkWidget*comb=gtk_combo_box_text_new();

    gtk_widget_set_can_focus(comb,FALSE);

    int i;
    int num_fsiz=sizeof(fontsiz_table)/sizeof(int);

    for(i=0;i<num_fsiz;i++){
        char*lab=g_strdup_printf("%d",fontsiz_table[i]);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comb),lab);
        g_free(lab);
    }


    int defaultindex=4;
    for(i=0;i<num_fsiz;i++){
        if(fontsiz_table[i]==defaultsize){
            defaultindex=i;
            break;
        }

    }

    g_print("Current Actived FontSize:[%d]:%d..\n",defaultindex,fontsiz_table[i]);

    gtk_combo_box_set_active(GTK_COMBO_BOX(comb),defaultindex);

    g_signal_connect(G_OBJECT(comb),"notify::active",G_CALLBACK(changed_notify),(text));

    return comb;

}


int sc_fontsize_chooser_get_size(SCFontsizeChooser*chooser)
{
    char*fsiz=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(chooser));

    return atoi(fsiz);
}


int sc_fontsize_chooser_get_table(SCFontsizeChooser*chooser,int**table)
{

    int num_fsiz=sizeof(fontsiz_table)/sizeof(int);

    int* array=(int*)g_malloc0(num_fsiz*sizeof(int));

    int i;
    for(i=0;i<num_fsiz;i++){
        array[i]=fontsiz_table[i];
    }
    *table=array;

    return num_fsiz;
}




