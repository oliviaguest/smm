#include "attractor_view.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_attractor_view_class_init(GtkAttractor_ViewClass *klass);
GtkType gtk_attractor_view_get_type(void);
static void gtk_attractor_view_init(GtkAttractor_View *attractor_view);
static void gtk_attractor_view_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_attractor_view_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_attractor_view_realize(GtkWidget *widget);
static gboolean gtk_attractor_view_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_attractor_view_paint(GtkWidget *widget);
static void gtk_attractor_view_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);



static int attractor_response[ATTRACTOR_DISTRIBUTION_LEVEL][ATTRACTOR_LESION_LEVEL][ATTRACTOR_SAMPLE][3][P_MAX]; // this is a temporary variable for putting values during data collection

void gtk_attractor_view_save_response(int d, int l, int s, int t, int p, int q)
{
    attractor_response[d][l][s][t][p] = q;
}

void gtk_attractor_view_init_response(void)
{
    int d, l, s, p, t;
    for (d = 0; d < ATTRACTOR_DISTRIBUTION_LEVEL; d++) { // distribution of lesion
        for (l = 0; l < ATTRACTOR_LESION_LEVEL; l++) { // level of lesioning
            for (s = 0; s < ATTRACTOR_SAMPLE; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P_MAX; p++) { // pattern
                    for (t = 0; t < 3; t++) {
                        attractor_response[d][l][s][t][p] = -1;
                    }
                }
            }
        }
    }
}





GtkType gtk_attractor_view_get_type(void)
{
    static GtkType gtk_attractor_view_type = 0;
    if (!gtk_attractor_view_type)
    {
        static const GtkTypeInfo gtk_attractor_view_info = {"GtkAttractor_View", sizeof(GtkAttractor_View), sizeof(GtkAttractor_ViewClass), (GtkClassInitFunc) gtk_attractor_view_class_init, (GtkObjectInitFunc) gtk_attractor_view_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_attractor_view_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_attractor_view_info);
    }
    return gtk_attractor_view_type;
}

GtkWidget * gtk_attractor_view_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_attractor_view_get_type()));
}




static void gtk_attractor_view_class_init(GtkAttractor_ViewClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_attractor_view_realize;
    widget_class->size_request = gtk_attractor_view_size_request;
    widget_class->size_allocate = gtk_attractor_view_size_allocate;
    widget_class->expose_event = gtk_attractor_view_expose;
    object_class->destroy = gtk_attractor_view_destroy;
}

static void gtk_attractor_view_init(GtkAttractor_View *attractor_view)
{
    int l, t, p;
    gtk_attractor_view_init_response();

    for (l = 0; l < ATTRACTOR_LESION_LEVEL; l++) {
        for (t = 0; t < 3; t++) {
            for (p = 0; p < P_MAX; p++) {
                attractor_view->response[l][t][p] = -1;
            }
        }
    }

    attractor_view->type = 0;

}

void gtk_attractor_view_set_type(GtkAttractor_View *attractor_view, int type)
{
    attractor_view->type = type;
}





