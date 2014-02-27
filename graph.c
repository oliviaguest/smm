#include "graph.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_graph_class_init(GtkGraphClass *klass);
GtkType gtk_graph_get_type(void);
static void gtk_graph_init(GtkGraph *graph);
static void gtk_graph_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_graph_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_graph_realize(GtkWidget *widget);
static gboolean gtk_graph_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_graph_paint(GtkWidget *widget);
static void gtk_graph_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

GtkType gtk_graph_get_type(void)
{
    static GtkType gtk_graph_type = 0;
    if (!gtk_graph_type)
    {
        static const GtkTypeInfo gtk_graph_info = {"GtkGraph", sizeof(GtkGraph), sizeof(GtkGraphClass), (GtkClassInitFunc) gtk_graph_class_init, (GtkObjectInitFunc) gtk_graph_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_graph_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_graph_info);
    }
    return gtk_graph_type;
}

GtkWidget * gtk_graph_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_graph_get_type()));
}



static void gtk_graph_class_init(GtkGraphClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_graph_realize;
    widget_class->size_request = gtk_graph_size_request;
    widget_class->size_allocate = gtk_graph_size_allocate;
    widget_class->expose_event = gtk_graph_expose;
    object_class->destroy = gtk_graph_destroy;
}

static void gtk_graph_init(GtkGraph *graph)
{
    graph->MAX_I = 0;
    graph->max_j = 100;
    graph->step = 1;
}

void gtk_graph_set_max_iterations(GtkGraph *graph, int MAX_I)
{
    graph->MAX_I = MAX_I;
    if (graph->MAX_I > 20000)
    {
        graph->step = (int) (graph->MAX_I / 2000.0);
    }
    // printf("%s: MAX_I: %d\n", __FUNCTION__, MAX_I);
}

void gtk_graph_set_max_j(GtkGraph *graph, int max_j)
{
    graph->max_j = max_j;
}
void gtk_graph_set_y1(GtkGraph *graph, double y1[])
{
    graph->y1 = &y1[0];
}

void gtk_graph_set_y2(GtkGraph *graph, double y2[])
{
    graph->y2 = &y2[0];
}

void gtk_graph_set_y3(GtkGraph *graph, double y3[])
{
    graph->y3 = &y3[0];
}

void gtk_graph_set_ys(GtkGraph *graph, double y1[], double y2[], double y3[], double y4[])
{
    graph->y1 = &y1[0];
    graph->y2 = &y2[0];
    graph->y3 = &y3[0];
    graph->y4 = &y4[0];

}
static void gtk_graph_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_GRAPH(widget));
    g_return_if_fail(requisition != NULL);
    requisition->width = W;
    requisition->height = H;
}

static void gtk_graph_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_GRAPH(widget));
    g_return_if_fail(allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED(widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
    }
}

static void gtk_graph_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_GRAPH(widget));

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

static gboolean gtk_graph_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_GRAPH(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_graph_paint(widget);

    return FALSE;
}

