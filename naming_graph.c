#include "naming_graph.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_naming_graph_class_init(GtkNaming_GraphClass *klass);
GtkType gtk_naming_graph_get_type(void);
static void gtk_naming_graph_init(GtkNaming_Graph *naming_graph);
static void gtk_naming_graph_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_naming_graph_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_naming_graph_realize(GtkWidget *widget);
static gboolean gtk_naming_graph_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_naming_graph_paint(GtkWidget *widget);
static void gtk_naming_graph_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

static char buffer[64];
static const double superordinate_dash[1] = {5.0};
static const double crossdomain_dash[2] = {1.0, 4.0};
static const double omission_dash[4] = {2.0, 2.0, 7.0, 2.0};

#define BLACK		0.2, 0.2, 0.2
#define RED		BLACK /*0.9, 0.1, 0.1*/
#define PURPLE		BLACK /*0.66, 0.1, 0.66*/
#define BLUE		BLACK /*0.1, 0.25, 0.9*/
#define GREEN		BLACK /*0.66, 0.66, 0.1*/

static double x_width;
static double y_height;
static double x_segments;
static double y_segments;
static double x_space;
static double y_space;
static double x_axis;
static double y_axis;
static double radius = 3.0;


GtkType gtk_naming_graph_get_type(void)
{
    static GtkType gtk_naming_graph_type = 0;
    if (!gtk_naming_graph_type)
    {
        static const GtkTypeInfo gtk_naming_graph_info = {"GtkNaming_Graph", sizeof(GtkNaming_Graph), sizeof(GtkNaming_GraphClass), (GtkClassInitFunc) gtk_naming_graph_class_init, (GtkObjectInitFunc) gtk_naming_graph_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_naming_graph_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_naming_graph_info);
    }
    return gtk_naming_graph_type;
}

GtkWidget * gtk_naming_graph_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_naming_graph_get_type()));
}



static void gtk_naming_graph_class_init(GtkNaming_GraphClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_naming_graph_realize;
    widget_class->size_request = gtk_naming_graph_size_request;
    widget_class->size_allocate = gtk_naming_graph_size_allocate;
    widget_class->expose_event = gtk_naming_graph_expose;
    object_class->destroy = gtk_naming_graph_destroy;
}
static char y_label[4][100];
double x_offset;
double y_offset;
int n_pattern[2] = {0, 0};
double point_x[NAMING_LESION_LEVEL];
double point_y_semantic[NAMING_LESION_LEVEL][2];
double point_y_superordinate[NAMING_LESION_LEVEL][2];
double point_y_crossdomain[NAMING_LESION_LEVEL][2];
double point_y_omission[NAMING_LESION_LEVEL][2];
double value_correct[NAMING_LESION_LEVEL];
double value_semantic[NAMING_LESION_LEVEL][2];
double value_superordinate[NAMING_LESION_LEVEL][2];
double value_crossdomain[NAMING_LESION_LEVEL][2];
double value_omission[NAMING_LESION_LEVEL][2];

