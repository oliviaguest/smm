// #ifndef __XBM_H // is this if statement really needed? -no
// #define __XBM_H
#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include "bm.h"

// extern int W, H;

#define D		9.0
#define R		(D/2.0)
#define Y_PADDING	(D/1.50)
#define X_PADDING	2.19
// #define H		620
// #define W		(MAX_N_PATTERN * X_PADDING * D) + 10.0
#define Y_SPACE		400.0
#define X_OFFSET	(10.0 + R)
#define Y_FONT		12.0
// #define Y_OFFSET	10.0
#define BM_BIAS_R		2.0

#define PIXEL 3

G_BEGIN_DECLS

// for drawing units_from and units_to
#define FROM	1
#define TO	0

// // for displaying weights above AVERAGE_COEF * average
// #define AVERAGE_COEF 1.45

#define GTK_XBM(obj) GTK_CHECK_CAST(obj, gtk_xbm_get_type (), GtkXbm)
#define GTK_XBM_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_xbm_get_type(), GtkXbmClass)
#define GTK_IS_XBM(obj) GTK_CHECK_TYPE(obj, gtk_xbm_get_type())


typedef struct _GtkXbm GtkXbm;
typedef struct _GtkXbmClass GtkXbmClass;

#define WEIGHT_COEF 1.5

struct _GtkXbm
{
    GtkWidget widget;
    int plus_type;
    int minus_type;
};



struct _GtkXbmClass
{
    GtkWidgetClass parent_class;
};


GtkType gtk_xbm_get_type(void);
void gtk_xbm_redraw(GtkXbm *xbm);
void gtk_xbm_prob(GtkXbm *xbm, int from, int to);
GtkWidget * gtk_xbm_new(void);
G_END_DECLS

// #endif