static void draw_graph(cairo_t *cr, GtkWidget *widget)
{

    int width = widget->allocation.width;
    int height = widget->allocation.height;

    GtkGraph *graph = GTK_GRAPH(widget);

    if(graph->y1) {
        int i;
        double offset = 40.0;
        double x_width = width - (offset * 2);
        double y_height = height - (offset * 2);
        double x_space = x_width / (double) (graph->MAX_I - 1);
        double y_space = y_height / (double) graph->max_j;
        double x_axis = width - offset;
        double y_axis = height - offset;
        double y_offset = y_axis;
        int tick = (int) (graph->MAX_I * 0.1);
        CairoxTextParameters p;
        char buffer[64];
        PangoLayout *layout = pango_cairo_create_layout(cr);

        cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);

        // Transparent background
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
        cairo_paint(cr);


        cairo_set_line_width(cr, 2.0);


        cairo_set_source_rgb(cr, 0.5, 0.5, 0); //verbal error
        cairo_move_to(cr, offset, -y_space * graph->y4[0] + y_offset);
        for (i = 1; i < graph->MAX_I; i+=graph->step) {
            cairo_line_to(cr, offset + i*x_space, -y_space * graph->y4[i] + y_offset);
        }
        cairo_stroke(cr);

        cairo_set_source_rgb(cr, 0.1, 0.25, 0.75); //visual error
        cairo_move_to(cr, offset, -y_space * graph->y3[0] + y_offset);
        for (i = 1; i < graph->MAX_I; i+=graph->step) {
            cairo_line_to(cr, offset + i*x_space, -y_space * graph->y3[i] + y_offset);
        }
        cairo_stroke(cr);


        cairo_set_source_rgb(cr, 0.5, 0, 0.5); //name error
        cairo_move_to(cr, offset, -y_space * graph->y2[0] + y_offset);
        for (i = 1; i < graph->MAX_I; i+=graph->step) {
            cairo_line_to(cr, offset + i*x_space, -y_space * graph->y2[i] + y_offset);
        }
        cairo_stroke(cr);
//




        cairo_set_source_rgb(cr, 0.75, 0.1, 0.1); //total error
        cairo_move_to(cr, offset, -y_space * graph->y1[0] + y_offset);
        for (i = 1; i < graph->MAX_I; i+=graph->step) {
            cairo_line_to(cr, offset + i*x_space, -y_space * graph->y1[i] + y_offset);
        }
        cairo_stroke(cr);

// 	cairo_set_source_rgb(cr, 0.75, 0.1, 0.1); //total error
// 	cairo_move_to(cr, offset, -y_space * Total_Error[0] + y_offset);
// 	for (i = 1; i < graph->MAX_I; i+=graph->step) {
// 	  cairo_line_to(cr, offset + i*x_space, -y_space * Total_Error[i] + y_offset);
// 	}
// 	cairo_stroke(cr);


        cairo_stroke(cr);
        //
        //draw legend
        pangox_layout_set_font_size(layout, 13);

        g_snprintf(buffer, 64, "%s", "Total Error:");
        cairox_text_parameters_set(&p, 2*offset, 2*offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_set_source_rgb(cr, 0.75, 0.1, 0.1);
        cairo_move_to(cr, 4.5*offset-5, 2*offset+1);
        cairo_line_to(cr, 6*offset-5, 2*offset+1);
        cairo_stroke(cr);

        g_snprintf(buffer, 64, "%s", "Name Error:");
        cairox_text_parameters_set(&p, 2*offset, 2.5*offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_set_source_rgb(cr, 0.5, 0, 0.5);
        cairo_move_to(cr, 4.5*offset-5, 2.5*offset+1);
        cairo_line_to(cr, 6*offset-5, 2.5*offset+1);
        cairo_stroke(cr);

        g_snprintf(buffer, 64, "%s", "Visual Error:");
        cairox_text_parameters_set(&p, 2*offset, 3*offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_set_source_rgb(cr, 0.1, 0.25, 0.75);
        cairo_move_to(cr, 4.5*offset-5, 3*offset+1);
        cairo_line_to(cr, 6*offset-5, 3*offset+1);
        cairo_stroke(cr);

        g_snprintf(buffer, 64, "%s", "Verbal Error:");
        cairox_text_parameters_set(&p, 2*offset, 3.5*offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_set_source_rgb(cr, 0.5, 0.5, 0);
        cairo_move_to(cr, 4.5*offset-5, 3.5*offset+1);
        cairo_line_to(cr, 6*offset-5, 3.5*offset+1);
        cairo_stroke(cr);

        // draw axes
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width (cr, 2.5);
        cairo_move_to(cr, x_axis, y_axis);
        cairo_line_to(cr, offset, y_axis);
        cairo_move_to(cr, offset, offset);
        cairo_line_to(cr, offset, y_axis);
        cairo_stroke(cr);

        // draw axes' ticks
        cairo_set_line_width (cr, 1.0);

        for (i = graph->max_j; i >= 0; i-=10) {
            cairo_move_to(cr, offset + 1.0, offset + i*y_space + 0.5);
            cairo_line_to(cr, offset - 4.0, offset + i*y_space + 0.5);
            g_snprintf(buffer, 64, "%i", graph->max_j-i);
            cairox_text_parameters_set(&p, offset - 6.0, offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }

        cairo_stroke(cr);

        for (i = 0; i <= graph->MAX_I; i++) {
// 		      if ((i+1) == graph->MAX_I)
// 		      {
// 			    cairo_move_to(cr, offset + i*x_space, y_axis - 1.0);
// 			    cairo_line_to(cr, offset + i*x_space, y_axis + 4.0);
// 			    g_snprintf(buffer, 64, "%i", i+1);
// 			    cairox_text_parameters_set(&p, offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
// 			    cairox_paint_pango_text(cr, &p, layout, buffer);
// 		      }
// 		      else
// 		      {
            if (tick && !(i % tick)) {
                cairo_move_to(cr, offset + i*x_space, y_axis - 1.0);
                cairo_line_to(cr, offset + i*x_space, y_axis + 4.0);
                g_snprintf(buffer, 64, "%i", (int) (i));

                cairox_text_parameters_set(&p, offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text(cr, &p, layout, buffer);
            }
            else if (graph->MAX_I < 10)
            {
                cairo_move_to(cr, offset + i*x_space, y_axis - 1.0);
                cairo_line_to(cr, offset + i*x_space, y_axis + 4.0);
                g_snprintf(buffer, 64, "%i", (int) (i));
                cairox_text_parameters_set(&p, offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text(cr, &p, layout, buffer);
            }
// 		      }
        }
        cairo_stroke(cr);

        // axes labels
        g_snprintf(buffer, 64, "%% Error");
        cairox_text_parameters_set(&p, offset - 30.0, y_axis/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "Epoch");
        cairox_text_parameters_set(&p, x_axis/2.0, y_axis + 27.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        cairo_stroke(cr);

        g_object_unref(layout);
        cairo_destroy(cr);
    }
}
void gtk_graph_pdf(GtkWidget *widget, char *filename)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = widget->allocation.width;
    int height = widget->allocation.height;

// 	const char *name = gtk_widget_get_name(widget);
    surface = cairo_pdf_surface_create(filename, width, height);
    cr = cairo_create(surface);

    // draw graph
    draw_graph(cr, widget);


    cairo_surface_destroy(surface);

}


static void gtk_graph_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    draw_graph(cr, widget);
}

void gtk_graph_reset(GtkGraph *graph)
{
    graph->MAX_I = 0;
    gtk_graph_redraw(GTK_WIDGET(graph));
}

static void gtk_graph_destroy(GtkObject *object)
{
//     GtkGraph *graph;
    GtkGraphClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_GRAPH(object));

//     graph = GTK_GRAPH(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_graph_set_x(GtkGraph *graph, int MAX_I)
{
    graph->MAX_I = MAX_I;

}

void gtk_graph_redraw(GtkWidget *widget)
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