static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    double coordx = 0, coordy = 0;
//     GtkNaming_Graph *naming_graph = GTK_NAMING_GRAPH(widget);

    gtk_naming_graph_redraw(widget);



    if (event->button == 1) {
        coordx = event->x;
        coordy = event->y;
    }
    //     printf("x = %d y = %d\n", coordx, coordy);

    //     if (event->button == 3) {
    //         gtk_widget_queue_draw(widget);
    //     }

    PangoLayout *layout;
    CairoxTextParameters text_p;
    char buffer[64];

    cairo_t *cr;
    int i;

    printf("x = %f, y = %f\n", coordx, coordy);


    cr = gdk_cairo_create (widget->window);

    /* Transparent background */
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint (cr);
    layout = pango_cairo_create_layout (cr);

    for (i = 0; i < naming_lesion_level; i++) {

//         printf("%d x = %f, y = %f x1 = %f, x2 = %f y1 = %f, y2 = %f value = %f\n", i, coordx, coordy, point_x[i] - radius*1.5, point_x[i] + radius*1.5, point_y_omission[i][0] - radius*1.5, point_y_omission[i][0] + radius*1.5, value_omission[i][0]);



        /*
        	  printf("y1 = %f, y2 = %f\n", y_axis, point_y[i][0] + y_axis);
                  printf("y3 = %f, y4 = %f\n", y_axis, point_y[i][2] + y_axis);*/



        if ((coordx >= (point_x[i] - radius*1.5)) && (coordx <= (point_x[i] + radius*1.5))) {

            if ((coordy >= (point_y_semantic[i][0] - radius*1.5)) && (coordy <= (point_y_semantic[i][0] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_semantic[i][0], i * 100.0 / (double) naming_lesion_level);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_semantic[i][0] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }
            if ((coordy >= (point_y_semantic[i][1] - radius*1.5)) && (coordy <= (point_y_semantic[i][1] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_semantic[i][1], i * 100.0 / (double) naming_lesion_level);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_semantic[i][1] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;
            }


            if ((coordy >= (point_y_superordinate[i][0] - radius*1.5)) && (coordy <= (point_y_superordinate[i][0] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_superordinate[i][0], i * 100.0 / (double) naming_lesion_level);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_superordinate[i][0] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }
            if ((coordy >= (point_y_superordinate[i][1] - radius*1.5)) && (coordy <= (point_y_superordinate[i][1] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_superordinate[i][1], i * 100.0 / (double) naming_lesion_level);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_superordinate[i][1] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;
            }


            if ((coordy >= (point_y_omission[i][0] - radius*1.5)) && (coordy <= (point_y_omission[i][0] + radius*1.5))) {


                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_omission[i][0], i * 100.0 / (double) naming_lesion_level);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_omission[i][0] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }
            if ((coordy >= (point_y_omission[i][1] - radius*1.5)) && (coordy <= (point_y_omission[i][1] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_omission[i][1], i * 100.0 / (double) naming_lesion_level);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_omission[i][1] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;
            }


            if ((coordy >= (point_y_crossdomain[i][0] - radius*1.5)) && (coordy <= (point_y_crossdomain[i][0] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_crossdomain[i][0], i * 100.0 / (double) naming_lesion_level);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_crossdomain[i][0] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }
            if ((coordy >= (point_y_crossdomain[i][1] - radius*1.5)) && (coordy <= (point_y_crossdomain[i][1] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.2f, %1.1f%%", value_correct[i], value_crossdomain[i][1], i * 100.0 / (double) naming_lesion_level);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_crossdomain[i][1] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;
            }
        }


        /*
        	    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.5);
        	    cairo_move_to(cr, point_x[i], point_y_semantic[i][0]);
                    cairo_arc(cr, point_x[i], point_y_semantic[i][0], radius, 0.0, 2*M_PI);
        	    cairo_move_to(cr, point_x[i], point_y_semantic[i][1]);
                    cairo_arc(cr, point_x[i], point_y_semantic[i][1], radius, 0.0, 2*M_PI);

                    cairo_arc(cr, point_x[i], point_y_omission[i][0], radius, 0.0, 2*M_PI);
                    cairo_arc(cr, point_x[i], point_y_omission[i][1], radius, 0.0, 2*M_PI);
                    cairo_arc(cr, point_x[i], point_y_crossdomain[i][0], radius, 0.0, 2*M_PI);
                    cairo_arc(cr, point_x[i], point_y_crossdomain[i][1], radius, 0.0, 2*M_PI);
                    cairo_arc(cr, point_x[i], point_y_superordinate[i][0], radius, 0.0, 2*M_PI);
                    cairo_arc(cr, point_x[i], point_y_superordinate[i][1], radius, 0.0, 2*M_PI);

        	    cairo_stroke(cr);
        	    */

    }



    g_object_unref (layout);
    cairo_destroy (cr);

    return TRUE;
}

static void gtk_naming_graph_init(GtkNaming_Graph *naming_graph)
{

    int l, domain;




    for (l = 0; l < NAMING_LESION_LEVEL; l++) {
        naming_graph->correct[l] = 0.0;
        for (domain = 0; domain < N_DOMAIN; domain++) {
            naming_graph->semantic[l][domain] = 0.0;
            naming_graph->superordinate[l][domain] = 0.0;
            naming_graph->crossdomain[l][domain] = 0.0;
            naming_graph->omission[l][domain] = 0.0;



        }


    }
    sprintf(y_label[0], "Omission");
    sprintf(y_label[1], "Semantic");
    sprintf(y_label[2], "Superordinate");
    sprintf(y_label[3], "Crossdomain");

    naming_graph->type = 0;
    naming_graph->legend = TRUE;

    if (EXCLUDE_FRUIT) {

        n_pattern[0] = get_domain_n_pattern(0) - get_n_fruit();// - get_domain_n_general_names(0);// animals
        n_pattern[1] = get_domain_n_pattern(1);// - get_domain_n_general_names(1); // artifacts

    }
    else
    {

        n_pattern[0] = get_domain_n_pattern(0);// - get_domain_n_general_names(0);
        n_pattern[1] = get_domain_n_pattern(1);// - get_domain_n_general_names(1);

    }


    n_pattern[0] -= get_domain_n_general_names(0);
    n_pattern[1] -= get_domain_n_general_names(1);

    gtk_widget_add_events(GTK_WIDGET(naming_graph), GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(naming_graph), "button-press-event", G_CALLBACK(clicked), NULL);

}

void gtk_naming_graph_set_type(GtkNaming_Graph *naming_graph, int type)
{
    naming_graph->type = type;

}

void gtk_naming_graph_set_legend(GtkNaming_Graph *naming_graph, int legend)
{
    naming_graph->legend = legend;

}

void gtk_naming_graph_set_data_type(GtkNaming_Graph *naming_graph, int type)
{
    naming_graph->data_type = type;

    gtk_naming_graph_update(naming_graph);
    gtk_naming_graph_redraw(GTK_WIDGET(naming_graph));

}

void gtk_naming_graph_set_way(GtkNaming_Graph *naming_graph, int value)
{
    naming_graph->way = value;

    printf("Way: %d\n", naming_graph->way);


    gtk_naming_graph_update(naming_graph);
    gtk_naming_graph_redraw(GTK_WIDGET(naming_graph));

}


static void gtk_naming_graph_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_NAMING_GRAPH(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 800;

//     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_naming_graph_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_NAMING_GRAPH(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

//     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_naming_graph_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_NAMING_GRAPH(widget));

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

static gboolean gtk_naming_graph_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_NAMING_GRAPH(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_naming_graph_paint(widget);

    return FALSE;
}

static double semantic[NAMING_LESION_LEVEL], superordinate[NAMING_LESION_LEVEL], crossdomain[NAMING_LESION_LEVEL], omission[NAMING_LESION_LEVEL];
extern double activation_threshold;

void gtk_naming_graph_update(GtkNaming_Graph *naming_graph)
{

    gtk_naming_graph_init(naming_graph);

    /*
        double sample_mean_superordinate[NAMING_LESION_DISTRIBUTION][NAMING_LESION_LEVEL][NAMING_SAMPLE], sample_mean_crossdomain[NAMING_LESION_DISTRIBUTION][NAMING_LESION_LEVEL][NAMING_SAMPLE], sample_mean_semantic[NAMING_LESION_DISTRIBUTION][NAMING_LESION_LEVEL][NAMING_SAMPLE], sample_mean_omission[NAMING_LESION_DISTRIBUTION][NAMING_LESION_LEVEL][NAMING_SAMPLE];
                                                                                                                                                                                           */


// if (naming_graph->type == 0) {
    int s, p, q, l, d, domain, r; // p is target, q is network's response
    for (l = 0; l < NAMING_LESION_LEVEL; l++) {
        naming_graph->correct[l] = 0.0;
        for (domain = 0; domain < N_DOMAIN; domain++) {
            naming_graph->semantic[l][domain] = 0.0;
            naming_graph->superordinate[l][domain] = 0.0;
            naming_graph->crossdomain[l][domain] = 0.0;
            naming_graph->omission[l][domain] = 0.0;
            point_x[l] = 0.0;
            point_y_semantic[l][domain] = 0.0;
            point_y_superordinate[l][domain] = 0.0;
            point_y_crossdomain[l][domain] = 0.0;
            point_y_omission[l][domain] = 0.0;
            value_correct[l] = 0.0;
            value_semantic[l][domain] = 0.0;
            value_superordinate[l][domain] = 0.0;
            value_crossdomain[l][domain] = 0.0;
            value_omission[l][domain] = 0.0;

        }
//         for (d = 0; d < NAMING_LESION_DISTRIBUTION; d++) {
// 	  for (s = 0; s < NAMING_SAMPLE; s++) {
// 	    sample_mean_superordinate[d][l][s] = 0.0;
// 	    sample_mean_crossdomain[d][l][s] = 0.0;
// 	    sample_mean_semantic[d][l][s] = 0.0;
// 	    sample_mean_omission[d][l][s] = 0.0;
// 	  }
// 	}
    }
    printf("Updating using way: %d\n", naming_graph->way);


    if (naming_graph->data_type == MODEL) {
        for (d = 0; d < naming_lesion_distribution; d++) {
            for (l = 0; l < naming_lesion_level; l++) {
                for (p = 0; p < P; p++) {
                    if (!excluded_pattern(p)) {
                        if (!is_general_name(p))
                        {   //new
                            for (s = 0; s < naming_sample; s++) {


                                if (naming_graph->way == 0) {
                                    if (naming_response[d][l][s][p][1] >= activation_threshold) {

                                        q = (int) naming_response[d][l][s][p][0];
                                        if (p != q) {
                                            if (is_general_name(q) && (get_category(p) == get_category(q))) {
                                                (naming_graph->superordinate[l][get_domain(p)])++;

                                            }
                                            else if (get_domain(p) != get_domain(q)) {
                                                (naming_graph->crossdomain[l][get_domain(p)])++;

                                            }
                                            else if (get_domain(p) == get_domain(q)) {
                                                (naming_graph->semantic[l][get_domain(p)])++;
                                            }
                                        }
                                    } else {
                                        (naming_graph->omission[l][get_domain(p)])++;
                                        // 				sample_mean_omission[d][l][s]++;
                                    }


                                } else if (naming_graph->way == 1) {

                                    r = (int) naming_response_1[d][l][s][p][0];
                                    if (naming_response_1[d][l][s][p][1] >= activation_threshold) {

// 					if (r == NAMING_CORRECT) {
// 					}
                                        if (r == NAMING_SUPERORDINATE) {
                                            (naming_graph->superordinate[l][get_domain(p)])++;

                                        }
                                        else if (r == NAMING_CROSSDOMAIN) {
                                            (naming_graph->crossdomain[l][get_domain(p)])++;

                                        }
                                        else if (r == NAMING_SEMANTIC) {
                                            (naming_graph->semantic[l][get_domain(p)])++;

                                        }
                                    }
                                    else
                                    {
                                        (naming_graph->omission[l][get_domain(p)])++;

                                    }

                                } else if (naming_graph->way == 2) {
                                    if (naming_response_alt[d][l][s][p][1] >= activation_threshold) {

                                        q = (int) naming_response_alt[d][l][s][p][0];
                                        if (p != q) {
                                            if (is_general_name(q) && (get_domain(p) == get_domain(q))) {
                                                (naming_graph->superordinate[l][get_domain(p)])++;

                                            }
                                            else if (get_domain(p) != get_domain(q)) {
                                                (naming_graph->crossdomain[l][get_domain(p)])++;

                                            }
                                            else if (get_domain(p) == get_domain(q)) {
                                                (naming_graph->semantic[l][get_domain(p)])++;
                                            }
                                        }
                                    } else {
                                        (naming_graph->omission[l][get_domain(p)])++;
                                        // 				sample_mean_omission[d][l][s]++;
                                    }
                                }






                            }
                        }

                    }
                }
            }
        }


        for (l = 0; l < naming_lesion_level; l++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {
                naming_graph->semantic[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_graph->superordinate[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_graph->crossdomain[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_graph->omission[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);

            }

        }





        for (l = 0; l < naming_lesion_level; l++) {
            semantic[l] = (naming_graph->semantic[l][0] * n_pattern[0]  + naming_graph->semantic[l][1] * n_pattern[1]) / (double) (n_pattern[0] + n_pattern[1]);
            superordinate[l] = (naming_graph->superordinate[l][0] * n_pattern[0]  + naming_graph->superordinate[l][1] * n_pattern[1]) / (double) (n_pattern[0] + n_pattern[1]);
            crossdomain[l] = (naming_graph->crossdomain[l][0] * n_pattern[0]  + naming_graph->crossdomain[l][1] * n_pattern[1]) / (double) (n_pattern[0] + n_pattern[1]);
            omission[l] = (naming_graph->omission[l][0] * n_pattern[0]  + naming_graph->omission[l][1] * n_pattern[1]) / (double) (n_pattern[0] + n_pattern[1]);
            for (domain = 0; domain < N_DOMAIN; domain++) {
                naming_graph->correct[l] += n_pattern[domain] * (naming_graph->semantic[l][domain] + naming_graph->superordinate[l][domain] + naming_graph->crossdomain[l][domain] + naming_graph->omission[l][domain]);

            }
            naming_graph->correct[l] /=  (double) (n_pattern[0] + n_pattern[1]);
            naming_graph->correct[l] = 1.0 - naming_graph->correct[l];
            if (naming_graph->correct[l] < 0.0) {
                printf("Error: Naming response proportions do not add up to 1.0!\n");
            }





        }
    }
    else if (naming_graph->data_type == PATIENTS) {




        naming_lesion_level = 4;

        semantic[3] = 0.0899;
        semantic[2] = 0.2410;
        semantic[1] = 0.1691;
        semantic[0] = 0.0665;


        superordinate[3] =    0.0090 ;
        superordinate[2] =0.0629;
        superordinate[1] = 0.1385;
        superordinate[0] = 0.0576;


        crossdomain[3] =    0.00;
        crossdomain[2] =0.0036;
        crossdomain[1] = 0.0018;
        crossdomain[0] = 0.0054;







        omission[3] =    0.1025;
        omission[2] =    0.3129;
        omission[1] =    0.5594;
        omission[0] =     0.8219;
        naming_graph->correct[0] = 0.0451;
        naming_graph->correct[1] = 0.1270;
        naming_graph->correct[2] = 0.3778;
        naming_graph->correct[3] = 0.7927;

    }
    else if (naming_graph->data_type == ROGERS) {








        naming_lesion_level = 4;
        semantic[3] =    0.0500;
        semantic[2] =    0.2031;
        semantic[1] =    0.2241;
        semantic[0] =    0.0804;




        superordinate[3] =        0.0196 ;
        superordinate[2] =   0.0866;
        superordinate[1] =     0.0987;
        superordinate[0] =    0.0535;





        crossdomain[3] =    0.0052;
        crossdomain[2] =    0.0113;
        crossdomain[1] =    0.0163;
        crossdomain[0] =     0.0123;










        omission[3] =     0.0662;
        omission[2] =        0.3178;
        omission[1] =        0.4697;
        omission[0] =         0.8402;





        naming_graph->correct[0] =     0.0116;
        naming_graph->correct[1] =
            0.1898;
        naming_graph->correct[2] =    0.3816;
        naming_graph->correct[3] =     0.8639;

    }
}



static void draw_naming_subgraph(cairo_t *cr, GtkNaming_Graph *naming_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p, int subgraph)
{

    x_width = width - (x_offset * 2);
    y_height = height - (y_offset * 2);
    x_segments = (x_width > 100) ? 10.0 : 5.0;
    y_segments = (y_height > 100) ? 10.0 : 5.0;
    x_space = x_width / x_segments;
    y_space = y_height / y_segments;
    x_axis = width - x_offset;
    y_axis = height - y_offset;
//   double y_offset = y_axis;

//   printf("x_width = %f x_segments = %f x_space = %f\n", x_width, x_segments, x_space);


    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);

    // 	  if(naming_graph->y1){
    int i;

    // Transparent background
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width(cr, 2.0);

    if (naming_graph->type == 0) {

        cairo_set_source_rgb(cr, RED);
        cairo_set_dash (cr, omission_dash, 0, 0);
        cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height *  semantic[0] + y_axis);

        for (i = 0; i < naming_lesion_level; i++) {
            cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * semantic[i] + y_axis);


        }
        cairo_stroke(cr);



        for (i = 0; i < naming_lesion_level; i++) {
            cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * semantic[i] + y_axis, radius, 0.0, 2*M_PI);
            cairo_fill(cr);

            point_x[i] = x_axis - naming_graph->correct[i] * x_width;

            value_correct[i] = naming_graph->correct[i];


            point_y_semantic[i][0] = -y_height * semantic[i] + y_axis;

            value_semantic[i][0] = semantic[i];
        }
        cairo_set_source_rgb(cr, PURPLE);
        cairo_set_dash (cr, superordinate_dash, 1, 0);
        cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * superordinate[0] + y_axis);
        for (i = 0; i < naming_lesion_level; i++) {
            cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * superordinate[i] + y_axis);
//             printf("super : %f, %f\n", naming_graph->correct[i], superordinate[i]);
        }
        cairo_stroke(cr);


        for (i = 0; i < naming_lesion_level; i++) {
            cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * superordinate[i] + y_axis, radius, 0.0, 2*M_PI);
            cairo_fill(cr);
            point_y_superordinate[i][0] = -y_height * superordinate[i] + y_axis;
            value_superordinate[i][0] = superordinate[i];
        }

        cairo_set_source_rgb(cr, BLUE);
        cairo_set_dash (cr, crossdomain_dash, 2, 0);
        cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * crossdomain[0] + y_axis);
        for (i = 0; i < naming_lesion_level; i++) {
            cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * crossdomain[i]  + y_axis);
//             printf("cross : %f, %f\n", naming_graph->correct[i], crossdomain[i]);

        }
        cairo_stroke(cr);


        for (i = 0; i < naming_lesion_level; i++) {
            cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * crossdomain[i] + y_axis, radius, 0.0, 2*M_PI);
            cairo_fill(cr);
            point_y_crossdomain[i][0] = -y_height * crossdomain[i] + y_axis;
            value_crossdomain[i][0] = crossdomain[i];

        }

        cairo_set_source_rgb(cr, GREEN);
        cairo_set_dash (cr, omission_dash, 4, 0);
        cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * omission[0] + y_axis);
        for (i = 0; i < naming_lesion_level; i++) {
            cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * omission[i] + y_axis);
//             printf("omission : %f, %f\n", naming_graph->correct[i], omission[i]);
        }

        cairo_stroke(cr);


        for (i = 0; i < naming_lesion_level; i++) {
            cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * omission[i] + y_axis, radius, 0.0, 2*M_PI);
            cairo_fill(cr);
            point_y_omission[i][0] = -y_height * omission[i] + y_axis;
            value_omission[i][0] = omission[i];
        }

    }
    else
    {
// 	  0 = omissions, 1 = semantic, 2 = superordinate, 3 = crossdomain

        if (subgraph == 0) { //omissions
            cairo_set_source_rgb(cr, GREEN);
            cairo_set_dash (cr, superordinate_dash, 0, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->omission[0][0]  + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->omission[i][0]  + y_axis);
//                 printf("correct = %f, omission = %f\n", naming_graph->correct[i], naming_graph->omission[i][0]);
            }
            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->omission[i][0] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_x[i] = x_axis - naming_graph->correct[i] * x_width;
                value_correct[i] = naming_graph->correct[i];
                point_y_omission[i][0] = -y_height * naming_graph->omission[i][0] + y_axis + (height * subgraph) - 10.0 - radius;
//                 if (i == 1) {
//                     printf("0 %d drawn at = %f\n", i, point_y_omission[i][0]);
//                 }

                value_omission[i][0] = naming_graph->omission[i][0];
//                 printf("%d x1 = %f, x2 = %f y1 = %f, y2 = %f value = %f\n", i, point_x[i] - radius*1.5, point_x[i] + radius*1.5, point_y_omission[i][0] - radius*1.5, point_y_omission[i][0] + radius*1.5, value_omission[i][0]);

            }


            cairo_set_dash (cr, superordinate_dash, 1, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->omission[0][1]  + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->omission[i][1]  + y_axis);
            }

            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->omission[i][1] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_y_omission[i][1] = -y_height * naming_graph->omission[i][1] + y_axis + (height * subgraph) - 10.0 - radius;
                value_omission[i][1] = naming_graph->omission[i][1];
