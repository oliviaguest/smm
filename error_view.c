#include "error_view.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_error_view_class_init(GtkError_ViewClass *klass);
GtkType gtk_error_view_get_type(void);
static void gtk_error_view_init(GtkError_View *error_view);
static void gtk_error_view_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_error_view_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_error_view_realize(GtkWidget *widget);
static gboolean gtk_error_view_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_error_view_paint(GtkWidget *widget);
static void gtk_error_view_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

GtkType gtk_error_view_get_type(void)
{
    static GtkType gtk_error_view_type = 0;
    if (!gtk_error_view_type)
    {
        static const GtkTypeInfo gtk_error_view_info = {"GtkError_View", sizeof(GtkError_View), sizeof(GtkError_ViewClass), (GtkClassInitFunc) gtk_error_view_class_init, (GtkObjectInitFunc) gtk_error_view_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_error_view_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_error_view_info);
    }
    return gtk_error_view_type;
}

GtkWidget * gtk_error_view_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_error_view_get_type()));
}
void gtk_error_view_set_errors(GtkWidget *widget, double Pattern_Error_Name[], double Pattern_Error_Visual[], double Pattern_Error_Verbal[], double Total_Stage_Error[], double Total_Pattern_Error[])
{
    GtkError_View *error_view = GTK_ERROR_VIEW(widget);

    error_view->Pattern_Error_Name = &Pattern_Error_Name[0];
    error_view->Pattern_Error_Visual = &Pattern_Error_Visual[0];
    error_view->Pattern_Error_Verbal = &Pattern_Error_Verbal[0];
    error_view->Total_Stage_Error = &Total_Stage_Error[0];
    error_view->Total_Pattern_Error = &Total_Pattern_Error[0];
}


static void gtk_error_view_class_init(GtkError_ViewClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_error_view_realize;
    widget_class->size_request = gtk_error_view_size_request;
    widget_class->size_allocate = gtk_error_view_size_allocate;
    widget_class->expose_event = gtk_error_view_expose;
    object_class->destroy = gtk_error_view_destroy;
}

static void gtk_error_view_init(GtkError_View *error_view)
{
//   GtkError_View *error_view = GTK_ERROR_VIEW(widget);
    error_view->Pattern_Error_Name = NULL;

    error_view->Pattern_Error_Visual = NULL;

    error_view->Pattern_Error_Verbal = NULL;
    error_view->Total_Stage_Error = NULL;
    error_view->Total_Pattern_Error = NULL;

}

