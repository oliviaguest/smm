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
#define GTK_NAMING_BAR_CHART(obj) GTK_CHECK_CAST(obj, gtk_naming_bar_chart_get_type(), GtkNaming_Bar_chart)
#define GTK_NAMING_BAR_CHART_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_naming_bar_chart_get_type(), GtkNaming_Bar_chartClass)
#define GTK_IS_NAMING_BAR_CHART(obj) GTK_CHECK_TYPE(obj, gtk_naming_bar_chart_get_type())

typedef struct _GtkNaming_Bar_chart GtkNaming_Bar_chart;
typedef struct _GtkNaming_Bar_chartClass GtkNaming_Bar_chartClass;

struct _GtkNaming_Bar_chart
{
    GtkWidget widget;

    int type;
    int data_type;

    /*  double drawing_correct[NAMING_LESION_LEVEL][N_DOMAIN];*/
    double drawing_omission[NAMING_LESION_LEVEL][N_DOMAIN];
    double drawing_intrusion[NAMING_LESION_LEVEL][N_DOMAIN];
    /*  double copying_correct[NAMING_LESION_LEVEL][N_DOMAIN];*/
    double copying_omission[NAMING_LESION_LEVEL][N_DOMAIN];
    double copying_intrusion[NAMING_LESION_LEVEL][N_DOMAIN];

    double se_drawing_omission_proportion[NAMING_LESION_LEVEL][N_DOMAIN];
    double se_drawing_intrusion_proportion[NAMING_LESION_LEVEL][N_DOMAIN];
    double se_copying_omission_proportion[NAMING_LESION_LEVEL][N_DOMAIN];
    double se_copying_intrusion_proportion[NAMING_LESION_LEVEL][N_DOMAIN];

    double se_drawing_overall[NAMING_LESION_LEVEL];
    double se_copying_overall[NAMING_LESION_LEVEL];

    double drawing_omission_proportion[NAMING_LESION_LEVEL][N_DOMAIN];
    double drawing_intrusion_proportion[NAMING_LESION_LEVEL][N_DOMAIN];
    double copying_omission_proportion[NAMING_LESION_LEVEL][N_DOMAIN];
    double copying_intrusion_proportion[NAMING_LESION_LEVEL][N_DOMAIN];



};

struct _GtkNaming_Bar_chartClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_naming_bar_chart_get_type(void);

void gtk_naming_bar_chart_redraw(GtkWidget *widget);
GtkWidget *gtk_naming_bar_chart_new(void);
void gtk_naming_bar_chart_reset(GtkNaming_Bar_chart *naming_bar_chart);
void gtk_naming_bar_chart_pdf(GtkWidget *widget, char *filename);
void gtk_naming_bar_chart_set_type(GtkNaming_Bar_chart *naming_bar_chart, int type);
void gtk_naming_bar_chart_set_data_type(GtkNaming_Bar_chart *naming_bar_chart, int type);

void gtk_naming_bar_chart_update(GtkNaming_Bar_chart *naming_bar_chart);
void gtk_naming_bar_chart_clear(GtkWidget *widget);

G_END_DECLS