static void gtk_attractor_view_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_ATTRACTOR_VIEW(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

//     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_attractor_view_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_ATTRACTOR_VIEW(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

//     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_attractor_view_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_ATTRACTOR_VIEW(widget));

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

static gboolean gtk_attractor_view_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_ATTRACTOR_VIEW(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_attractor_view_paint(widget);

    return FALSE;
}


void gtk_attractor_view_update(GtkAttractor_View *attractor_view)
{
    int d, l, p, q, s, t, max = 0, max_q = 0, pattern_response[ATTRACTOR_LESION_LEVEL][3][P_MAX];


    for (p = 0; p < P; p++) { // for each pattern, calculate the response per level

        for (q = 0; q < P; q++) { // clear the array of hits per response pattern q
            for (l = 0; l < ATTRACTOR_LESION_LEVEL; l++) {
                for (t = 0; t < 3; t++) {
                    pattern_response[l][t][q] = 0;
                }
            }
        }

        for (l = 0; l < ATTRACTOR_LESION_LEVEL; l++) {
            for (t = 0; t < 3; t++) {

                for (d = 0; d < ATTRACTOR_DISTRIBUTION_LEVEL; d++) {
                    for (s = 0; s < ATTRACTOR_SAMPLE; s++) {
                        pattern_response[l][t][attractor_response[d][l][s][t][p]]++;  // add up all responses that were given for p
                    }
                }

                // Now we're done with collecting, we can find which response has the most hits on this level, l, for this type, t.
                for (q = 0; q < P; q++) { // clear the array of hits per response pattern q
                    if (max <  pattern_response[l][t][q]) {
                        max =  pattern_response[l][t][q];
                        max_q = q;
                    }
                }

                attractor_view->response[l][t][p] = max_q;

                max = 0;
                max_q = 0;
            }

        }

    }

}



void gtk_attractor_view_save(GtkAttractor_View *attractor_view, char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int l, t, p;
    FILE *fp = NULL;
    fp = fopen(filename, "wt");

    for (l = 0; l < ATTRACTOR_LESION_LEVEL; l++) { // level of leasioning
        for (t = 0; t < 3; t++) { // to ensure network in "right" attractor
            for (p = 0; p < P; p++) { // pattern
                fprintf(fp, "%d\n", attractor_view->response[l][t][p]);
            }
        }
    }
    fprintf(fp, "\nEOF\n");
    if (fp) {
        fclose(fp);
    }
}

void gtk_attractor_view_open(GtkAttractor_View *attractor_view, char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int l, t, p;
    FILE *fp = NULL;
    fp = fopen(filename, "r");

    for (l = 0; l < ATTRACTOR_LESION_LEVEL; l++) { // level of leasioning
        for (t = 0; t < 3; t++) { // to ensure network in "right" attractor
            for (p = 0; p < P; p++) { // pattern
                if (!fscanf(fp, "%d\n", &attractor_view->response[l][t][p])) {
                    throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: response[][][]", 0);
                    gtk_attractor_view_init(attractor_view);
                    break;
                }
            }
        }
    }
    fprintf(fp, "\nEOF\n");
    if (fp) {
        fclose(fp);
    }
}


static void gtk_attractor_view_paint(GtkWidget *widget)
{
    GtkAttractor_View *attractor_view = GTK_ATTRACTOR_VIEW(widget);






    int width, height;
    PangoLayout *layout;
    CairoxTextParameters text_p;
    char buffer[64];
    cairo_t *cr;
    int p, c;

    width = widget->allocation.width;
    height = widget->allocation.height;

    cr = gdk_cairo_create(widget->window);

    /* Transparent background */
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint(cr);
    //int current_attractor_i = ((iterations_so_far) && (iterations_so_far < C_MAX)) ? (iterations_so_far) : (0);
    layout = pango_cairo_create_layout(cr);
// 	if (!training) {
//         g_snprintf(buffer, 64, "Step: %d", step_number);
//         cairox_text_parameters_set(&text_p, 20, height-20, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
//         cairox_paint_pango_text(cr, &text_p, layout, buffer);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);

// 	  cairo_rectangle(cr, 20, 20, 120, 80);
    cairo_rectangle(cr, X_MARGIN, Y_MARGIN, width - (2.0 * X_MARGIN), height - (2.0 * Y_MARGIN));
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_fill(cr);

    double n_rows = (double) (P + 6);
    double row_height = (height - (Y_MARGIN*2)) / n_rows;

    double n_columns = ATTRACTOR_LESION_LEVEL + 1.0;
    double column_width = (width - (X_MARGIN*2)) / n_columns;

    int smaller_font_size = 14, bigger_font_size = 16;
    if (n_rows > 36) {
        smaller_font_size = 12;
        bigger_font_size = 15;
    }

    pangox_layout_set_font_size(layout, bigger_font_size);
    g_snprintf(buffer, 64, "Pattern Name");
    cairox_text_parameters_set(&text_p, X_MARGIN*2, row_height*2, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    cairox_paint_pango_text(cr, &text_p, layout, buffer);

    for (c = 0; c < ATTRACTOR_LESION_LEVEL; c++) {
        g_snprintf(buffer, 64, "%1.1f%% Lesioned", c * 100.0 / (double) ATTRACTOR_LESION_LEVEL);
        cairox_text_parameters_set(&text_p, column_width*(c+1), row_height*2, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);
    }


    pangox_layout_set_font_size(layout, smaller_font_size);

    for (p = 0; p < P; p++) {
        g_snprintf(buffer, 64, "%s", get_name_string(p));
        cairox_text_parameters_set(&text_p, X_MARGIN*2, (p+4)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);
    }

    if (attractor_view->response[0][0][0] != -1) {

        for (c = 0; c < ATTRACTOR_LESION_LEVEL; c++) {

            for (p = 0; p < P; p++) {
                g_snprintf(buffer, 64, "%s", get_name_string(attractor_view->response[c][attractor_view->type][p]));
                cairox_text_parameters_set(&text_p, column_width*(c+1), (p+4)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text(cr, &text_p, layout, buffer);
            }
        }
    }





    g_object_unref(layout);

    cairo_destroy(cr);

}

void gtk_attractor_view_reset(GtkAttractor_View *attractor_view)
{

}

static void gtk_attractor_view_destroy(GtkObject *object)
{
//     GtkAttractor_View *attractor_view;
    GtkAttractor_ViewClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_ATTRACTOR_VIEW(object));

//     attractor_view = GTK_ATTRACTOR_VIEW(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}




void gtk_attractor_view_redraw(GtkWidget *widget)
{
    GdkRegion *region;
    if (!widget->window) return;

// 	gdk_window_get_size(widget->window, &W, &H);
    region = gdk_drawable_get_clip_region (widget->window);
    /* redraw the cairo canvas completely by exposing it */
    gdk_window_invalidate_region (widget->window, region, TRUE);
    gdk_window_process_updates (widget->window, TRUE);
    gdk_region_destroy (region);

}