//                 if (i == 1) {
//                     printf("1 %d drawn at = %f\n", i, point_y_omission[i][1]);
//                 }

            }

        }
        else if (subgraph == 1) { //semantic
            cairo_set_source_rgb(cr, RED);
            cairo_set_dash (cr, superordinate_dash, 0, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->semantic[0][0] + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->semantic[i][0] + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->semantic[i][0] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_y_semantic[i][0] = -y_height * naming_graph->semantic[i][0] + y_axis + (height * subgraph) - 10.0 - radius;
                value_semantic[i][0] = naming_graph->semantic[i][0];

//                 if (i == 1) {
//                     printf("1 %d drawn at = %f\n", i, point_y_semantic[i][0]);
//                 }
            }

            cairo_set_source_rgb(cr, RED);
            cairo_set_dash (cr, superordinate_dash, 1, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->semantic[0][1] + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->semantic[i][1] + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->semantic[i][1] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_y_semantic[i][1] = -y_height * naming_graph->semantic[i][1] + y_axis + (height * subgraph) - 10.0 - radius;
                value_semantic[i][1] = naming_graph->semantic[i][1];


//                 if (i == 1) {
//                     printf("1 %d drawn at = %f\n", i, point_y_semantic[i][1]);
//                 }

            }
        }
        else if (subgraph == 2) {
            cairo_set_source_rgb(cr, PURPLE);
            cairo_set_dash (cr, superordinate_dash, 0, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->superordinate[0][0] + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->superordinate[i][0] + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->superordinate[i][0] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_y_superordinate[i][0] = -y_height * naming_graph->superordinate[i][0] + y_axis + (height * subgraph) - 10.0 - radius;
                value_superordinate[i][0] = naming_graph->superordinate[i][0];

            }

            cairo_set_source_rgb(cr, PURPLE);
            cairo_set_dash (cr, superordinate_dash, 1, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->superordinate[0][1] + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->superordinate[i][1] + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->superordinate[i][1] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_y_superordinate[i][1] = -y_height * naming_graph->superordinate[i][1] + y_axis + (height * subgraph) - 10.0 - radius;
                value_superordinate[i][1] = naming_graph->superordinate[i][1];


            }
        }
        else if (subgraph == 3) {
            cairo_set_source_rgb(cr, BLUE);
            cairo_set_dash (cr, superordinate_dash, 0, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->crossdomain[0][0] + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->crossdomain[i][0] + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->crossdomain[i][0] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_y_crossdomain[i][0] = -y_height * naming_graph->crossdomain[i][0] + y_axis + (height * subgraph) - 10.0 - radius;
                value_crossdomain[i][0] = naming_graph->crossdomain[i][0];
            }

            cairo_set_source_rgb(cr, BLUE);
            cairo_set_dash (cr, superordinate_dash, 1, 0);
            cairo_move_to(cr, x_axis - naming_graph->correct[0] * x_width, -y_height * naming_graph->crossdomain[0][1] + y_axis);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_line_to(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->crossdomain[i][1] + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < naming_lesion_level; i++) {
                cairo_arc(cr, x_axis - naming_graph->correct[i] * x_width, -y_height * naming_graph->crossdomain[i][1] + y_axis, radius, 0.0, 2*M_PI);
                cairo_fill(cr);
                point_y_crossdomain[i][1] = -y_height * naming_graph->crossdomain[i][1] + y_axis + (height * subgraph) - 10.0 - radius;
                value_crossdomain[i][1] = naming_graph->crossdomain[i][1];


            }
        }

    }
    cairo_set_dash (cr, omission_dash, 0, 0);

    // draw axes
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width (cr, 1.0);
    cairo_move_to(cr, x_axis, y_axis);
    cairo_line_to(cr, x_offset, y_axis);
    cairo_move_to(cr, x_offset, y_offset);
    cairo_line_to(cr, x_offset, y_axis);
    cairo_stroke(cr);
    //
    // draw axes' ticks
    cairo_set_line_width (cr, 1.0);
    int step = 1;
    if (naming_graph->type != 0) {
        step = 2;
    }
    for (i = 0; i < ((int)y_segments + 1); i+=step) {
        cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
        cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
        g_snprintf(buffer, 64, "%1.1f", 1 - i/ (double) y_segments);
        cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
    }
    //
    cairo_stroke(cr);
    //
    for (i = 0; i < ((int)x_segments + 1); i++) {
        cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
        cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
        g_snprintf(buffer, 64, "%1.1f", 1 - i/(double) x_segments);
        cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
    }

}

