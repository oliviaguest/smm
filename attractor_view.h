#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include "network.h"
#include "lib_cairox.h"

#define X_MARGIN 20
#define Y_MARGIN 20

extern int W, H;

G_BEGIN_DECLS

#define GTK_ATTRACTOR_VIEW(obj) GTK_CHECK_CAST(obj, gtk_attractor_view_get_type(), GtkAttractor_View)
#define GTK_ATTRACTOR_VIEW_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_attractor_view_get_type(), GtkAttractor_ViewClass)
#define GTK_IS_ATTRACTOR_VIEW(obj) GTK_CHECK_TYPE(obj, gtk_attractor_view_get_type())

#define ATTRACTOR_LESION_LEVEL 20
#define ATTRACTOR_DISTRIBUTION_LEVEL 1
#define ATTRACTOR_SAMPLE 1

typedef struct _GtkAttractor_View GtkAttractor_View;
typedef struct _GtkAttractor_ViewClass GtkAttractor_ViewClass;


struct _GtkAttractor_View
{
    GtkWidget widget;
    int response[ATTRACTOR_LESION_LEVEL][3][P_MAX];
    int type;
//     int distribution_max;
//     int sample_max;

};

struct _GtkAttractor_ViewClass
{
    GtkWidgetClass parent_class;
};

GtkType gtk_attractor_view_get_type(void);

void gtk_attractor_view_redraw(GtkWidget *widget);
// gave p, got q
void gtk_attractor_view_save_response(int d, int l, int s, int t, int p, int q);
void gtk_attractor_view_init_response(void);
// void gtk_attractor_view_set_response_name(GtkWidget *widget, int p, int q, int distribution, int level, int sample);
// void gtk_attractor_view_set_response_visual(GtkWidget *widget, int p, int q, int distribution, int level, int sample);
// void gtk_attractor_view_set_response_verbal(GtkWidget *widget, int p, int q, int distribution, int level, int sample);
GtkWidget *gtk_attractor_view_new(void);
void gtk_attractor_view_reset(GtkAttractor_View *attractor_view);
void gtk_attractor_view_set_type(GtkAttractor_View *attractor_view, int type);
void gtk_attractor_view_save(GtkAttractor_View *attractor_view, char *filename);
void gtk_attractor_view_open(GtkAttractor_View *attractor_view, char *filename);
void gtk_attractor_view_update(GtkAttractor_View *attractor_view);
G_END_DECLS
