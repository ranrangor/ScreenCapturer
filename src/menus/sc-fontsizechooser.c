
#include<gtk/gtk.h>
#include"sc-fontsizechooser.h"
#include<stdlib.h>


static int fontsiz_table[]={8,9,10,12,14,16,18,20,22};



GtkWidget* sc_fontsize_chooser_new(int defaultsize)
{


    GtkWidget*comb=gtk_combo_box_text_new();


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




