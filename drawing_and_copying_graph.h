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
#define GTK_DRAWING_AND_COPYING_GRAPH(obj) GTK_CHECK_CAST(obj, gtk_drawing_and_copying_graph_get_type(), GtkDrawing_and_copying_Graph)
#define GTK_DRAWING_AND_COPYING_GRAPH_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_drawing_and_copying_graph_get_type(), GtkDrawing_and_copying_GraphClass)
#define GTK_IS_DRAWING_AND_COPYING_GRAPH(obj) GTK_CHECK_TYPE(obj, gtk_drawing_and_copying_graph_get_type())

typedef struct _GtkDrawing_and_copying_Graph GtkDrawing_and_copying_Graph;
typedef struct _GtkDrawing_and_copying_GraphClass GtkDrawing_and_copying_GraphClass;

struct _GtkDrawing_and_copying_Graph
{
    GtkWidget widget;

    int type;
    int data_type;

    int legend;

    /*  double drawing_correct[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];*/
    double drawing_omission[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double drawing_intrusion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    /*  double copying_correct[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];*/
    double copying_omission[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double copying_intrusion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];

    double se_drawing_omission_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double se_drawing_intrusion_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double se_copying_omission_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double se_copying_intrusion_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];

    double se_drawing_overall[DRAWING_AND_COPYING_LESION_LEVEL];
    double se_copying_overall[DRAWING_AND_COPYING_LESION_LEVEL];

    double drawing_omission_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double drawing_intrusion_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double copying_omission_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
    double copying_intrusion_proportion[DRAWING_AND_COPYING_LESION_LEVEL][N_DOMAIN];
};

struct _GtkDrawing_and_copying_GraphClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_drawing_and_copying_graph_get_type(void);

void gtk_drawing_and_copying_graph_redraw(GtkWidget *widget);
GtkWidget *gtk_drawing_and_copying_graph_new(void);
void gtk_drawing_and_copying_graph_reset(GtkDrawing_and_copying_Graph *drawing_and_copying_graph);
void gtk_drawing_and_copying_graph_pdf(GtkWidget *widget, char *filename);
void gtk_drawing_and_copying_graph_set_type(GtkDrawing_and_copying_Graph *drawing_and_copying_graph, int type);
void gtk_drawing_and_copying_graph_set_data_type(GtkDrawing_and_copying_Graph *drawing_and_copying_graph, int type);

void gtk_drawing_and_copying_graph_update(GtkDrawing_and_copying_Graph *drawing_and_copying_graph);
void gtk_drawing_and_copying_graph_clear(GtkWidget *widget);

void gtk_drawing_and_copying_graph_set_legend(GtkDrawing_and_copying_Graph *drawing_and_copying_graph, int legend);

G_END_DECLS