static void gtk_error_view_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_ERROR_VIEW(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

//     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_error_view_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_ERROR_VIEW(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

//     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_error_view_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_ERROR_VIEW(widget));

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

static gboolean gtk_error_view_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_ERROR_VIEW(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_error_view_paint(widget);

    return FALSE;
}




static void gtk_error_view_paint(GtkWidget *widget)
{
    GtkError_View *error_view = GTK_ERROR_VIEW(widget);


    if (error_view->Pattern_Error_Name != NULL) {



        int width, height;
        PangoLayout *layout;
        CairoxTextParameters text_p;
        char buffer[64];
        cairo_t *cr;
        int p;

        width = widget->allocation.width;
        height = widget->allocation.height;

        cr = gdk_cairo_create(widget->window);

        /* Transparent background */
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
        cairo_paint(cr);
        //int current_error_i = ((iterations_so_far) && (iterations_so_far < C_MAX)) ? (iterations_so_far) : (0);
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

        double n_columns = 5.0;
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

        g_snprintf(buffer, 64, "Name Error");
        cairox_text_parameters_set(&text_p, column_width, row_height*2, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);

        g_snprintf(buffer, 64, "Visual Error");
        cairox_text_parameters_set(&text_p, column_width*2, row_height*2, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);

        g_snprintf(buffer, 64, "Verbal Error");
        cairox_text_parameters_set(&text_p, column_width*3, row_height*2, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);

        g_snprintf(buffer, 64, "Total Error");
        cairox_text_parameters_set(&text_p, column_width*4, row_height*2, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);


        pangox_layout_set_font_size(layout, smaller_font_size);
        for (p = 0; p < P; p++) {
            g_snprintf(buffer, 64, "%s", get_name_string(p));
            cairox_text_parameters_set(&text_p, X_MARGIN*2, (p+4)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &text_p, layout, buffer);
        }
        pangox_layout_set_font_size(layout, bigger_font_size);
        g_snprintf(buffer, 64, "Total Error");
        cairox_text_parameters_set(&text_p, X_MARGIN*2, (n_rows-1)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);
        pangox_layout_set_font_size(layout, smaller_font_size);
        for (p = 0; p < P; p++) {
            g_snprintf(buffer, 64, "%f%%", error_view->Pattern_Error_Name[p]);
            cairox_text_parameters_set(&text_p, column_width, (p+4)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &text_p, layout, buffer);
        }
        pangox_layout_set_font_size(layout, bigger_font_size);
        g_snprintf(buffer, 64, "%f%%", error_view->Total_Stage_Error[NAME_ERROR]);
        cairox_text_parameters_set(&text_p, column_width, (n_rows-1)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);

        pangox_layout_set_font_size(layout, smaller_font_size);
        for (p = 0; p < P; p++) {
            g_snprintf(buffer, 64, "%f%%",  error_view->Pattern_Error_Visual[p]);
            cairox_text_parameters_set(&text_p, column_width*2, (p+4)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &text_p, layout, buffer);
        }
        pangox_layout_set_font_size(layout, bigger_font_size);
        g_snprintf(buffer, 64, "%f%%", error_view->Total_Stage_Error[VISUAL_ERROR]);
        cairox_text_parameters_set(&text_p, column_width*2, (n_rows-1)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);

        pangox_layout_set_font_size(layout, smaller_font_size);
        for (p = 0; p < P; p++) {
            g_snprintf(buffer, 64, "%f%%",  error_view->Pattern_Error_Verbal[p]);
            cairox_text_parameters_set(&text_p, column_width*3, (p+4)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &text_p, layout, buffer);
        }
        pangox_layout_set_font_size(layout, bigger_font_size);
        g_snprintf(buffer, 64, "%f%%", error_view->Total_Stage_Error[VERBAL_ERROR]);
        cairox_text_parameters_set(&text_p, column_width*3, (n_rows-1)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);

        pangox_layout_set_font_size(layout, smaller_font_size);
        for (p = 0; p < P; p++) {
            g_snprintf(buffer, 64, "%f%%", ((error_view->Pattern_Error_Name[p] + error_view->Pattern_Error_Visual[p] + error_view->Pattern_Error_Verbal[p]) / 3.0));
            cairox_text_parameters_set(&text_p, column_width*4, (p+4)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &text_p, layout, buffer);
        }
        pangox_layout_set_font_size(layout, bigger_font_size);
        g_snprintf(buffer, 64, "%f%%", (error_view->Total_Stage_Error[NAME_ERROR] + error_view->Total_Stage_Error[VISUAL_ERROR] + error_view->Total_Stage_Error[VERBAL_ERROR]) / 3.0);
        cairox_text_parameters_set(&text_p, column_width*4, (n_rows-1)* row_height, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &text_p, layout, buffer);






//         g_snprintf(buffer, 64, "Name: %s", get_name_string(get_clamped_pattern()));
//         cairox_text_parameters_set(&text_p, 220, height-20, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
//         cairox_paint_pango_text(cr, &text_p, layout, buffer);
// 	}
//         g_snprintf(buffer, 64, "Error: %f%%%%",  calculate_rms_error(pattern_number));
//         cairox_text_parameters_set(&text_p, 520, height-20, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
//         cairox_paint_pango_text(cr, &text_p, layout, buffer);


        g_object_unref(layout);

        cairo_destroy(cr);
    }

}

void gtk_error_view_reset(GtkError_View *error_view)
{

}

static void gtk_error_view_destroy(GtkObject *object)
{
//     GtkError_View *error_view;
    GtkError_ViewClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_ERROR_VIEW(object));

//     error_view = GTK_ERROR_VIEW(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_error_view_redraw(GtkWidget *widget)
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