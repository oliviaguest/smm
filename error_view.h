#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include "network.h"
#include "lib_cairox.h"
#include "error.h"

#define X_MARGIN 20
#define Y_MARGIN 20

extern int W, H;

G_BEGIN_DECLS

#define GTK_ERROR_VIEW(obj) GTK_CHECK_CAST(obj, gtk_error_view_get_type(), GtkError_View)
#define GTK_ERROR_VIEW_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_error_view_get_type(), GtkError_ViewClass)
#define GTK_IS_ERROR_VIEW(obj) GTK_CHECK_TYPE(obj, gtk_error_view_get_type())

typedef struct _GtkError_View GtkError_View;
typedef struct _GtkError_ViewClass GtkError_ViewClass;

struct _GtkError_View
{
    GtkWidget widget;
    double *Pattern_Error_Name;
    double *Pattern_Error_Visual;
    double *Pattern_Error_Verbal;
    double *Total_Stage_Error;
    double *Total_Pattern_Error;
};

struct _GtkError_ViewClass
{
    GtkWidgetClass parent_class;
};

GtkType gtk_error_view_get_type(void);

void gtk_error_view_redraw(GtkWidget *widget);
void gtk_error_view_set_errors(GtkWidget *widget, double Pattern_Error_Name[], double Pattern_Error_Visual[], double Pattern_Error_Verbal[], double Total_Stage_Error[], double Total_Pattern_Error[]);
GtkWidget *gtk_error_view_new(void);
void gtk_error_view_reset(GtkError_View *error_view);


G_END_DECLS
