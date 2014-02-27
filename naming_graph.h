#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
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

#define GTK_NAMING_GRAPH(obj) GTK_CHECK_CAST(obj, gtk_naming_graph_get_type(), GtkNaming_Graph)
#define GTK_NAMING_GRAPH_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_naming_graph_get_type(), GtkNaming_GraphClass)
#define GTK_IS_NAMING_GRAPH(obj) GTK_CHECK_TYPE(obj, gtk_naming_graph_get_type())



typedef struct _GtkNaming_Graph GtkNaming_Graph;
typedef struct _GtkNaming_GraphClass GtkNaming_GraphClass;

struct _GtkNaming_Graph
{
    GtkWidget widget;
    double semantic[NAMING_LESION_LEVEL][N_DOMAIN];
    double superordinate[NAMING_LESION_LEVEL][N_DOMAIN];
    double crossdomain[NAMING_LESION_LEVEL][N_DOMAIN];
    double omission[NAMING_LESION_LEVEL][N_DOMAIN];
    double correct[NAMING_LESION_LEVEL];
    int type;
    int data_type;
    int legend;
    int way;

};

struct _GtkNaming_GraphClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_naming_graph_get_type(void);
void gtk_naming_graph_set_way(GtkNaming_Graph *naming_graph, int value);
void gtk_naming_graph_redraw(GtkWidget *widget);
GtkWidget *gtk_naming_graph_new(void);
void gtk_naming_graph_reset(GtkNaming_Graph *naming_graph);
void gtk_naming_graph_pdf(GtkWidget *widget, char *filename);
void gtk_naming_graph_set_type(GtkNaming_Graph *naming_graph, int type);
void gtk_naming_graph_set_legend(GtkNaming_Graph *naming_graph, int legend);
void gtk_naming_graph_set_data_type(GtkNaming_Graph *naming_graph, int type);
void gtk_naming_graph_update(GtkNaming_Graph *naming_graph);
void gtk_naming_graph_clear(GtkWidget *widget);

G_END_DECLS