static void draw_naming_legend(cairo_t *cr, GtkNaming_Graph *naming_graph, PangoLayout *layout, CairoxTextParameters p)
{
    pangox_layout_set_font_size(layout, 13);

    pangox_layout_select_font_from_string(layout, "CMU Serif");
    // 	//
    if (naming_graph->legend) {

        double text_width = 0.0;

//     if (naming_graph->data_type == PATIENTS) {

        //draw legend
        if (naming_graph->type == 0) {

            //
            g_snprintf(buffer, 64, "%s", "Semantic:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 2*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);

            g_snprintf(buffer, 64, "%s", "Superordinate:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 2.5*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);

            g_snprintf(buffer, 64, "%s", "Crossdomain:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 3*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);

            g_snprintf(buffer, 64, "%s", "Omission:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 3.5*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);


            cairo_set_source_rgb(cr, RED);
            cairo_set_dash (cr, omission_dash, 0, 0);
            cairo_move_to(cr, 2.5*x_offset + text_width, 2*x_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 2*x_offset+1);
            cairo_stroke(cr);

            cairo_set_source_rgb(cr, PURPLE);
            cairo_set_dash (cr, superordinate_dash, 1, 0);
            cairo_move_to(cr, 2.5*x_offset + text_width, 2.5*x_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 2.5*x_offset+1);
            cairo_stroke(cr);

            cairo_set_source_rgb(cr, BLUE);
            cairo_set_dash (cr, crossdomain_dash, 2, 0);
            cairo_move_to(cr, 2.5*x_offset + text_width, 3*x_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 3*x_offset+1);
            cairo_stroke(cr);

            cairo_set_source_rgb(cr, GREEN);
            cairo_set_dash (cr, omission_dash, 4, 0);
            cairo_move_to(cr, 2.5*x_offset + text_width, 3.5*x_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 3.5*x_offset+1);
            cairo_stroke(cr);
            //
        }
        else if (naming_graph->type == 1) {
            g_snprintf(buffer, 64, "%s", "Animals:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 1*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);


            g_snprintf(buffer, 64, "%s", "Artifacts:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 1.5*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);

            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_set_dash (cr, omission_dash, 0, 0);

            cairo_move_to(cr, 2.5*x_offset + text_width, 1*x_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 1*x_offset+1);
            cairo_stroke(cr);
            cairo_set_dash (cr, superordinate_dash, 1, 0);

            cairo_move_to(cr, 2.5*x_offset + text_width, 1.5*x_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 1.5*x_offset+1);
            cairo_stroke(cr);
        }
//     }
    }

}

static void draw_naming_axes_labels(cairo_t *cr, GtkNaming_Graph *naming_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p)
{

    if (naming_graph->type == 0)
    {
        g_snprintf(buffer, 64, "Probability of Errors by Type");
        cairox_text_parameters_set(&p, x_offset - 44.0, height/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_stroke(cr);


    }
    else
    {
        int i;
        double quarter_height = height / 4.0;
        for (i = 0; i < 4; i++) {

            g_snprintf(buffer, 64, "%s", y_label[i]);
            cairox_text_parameters_set(&p, x_offset - 40.0, y_offset + (quarter_height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_translate(cr, 0, quarter_height);
        }
        cairo_stroke(cr);

        cairo_translate(cr, 0, -height- y_offset/2.0);
        cairo_stroke(cr);


    }

    g_snprintf(buffer, 64, "Proportion Correct");
    cairox_text_parameters_set(&p, width/2.0, (height - y_offset) + 37.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
    cairox_paint_pango_text(cr, &p, layout, buffer);
    cairo_stroke(cr);


    //
}

static void draw_naming_graph(cairo_t *cr, GtkWidget *widget, double width, double height)
{


    GtkNaming_Graph *naming_graph =  GTK_NAMING_GRAPH(widget);
    PangoLayout *layout = pango_cairo_create_layout(cr);
    CairoxTextParameters p;


    x_offset = (naming_graph->type) ? 51.0 : 51.0;
    y_offset = (naming_graph->type) ? 25.0 : 50.0;


    draw_naming_legend(cr, GTK_NAMING_GRAPH(widget), layout, p);

    draw_naming_axes_labels(cr, naming_graph, width, height, layout, p);


    if (naming_graph->type == 0) {
        draw_naming_subgraph(cr, naming_graph, width, height, layout, p, -1);
    }
    else
    {
        int i;
        for (i = 0; i < 4; i++) {

            draw_naming_subgraph(cr, naming_graph, width, height / 4.0, layout, p, i);
            cairo_translate(cr, 0, height / 4.0);
        }

    }
    g_object_unref(layout);
    cairo_destroy(cr);

}
void gtk_naming_graph_pdf(GtkWidget *widget, char filename[])
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = 500;//widget->allocation.width;
    int height = 500;//widget->allocation.height;
    char name[100];
    int i = 0;
    GtkNaming_Graph *naming_graph = GTK_NAMING_GRAPH(widget);


    if (naming_graph->data_type == MODEL) {

        while (filename[i] != '\0') {
            name[i] = filename[i];
            i++;
        }
        name[i] = '\0';
        printf("%s\n", name);
        sprintf(name, "%s_naming.pdf", name);
    }
    else if (naming_graph->data_type == ROGERS) {
        sprintf(name, "rogers_model_naming.pdf");
    }
    else if (naming_graph->data_type == PATIENTS) {
        sprintf(name, "rogers_patients_naming.pdf");
    }


    surface = cairo_pdf_surface_create(name, width, height);
    cr = cairo_create(surface);

    // draw naming_graph
    draw_naming_graph(cr, widget, width, height);


    cairo_surface_destroy(surface);

}


static void gtk_naming_graph_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);

    draw_naming_graph(cr, widget, (double) widget->allocation.width, (double) widget->allocation.height);
}

void gtk_naming_graph_reset(GtkNaming_Graph *naming_graph)
{
    gtk_naming_graph_init(naming_graph);
    gtk_naming_graph_redraw(GTK_WIDGET(naming_graph));
}

static void gtk_naming_graph_destroy(GtkObject *object)
{
//     GtkNaming_Graph *naming_graph;
    GtkNaming_GraphClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_NAMING_GRAPH(object));

//     naming_graph = GTK_NAMING_GRAPH(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_naming_graph_redraw(GtkWidget *widget)
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

void gtk_naming_graph_clear(GtkWidget *widget)
{
    init_naming_response(0, 0, 0);
    init_naming_response_1(0, 0, 0);
    init_naming_response_alt(0, 0, 0);
    gtk_naming_graph_reset(GTK_NAMING_GRAPH(widget));

}



