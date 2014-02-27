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

//types of graph
#define DISTINCTIVE_FEATURES_ERROR	0
#define FEATURES_LIVING_ERROR		1
#define SHARED_FEATURES_ERROR		2
#define FUNCTIONAL_FEATURES_ERROR	3
#define PERCENT_PATTERNS_CORRECT	4
#define PERCENT_PATTERNS_INCORRECT	5

#define MODEL           0
#define PATIENTS	1

#define TYLER		2
#define GTK_TYLER_GRAPH(obj) GTK_CHECK_CAST(obj, gtk_tyler_graph_get_type(), GtkTyler_Graph)
#define GTK_TYLER_GRAPH_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_tyler_graph_get_type(), GtkTyler_GraphClass)
#define GTK_IS_TYLER_GRAPH(obj) GTK_CHECK_TYPE(obj, gtk_tyler_graph_get_type())

typedef struct _GtkTyler_Graph GtkTyler_Graph;
typedef struct _GtkTyler_GraphClass GtkTyler_GraphClass;

struct _GtkTyler_Graph
{
    GtkWidget widget;
    double shared_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
    double distinctive_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
    double functional_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
    double percent_correct[TYLER_LESION_LEVEL][N_DOMAIN];
    double percent_incorrect[TYLER_LESION_LEVEL][N_DOMAIN][2]; // between/within per domain

//     double shared_features_error_overall[TYLER_LESION_LEVEL];
//     double distinctive_features_error_overall[TYLER_LESION_LEVEL];
//     double functional_features_error_overall[TYLER_LESION_LEVEL];
    int type;
    int data_type;

    double se_shared_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
    double se_distinctive_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
    double se_functional_features_error[TYLER_LESION_LEVEL][N_DOMAIN];

    double se_percent_correct[TYLER_LESION_LEVEL][N_DOMAIN];
    double se_percent_incorrect[TYLER_LESION_LEVEL][N_DOMAIN][2]; // between/within per domain

    int networks;
    char *auto_name;
//     double se_shared_features_error_overall[TYLER_LESION_LEVEL];
//     double se_distinctive_features_error_overall[TYLER_LESION_LEVEL];
//     double se_functional_features_error_overall[TYLER_LESION_LEVEL];


};

struct _GtkTyler_GraphClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_tyler_graph_get_type(void);

void gtk_tyler_graph_redraw(GtkWidget *widget);
GtkWidget *gtk_tyler_graph_new(void);
void gtk_tyler_graph_reset(GtkTyler_Graph *tyler_graph);
void gtk_tyler_graph_pdf(GtkWidget *widget, char *filename);
void gtk_tyler_graph_set_type(GtkTyler_Graph *tyler_graph, int type);
void gtk_tyler_graph_set_data_type(GtkTyler_Graph *tyler_graph, int type);
void gtk_tyler_graph_update(GtkTyler_Graph *tyler_graph);
void gtk_tyler_graph_clear(GtkWidget *widget);
void gtk_tyler_graph_networks(GtkTyler_Graph *tyler_graph, int value);
void gtk_tyler_graph_update_networks(GtkTyler_Graph *tyler_graph);
void gtk_tyler_set_auto_name(GtkTyler_Graph *tyler_graph, const char auto_name[]);

void gtk_tyler_graph_update_networks_accumulate(GtkTyler_Graph *tyler_graph, int n);
void gtk_tyler_graph_update_networks_final(GtkTyler_Graph *tyler_graph);

G_END_DECLS
