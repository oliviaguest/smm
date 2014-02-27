#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include "network.h"
#include "lib_cairox.h"

// #define STATES		0
// #define DELTAS		1
// #define ERRORS		2
#define X_MARGIN 20
#define Y_MARGIN 20

extern int W, H;

G_BEGIN_DECLS

#define GTK_GRAPH(obj) GTK_CHECK_CAST(obj, gtk_graph_get_type(), GtkGraph)
#define GTK_GRAPH_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_graph_get_type(), GtkGraphClass)
#define GTK_IS_GRAPH(obj) GTK_CHECK_TYPE(obj, gtk_graph_get_type())

typedef struct _GtkGraph GtkGraph;
typedef struct _GtkGraphClass GtkGraphClass;

struct _GtkGraph
{
    GtkWidget widget;
// 	int I;
    int step;
    int MAX_I;
    int max_j;
    double *y1;
    double *y2;
    double *y3;
    double *y4;
};

struct _GtkGraphClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_graph_get_type(void);

void gtk_graph_redraw(GtkWidget *widget);
GtkWidget *gtk_graph_new(void);
void gtk_graph_reset(GtkGraph *graph);
void gtk_graph_set_max_iterations(GtkGraph *graph, int MAX_I);
void gtk_graph_set_max_j(GtkGraph *graph, int max_j);
void gtk_graph_set_y1(GtkGraph *graph, double y1[]);
void gtk_graph_set_ys(GtkGraph *graph, double y1[], double y2[], double y3[], double y4[]);
void gtk_graph_put_point(GtkGraph *graph, int x, double y1);
void gtk_graph_pdf(GtkWidget *widget, char *filename);

G_END_DECLS
