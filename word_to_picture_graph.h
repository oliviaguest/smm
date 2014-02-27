#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include "bptt.h"
#include "lib_cairox.h"
#include "semantic_tasks.h"
#include "network.h"
#define X_MARGIN 20
#define Y_MARGIN 20

extern int W, H;

G_BEGIN_DECLS
#define MODEL           0
#define PATIENTS	1

#define ROGERS		2
#define GTK_WORD_TO_PICTURE_GRAPH(obj) GTK_CHECK_CAST(obj, gtk_word_to_picture_graph_get_type(), GtkWord_to_picture_Graph)
#define GTK_WORD_TO_PICTURE_GRAPH_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_word_to_picture_graph_get_type(), GtkWord_to_picture_GraphClass)
#define GTK_IS_WORD_TO_PICTURE_GRAPH(obj) GTK_CHECK_TYPE(obj, gtk_word_to_picture_graph_get_type())

typedef struct _GtkWord_to_picture_Graph GtkWord_to_picture_Graph;
typedef struct _GtkWord_to_picture_GraphClass GtkWord_to_picture_GraphClass;

struct _GtkWord_to_picture_Graph
{
    GtkWidget widget;
    double close[WORD_TO_PICTURE_LESION_LEVEL][N_DOMAIN];
    double distant[WORD_TO_PICTURE_LESION_LEVEL][N_DOMAIN];
    double unrelated[WORD_TO_PICTURE_LESION_LEVEL][N_DOMAIN];
    int type;
    int data_type;
    int current_level;
    int legend;
    double se_close[WORD_TO_PICTURE_LESION_LEVEL];
    double se_distant[WORD_TO_PICTURE_LESION_LEVEL];
    double se_unrelated[WORD_TO_PICTURE_LESION_LEVEL];
};

struct _GtkWord_to_picture_GraphClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_word_to_picture_graph_get_type(void);

void gtk_word_to_picture_graph_redraw(GtkWidget *widget);
GtkWidget *gtk_word_to_picture_graph_new(void);
void gtk_word_to_picture_graph_reset(GtkWord_to_picture_Graph *word_to_picture_graph);
void gtk_word_to_picture_graph_pdf(GtkWidget *widget, char *filename);
void gtk_word_to_picture_graph_set_type(GtkWord_to_picture_Graph *word_to_picture_graph, int type);
void gtk_word_to_picture_graph_set_current_level(GtkWord_to_picture_Graph *word_to_picture_graph, int level);

void gtk_word_to_picture_graph_update(GtkWord_to_picture_Graph *word_to_picture_graph);
void gtk_word_to_picture_graph_clear(GtkWidget *widget);
void gtk_word_to_picture_graph_set_legend(GtkWord_to_picture_Graph *word_to_picture_graph, int legend);
G_END_DECLS
