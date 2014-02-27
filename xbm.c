#include "xbm.h"

static void gtk_xbm_class_init(GtkXbmClass *klass);
static void gtk_xbm_init(GtkXbm *xbm);
static void gtk_xbm_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_xbm_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_xbm_realize(GtkWidget *widget);
static gboolean gtk_xbm_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_xbm_paint(GtkWidget *widget);
static void gtk_xbm_destroy(GtkObject *object);

double Y_OFFSET;

GtkType gtk_xbm_get_type(void)
{
    static GtkType gtk_xbm_type = 0;
    if (!gtk_xbm_type)
    {
        static const GtkTypeInfo gtk_xbm_info = {"GtkXbm", sizeof(GtkXbm), sizeof(GtkXbmClass), (GtkClassInitFunc) gtk_xbm_class_init, (GtkObjectInitFunc) gtk_xbm_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_xbm_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_xbm_info);
    }
    return gtk_xbm_type;
}

GtkWidget * gtk_xbm_new()
{

    return GTK_WIDGET(gtk_type_new(gtk_xbm_get_type()));
}


static void gtk_xbm_class_init(GtkXbmClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_xbm_realize;
    widget_class->size_request = gtk_xbm_size_request;
    widget_class->size_allocate = gtk_xbm_size_allocate;
    widget_class->expose_event = gtk_xbm_expose;
    object_class->destroy = gtk_xbm_destroy;
}


void gtk_xbm_prob(GtkXbm *xbm, int t_1, int t_2)
{

    if (((t_1 == BM_PLUS) || (t_1 == BM_PLUS_NAME)) && bm_valid_count_type(t_1))
    {
        xbm->plus_type = t_1;
    }
    else
    {
        bm_error("xbm.c", "gtk_xbm_prob", "Invalid plus_type value", t_1);
    }

    if (bm_valid_count_type(t_2))
    {
        xbm->minus_type = t_2;
    }
    else
    {
        bm_error("xbm.c", "gtk_xbm_prob", "Invalid minus_type value", t_2);
    }
}

static void gtk_xbm_init(GtkXbm *xbm)
{
    xbm->plus_type = BM_PLUS;
    xbm->minus_type = BM_MINUS_NAME;
}

static void gtk_xbm_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_XBM(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

}

static void gtk_xbm_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_XBM(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

}

static void gtk_xbm_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_XBM(widget));

    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;//W;
    attributes.height = widget->allocation.height;//H;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.event_mask = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK;
    attributes_mask = GDK_WA_X | GDK_WA_Y;

    widget->window = gdk_window_new(gtk_widget_get_parent_window (widget), &attributes, attributes_mask);
    gdk_window_set_user_data(widget->window, widget);
    widget->style = gtk_style_attach(widget->style, widget->window);
    gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);
}

static gboolean gtk_xbm_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_XBM(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_xbm_paint(widget);

    return FALSE;
}



/******************************************************************************/

static double hsl_index_to_rgb_index(double tc, double p, double q)
{
    if (tc < 0.1667) {
        return(p + ((q - p) * 6.0 * tc));
    }
    else if (tc < 0.5) {
        return(q);
    }
    else if (tc < 0.6667) {
        return(p + ((q - p) * 6.0 * (0.6667 - tc)));
    }
    else {
        return(p);
    }
}

static void hsl_to_rgb(double h, double s, double l, double *r, double *g, double *b)
{
    double q, p, tr, tg, tb;

    q = (l < 0.5) ? l * (s + 1.0) : (l + s - (l * s));
    p = 2.0 * l - q;
    tr = (h > 0.6667) ? (h - 0.6667) : h + 0.3333;
    tg = h;
    tb = (h < 0.3333) ? (h + 0.6667) : h - 0.3333;
    *r = hsl_index_to_rgb_index(tr, p, q);
    *g = hsl_index_to_rgb_index(tg, p, q);
    *b = hsl_index_to_rgb_index(tb, p, q);
}

void select_colour_scale(cairo_t *cr, double d) {
    double r = 0.0, g = 0.0, b = 0.0;
    d = fabs(1.0 - fabs(d));
    if ((d >= 0.0) && (d <= 1.0))
    {
        hsl_to_rgb(d * 0.667, 1.0, 0.5, &r, &g, &b);
    }
    else
//         hsl_to_rgb(0.667, 1.0, 0.5, &r, &g, &b);
    {
        printf("d = %f\n", d);
        bm_error("xbm.c", "select_colour_scale", "Invalid input value", d);
    }
    cairo_set_source_rgb(cr, r, g, b);
}

/******************************************************************************/

