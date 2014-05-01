#ifndef _H_SC_CANVAS_
#define _H_SC_CANVAS_







#define SC_TYPE_CANVAS (sc_canvas_get_type())

#define SC_CANVAS(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_CANVAS,SCCanvas))
#define SC_IS_CANVAS(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_CANVAS))

#define SC_CANVAS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),SC_TYPE_CANVAS,SCCanvasClass))
#define SC_IS_CANVAS_CLASS(klass) (G_TYPE_CHECK_CLAsS_TYPE((klass),SC_TYPE_CANVAS))


#define SC_CANVAS_GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj),SC_TYPE_CANVAS,SCCanvasPriv))



typedef struct _SCCanvasPriv SCCanvasPriv;


typedef struct _SCCanvas{

    GtkBin parent;

    SCCanvasPriv*priv;


}SCCanvas;



typedef struct _SCCanvasClass{
    GtkBinClass parent_class;

    void (*set_background)(SCCanvas*canvas);


}SCCanvasClass;



GType sc_canvas_get_type(void);




GtkWidget*sc_canvas_new(int x,int y,int width, int height);
void sc_canvas_destroy(SCCanvas *canvas);

void sc_canvas_set(SCCanvas*canvas,int x, int y, int width, int height);
void sc_canvas_get(SCCanvas*canvas,int* x, int* y, int* width, int* height);
//void sc_canvas_s(SCCanvas*canvas,int x, int y, int width, int height);

void sc_canvas_add_menu(SCCanvas*canvas);//,GtkWidget*menu)//SCOperator* op)




#endif
