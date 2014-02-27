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
#define GTK_SORTING_GRAPH(obj) GTK_CHECK_CAST(obj, gtk_sorting_graph_get_type(), GtkSorting_Graph)
#define GTK_SORTING_GRAPH_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_sorting_graph_get_type(), GtkSorting_GraphClass)
#define GTK_IS_SORTING_GRAPH(obj) GTK_CHECK_TYPE(obj, gtk_sorting_graph_get_type())

typedef struct _GtkSorting_Graph GtkSorting_Graph;
typedef struct _GtkSorting_GraphClass GtkSorting_GraphClass;

struct _GtkSorting_Graph
{
    GtkWidget widget;
    double general_name[SORTING_LESION_LEVEL];
    double specific_name[SORTING_LESION_LEVEL];
    double general_visual[SORTING_LESION_LEVEL];
    double specific_visual[SORTING_LESION_LEVEL];
    int type;
    int data_type;
    int legend;

    double se_general_name[SORTING_LESION_LEVEL];
    double se_specific_name[SORTING_LESION_LEVEL];
    double se_general_visual[SORTING_LESION_LEVEL];
    double se_specific_visual[SORTING_LESION_LEVEL];


};

struct _GtkSorting_GraphClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_sorting_graph_get_type(void);

void gtk_sorting_graph_redraw(GtkWidget *widget);
GtkWidget *gtk_sorting_graph_new(void);
void gtk_sorting_graph_reset(GtkSorting_Graph *sorting_graph);
void gtk_sorting_graph_pdf(GtkWidget *widget, char *filename);
void gtk_sorting_graph_set_type(GtkSorting_Graph *sorting_graph, int type);
void gtk_sorting_graph_set_legend(GtkSorting_Graph *sorting_graph, int legend);
void gtk_sorting_graph_set_data_type(GtkSorting_Graph *sorting_graph, int type);
void gtk_sorting_graph_update(GtkSorting_Graph *sorting_graph);
void gtk_sorting_graph_clear(GtkWidget *widget);

G_END_DECLS