void paint_probs(GtkXbm *xbm, cairo_t *cr, int y, int type)
{
    int i, j;
    double centering = (GTK_WIDGET(xbm)->allocation.width - ((BM_UNITS+1) * PIXEL)) / (double) 2.0;

    Y_OFFSET = (GTK_WIDGET(xbm)->allocation.height - ((BM_K+1) * 3.0 * PIXEL + 20.0 * 2.0)) / (double) 2.0;

    // bias of every visible unit
    for (i = 0; i < N; i++)
    {
        cairo_rectangle(cr, centering + i*PIXEL, y + Y_OFFSET, PIXEL, PIXEL);
        select_colour_scale(cr, bm_get_count(BM_PAIRS + i, type) / (double) (BM_C * P));
        cairo_fill(cr);
    }

    // bias of every hidden unit
    for (i = 0; i < BM_K; i++)
    {
        if (!i) // this is to make it a triangle that's got a smooth hypoteneuse
        {
            cairo_move_to(cr, centering + N*PIXEL + i*PIXEL + X_PADDING, y + Y_OFFSET);
            cairo_line_to(cr, centering + N*PIXEL + i*PIXEL + PIXEL + X_PADDING, y + Y_OFFSET);
            cairo_line_to(cr, centering + N*PIXEL + i*PIXEL + PIXEL + X_PADDING, y + Y_OFFSET + PIXEL);
            cairo_close_path(cr);
        }
        else
        {
            cairo_rectangle(cr, centering + N*PIXEL + i*PIXEL + X_PADDING, y + Y_OFFSET, PIXEL, PIXEL);
        }
        select_colour_scale(cr, bm_get_count(BM_PAIRS + N + i, type) / (double) (BM_C * P));
        cairo_fill(cr);
    }

    // units
    for (j = 0; j < BM_K; j++)
    {
        // visible <-> hidden
        for (i = 0; i < N; i++)
        {
            cairo_rectangle(cr, centering + i*PIXEL, y + Y_OFFSET + (j+1) * PIXEL, PIXEL, PIXEL);
            select_colour_scale(cr, bm_get_count(bm_map_vh(i, j), type) / (double) (BM_C * P));
            cairo_fill(cr);
        }

        // hidden <-> hidden
        for (i = j + 1; i < BM_K; i++)
        {
            if (i == j+1) // this is to make it a triangle that's got a smooth hypoteneuse
            {
                cairo_move_to(cr, centering + N*PIXEL + i*PIXEL + X_PADDING, y + Y_OFFSET + (j+1) * PIXEL);
                cairo_line_to(cr, centering + N*PIXEL + i*PIXEL + PIXEL + X_PADDING, y + Y_OFFSET + (j+1) * PIXEL);
                cairo_line_to(cr, centering + N*PIXEL + i*PIXEL + PIXEL + X_PADDING, y + Y_OFFSET + (j+1) * PIXEL + PIXEL);
                cairo_close_path(cr);
            }
            else
            {
                cairo_rectangle(cr, centering + N*PIXEL + i*PIXEL + X_PADDING, y + Y_OFFSET + (j+1) * PIXEL, PIXEL, PIXEL);
            }
            select_colour_scale(cr, bm_get_count(bm_map_hh(i, j), type) / (double) (BM_C * P));
            cairo_fill(cr);
        }
    }
}

static void gtk_xbm_paint(GtkWidget *widget)
{
    cairo_t *cr;
    cr = gdk_cairo_create(widget->window);
    double y = 0.0;
    cairo_translate(cr, 1, 0);

    cairo_set_source_rgba(cr, 1,1,1,0); //background colour

    cairo_paint(cr);

    paint_probs(GTK_XBM(widget), cr, y, GTK_XBM(widget)->plus_type);
    y += BM_K*PIXEL + 20;
    paint_probs(GTK_XBM(widget), cr, y, GTK_XBM(widget)->minus_type);
    y += BM_K*PIXEL + 20;
    paint_probs(GTK_XBM(widget), cr, y, BM_DELTA);

    cairo_destroy(cr);
}

static void gtk_xbm_destroy(GtkObject *object)
{
//     GtkXbm *xbm;
    GtkXbmClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_XBM(object));

//     xbm = GTK_XBM(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}


void gtk_xbm_redraw(GtkXbm *xbm)
{
    GtkWidget *widget;
    GdkRegion *region;

    widget = GTK_WIDGET(xbm);



    if (!widget->window) return;

    region = gdk_drawable_get_clip_region (widget->window);
    /* redraw the cairo canvas completely by exposing it */
    gdk_window_invalidate_region (widget->window, region, TRUE);
    gdk_window_process_updates (widget->window, TRUE);
    gdk_region_destroy (region);
}
