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
#define ERROR_BY_DOMAIN	0
#define FEATURES_LIVING_ERROR		1
#define SHARED_FEATURES_ERROR		2
#define FUNCTIONAL_FEATURES_ERROR	3
#define PERCENT_PATTERNS_CORRECT	4
#define PERCENT_PATTERNS_INCORRECT	5


#define MODEL           0
#define PATIENTS	1

#define AOA		2
#define GTK_AOA_GRAPH(obj) GTK_CHECK_CAST(obj, gtk_aoa_graph_get_type(), GtkAoA_Graph)
#define GTK_AOA_GRAPH_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_aoa_graph_get_type(), GtkAoA_GraphClass)
#define GTK_IS_AOA_GRAPH(obj) GTK_CHECK_TYPE(obj, gtk_aoa_graph_get_type())

typedef struct _GtkAoA_Graph GtkAoA_Graph;
typedef struct _GtkAoA_GraphClass GtkAoA_GraphClass;

struct _GtkAoA_Graph
{
    GtkWidget widget;
    double error[AOA_LESION_LEVEL][N_DOMAIN];
    double percent_correct[AOA_LESION_LEVEL][N_DOMAIN];
    double percent_incorrect[AOA_LESION_LEVEL][N_DOMAIN][2]; // between/within per domain

    //     double shared_features_error_overall[AOA_LESION_LEVEL];
    //     double distinctive_features_error_overall[AOA_LESION_LEVEL];
    //     double functional_features_error_overall[AOA_LESION_LEVEL];
    int type;
    int data_type;

    double se_error[AOA_LESION_LEVEL][N_DOMAIN];
//     double se_distinctive_features_error[AOA_LESION_LEVEL][N_DOMAIN];
//     double se_functional_features_error[AOA_LESION_LEVEL][N_DOMAIN];

    double se_percent_correct[AOA_LESION_LEVEL][N_DOMAIN];
    double se_percent_incorrect[AOA_LESION_LEVEL][N_DOMAIN][2]; // between/within per domain

    int networks;
    char *auto_name;
    //     double se_shared_features_error_overall[AOA_LESION_LEVEL];
    //     double se_distinctive_features_error_overall[AOA_LESION_LEVEL];
    //     double se_functional_features_error_overall[AOA_LESION_LEVEL];


};

struct _GtkAoA_GraphClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_aoa_graph_get_type(void);

void gtk_aoa_graph_redraw(GtkWidget *widget);
GtkWidget *gtk_aoa_graph_new(void);
void gtk_aoa_graph_reset(GtkAoA_Graph *aoa_graph);
void gtk_aoa_graph_pdf(GtkWidget *widget, char *filename);
void gtk_aoa_graph_set_type(GtkAoA_Graph *aoa_graph, int type);
void gtk_aoa_graph_set_data_type(GtkAoA_Graph *aoa_graph, int type);
void gtk_aoa_graph_update(GtkAoA_Graph *aoa_graph);
void gtk_aoa_graph_clear(GtkWidget *widget);
void gtk_aoa_graph_networks(GtkAoA_Graph *aoa_graph, int value);
void gtk_aoa_graph_update_networks(GtkAoA_Graph *aoa_graph);
void gtk_aoa_set_auto_name(GtkAoA_Graph *aoa_graph, const char auto_name[]);
int gtk_aoa_graph_get_networks(GtkAoA_Graph *aoa_graph) ;

void gtk_aoa_graph_update_networks_accumulate(GtkAoA_Graph *aoa_graph, int n);
void gtk_aoa_graph_update_networks_final(GtkAoA_Graph *aoa_graph);

G_END_DECLS
