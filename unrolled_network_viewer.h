#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include "network.h"
#include "lib_cairox.h"
#include "utils.h"

#define UNROLLED_STATES	0
#define ROLLED_STATES	1
#define DELTAS		2
#define ERRORS		3
#define PATTERNS	4
#define PRETTY_STATES	5

// #define CLAMPED_PATTERN 5
// #define BM_STATES	6
// #define BM_PATTERN	7
// #define PDPTOOL_STATES	8
// #define PDPTOOL_PATTERN	9

#define X_MARGIN 20
#define Y_MARGIN 20

extern int W, H;

G_BEGIN_DECLS

#define GTK_UNROLLED_NETWORK_VIEWER(obj) GTK_CHECK_CAST(obj, gtk_unrolled_network_viewer_get_type(), GtkUnrolled_network_viewer)
#define GTK_UNROLLED_NETWORK_VIEWER_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_unrolled_network_viewer_get_type(), GtkUnrolled_network_viewerClass)
#define GTK_IS_UNROLLED_NETWORK_VIEWER(obj) GTK_CHECK_TYPE(obj, gtk_unrolled_network_viewer_get_type())

typedef struct _GtkUnrolled_network_viewer GtkUnrolled_network_viewer;
typedef struct _GtkUnrolled_network_viewerClass GtkUnrolled_network_viewerClass;

struct _GtkUnrolled_network_viewer
{
    GtkWidget widget;
    int type_of_view;
    double delta_min;
    double delta_max;
    double error_min;
    double error_max;
    int *network_type;
    int *error_type;


};

struct _GtkUnrolled_network_viewerClass
{
    GtkWidgetClass parent_class;
};

void unrolled_network_viewer_network_type_set(GtkUnrolled_network_viewer *unrolled_network_viewer, int *network_type);

void unrolled_network_viewer_error_type_set(GtkUnrolled_network_viewer *unrolled_network_viewer, int *error_type);
GtkType gtk_unrolled_network_viewer_get_type(void);
void gtk_unrolled_network_viewer_redraw(GtkWidget *widget);
GtkWidget *gtk_unrolled_network_viewer_new(void);
void gtk_unrolled_network_viewer_type_of_view(GtkUnrolled_network_viewer *unrolled_network_viewer, int value);
int gtk_unrolled_network_viewer_get_type_of_view(GtkUnrolled_network_viewer *unrolled_network_viewer);
void gtkx_flush_events(void);
void gtk_unrolled_network_viewer_pdf(GtkWidget *widget, char filename[]);
G_END_DECLS
