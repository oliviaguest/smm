#include "sorting_graph.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_sorting_graph_class_init(GtkSorting_GraphClass *klass);
GtkType gtk_sorting_graph_get_type(void);
static void gtk_sorting_graph_init(GtkSorting_Graph *sorting_graph);
static void gtk_sorting_graph_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_sorting_graph_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_sorting_graph_realize(GtkWidget *widget);
static gboolean gtk_sorting_graph_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_sorting_graph_paint(GtkWidget *widget);
static void gtk_sorting_graph_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

static char buffer[64];
static const double superordinate_dash[1] = {5.0};
static const double crossdomain_dash[2] = {1.0, 4.0};
static const double omission_dash[4] = {1.0, 1.0, 5.0, 1.0};

static double radius = 3.0;
double x_width;
double y_height;
double x_segments;
double y_segments;
double x_space;
double y_space;
double x_axis;
double y_axis;

#define BLACK		0.2, 0.2, 0.2



#define WORDS_COLOUR  BLACK
#define PICTURES_COLOUR  0.6, 0.6, 0.6

#define RED  0.75, 0.1, 0.1
#define BLUE  0.1, 0.25, 0.75

double point_x[SORTING_LESION_LEVEL];
double point_y_general_name[SORTING_LESION_LEVEL];
double point_y_general_visual[SORTING_LESION_LEVEL];
double point_y_specific_name[SORTING_LESION_LEVEL];
double point_y_specific_visual[SORTING_LESION_LEVEL];

double wp_correct_name[SORTING_LESION_LEVEL], wp_correct_visual[SORTING_LESION_LEVEL];
// double patient_general_name[4] = {1.0, };
// double patient_specific_name[4] =

int wp_name_data_points;
int wp_visual_data_points;
double lesioning_percent_rogers[8];
int rogers_model_data_points;
GtkType gtk_sorting_graph_get_type(void)
{
    static GtkType gtk_sorting_graph_type = 0;
    if (!gtk_sorting_graph_type)
    {
        static const GtkTypeInfo gtk_sorting_graph_info = {"GtkSorting_Graph", sizeof(GtkSorting_Graph), sizeof(GtkSorting_GraphClass), (GtkClassInitFunc) gtk_sorting_graph_class_init, (GtkObjectInitFunc) gtk_sorting_graph_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_sorting_graph_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_sorting_graph_info);
    }
    return gtk_sorting_graph_type;
}



static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    double coordx = 0, coordy = 0;
    GtkSorting_Graph *sorting_graph = GTK_SORTING_GRAPH(widget);

    gtk_sorting_graph_redraw(widget);



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

    for (i = 0; i < sorting_lesion_level; i++) {

        //         printf("%d x = %f, y = %f x1 = %f, x2 = %f y1 = %f, y2 = %f value = %f\n", i, coordx, coordy, point_x[i] - radius*1.5, point_x[i] + radius*1.5, point_y_specific_name[i][0] - radius*1.5, point_y_specific_name[i][0] + radius*1.5, value_omission[i][0]);



        /*
         *        	  printf("y1 = %f, y2 = %f\n", y_axis, point_y[i][0] + y_axis);
         *                  printf("y3 = %f, y4 = %f\n", y_axis, point_y[i][2] + y_axis);*/



        if ((coordx >= (point_x[i] - radius*1.5)) && (coordx <= (point_x[i] + radius*1.5))) {

            if ((coordy >= (point_y_general_name[i] - radius*1.5)) && (coordy <= (point_y_general_name[i] + radius*1.5))) {

                if (sorting_graph->data_type == PATIENTS) {
                    if (i >= wp_name_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }

                    g_snprintf (buffer, 64, "%1.2f, %1.1f", sorting_graph->general_name[i], wp_correct_name[i]);
                }
                else if (sorting_graph->data_type == ROGERS) {
                    if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }

                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->general_name[i], lesioning_percent_rogers[i]);
                }
                else {
                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->general_name[i], i * 100.0 / (double) sorting_lesion_level);

                }
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_general_name[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }


            if ((coordy >= (point_y_general_visual[i] - radius*1.5)) && (coordy <= (point_y_general_visual[i] + radius*1.5))) {
                if (sorting_graph->data_type == PATIENTS) {

                    if (i >= wp_visual_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }

                    g_snprintf (buffer, 64, "%1.2f, %1.1f", sorting_graph->general_visual[i], wp_correct_visual[i]);
                }
                else if (sorting_graph->data_type == ROGERS) {
                    if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }
                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->general_visual[i], lesioning_percent_rogers[i]);
                }
                else {
                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->general_visual[i], i * 100.0 / (double) sorting_lesion_level);

                }
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_general_visual[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }



            if ((coordy >= (point_y_specific_name[i] - radius*1.5)) && (coordy <= (point_y_specific_name[i] + radius*1.5))) {


                if (sorting_graph->data_type == PATIENTS) {
                    if (i >= wp_name_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }
                    g_snprintf (buffer, 64, "%1.2f, %1.1f", sorting_graph->specific_name[i], wp_correct_name[i]);
                }
                else if (sorting_graph->data_type == ROGERS) {
                    if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }
                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->specific_name[i], lesioning_percent_rogers[i]);
                }
                else {
                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->specific_name[i], i * 100.0 / (double) sorting_lesion_level);

                }
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_specific_name[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }

            if ((coordy >= (point_y_specific_visual[i] - radius*1.5)) && (coordy <= (point_y_specific_visual[i] + radius*1.5))) {


                if (sorting_graph->data_type == PATIENTS) {
                    if (i >= wp_visual_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }
                    g_snprintf (buffer, 64, "%1.2f, %1.1f", sorting_graph->specific_visual[i], wp_correct_visual[i]);
                }
                else if (sorting_graph->data_type == ROGERS) {
                    if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                        break;
                    }
                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->specific_visual[i], lesioning_percent_rogers[i]);
                }
                else {
                    g_snprintf (buffer, 64, "%1.2f, %1.1f%%", sorting_graph->specific_visual[i], i * 100.0 / (double) sorting_lesion_level);

                }
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_specific_visual[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }


        }


        /*
         *        	    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.5);
         *        	    cairo_move_to(cr, point_x[i], point_y_general_name[i][0]);
         *                    cairo_arc(cr, point_x[i], point_y_general_name[i][0], radius, 0.0, 2*M_PI);
         *        	    cairo_move_to(cr, point_x[i], point_y_general_name[i][1]);
         *                    cairo_arc(cr, point_x[i], point_y_general_name[i][1], radius, 0.0, 2*M_PI);
         *
         *                    cairo_arc(cr, point_x[i], point_y_specific_name[i][0], radius, 0.0, 2*M_PI);
         *                    cairo_arc(cr, point_x[i], point_y_specific_name[i][1], radius, 0.0, 2*M_PI);
         *                    cairo_arc(cr, point_x[i], point_y_crossdomain[i][0], radius, 0.0, 2*M_PI);
         *                    cairo_arc(cr, point_x[i], point_y_crossdomain[i][1], radius, 0.0, 2*M_PI);
         *                    cairo_arc(cr, point_x[i], point_y_general_visual[i][0], radius, 0.0, 2*M_PI);
         *                    cairo_arc(cr, point_x[i], point_y_general_visual[i][1], radius, 0.0, 2*M_PI);
         *
         *        	    cairo_stroke(cr);
         */

    }



    g_object_unref (layout);
    cairo_destroy (cr);

    return TRUE;
}

GtkWidget * gtk_sorting_graph_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_sorting_graph_get_type()));
}



static void gtk_sorting_graph_class_init(GtkSorting_GraphClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_sorting_graph_realize;
    widget_class->size_request = gtk_sorting_graph_size_request;
    widget_class->size_allocate = gtk_sorting_graph_size_allocate;
    widget_class->expose_event = gtk_sorting_graph_expose;
    object_class->destroy = gtk_sorting_graph_destroy;
}
static char y_label[4][100];
double x_offset;
double y_offset;
int n_pattern[2];
static void gtk_sorting_graph_init(GtkSorting_Graph *sorting_graph)
{

    int l;
    for (l = 0; l < SORTING_LESION_LEVEL; l++) {
        sorting_graph->general_name[l] = 0.0;
        sorting_graph->specific_name[l] = 0.0;
        sorting_graph->general_visual[l] = 0.0;
        sorting_graph->specific_visual[l] = 0.0;

        sorting_graph->se_general_name[l] = 0.0;
        sorting_graph->se_specific_name[l] = 0.0;
        sorting_graph->se_general_visual[l] = 0.0;
        sorting_graph->se_specific_visual[l] = 0.0;

    }
    // 	sprintf(y_label[0], "Omission Errors");
    // 	sprintf(y_label[1], "Semantic Errors");
    // 	sprintf(y_label[2], "Superordinate Errors");
    // 	sprintf(y_label[3], "Crossdomain Errors");

    // 	sorting_graph->type = 0;

    if (EXCLUDE_FRUIT) {

        n_pattern[0] = get_domain_n_pattern(0) - get_n_fruit();// animals
        n_pattern[1] = get_domain_n_pattern(1);
    }
    else
    {

        n_pattern[0] = get_domain_n_pattern(0);
        n_pattern[1] = get_domain_n_pattern(1);
    }
    sorting_graph->legend = TRUE;

    gtk_widget_add_events(GTK_WIDGET(sorting_graph), GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(sorting_graph), "button-press-event", G_CALLBACK(clicked), NULL);

}

void gtk_sorting_graph_set_type(GtkSorting_Graph *sorting_graph, int type)
{
    sorting_graph->type = type;

}

void gtk_sorting_graph_set_legend(GtkSorting_Graph *sorting_graph, int legend)
{
    sorting_graph->legend = legend;

}

void gtk_sorting_graph_set_data_type(GtkSorting_Graph *sorting_graph, int type)
{
    sorting_graph->data_type = type;

    gtk_sorting_graph_update(sorting_graph);

    gtk_sorting_graph_redraw(GTK_WIDGET(sorting_graph));

}


static void gtk_sorting_graph_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_SORTING_GRAPH(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

    //     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_sorting_graph_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_SORTING_GRAPH(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

    //     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_sorting_graph_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_SORTING_GRAPH(widget));

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

static gboolean gtk_sorting_graph_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_SORTING_GRAPH(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_sorting_graph_paint(widget);

    return FALSE;
}


extern double activation_threshold;

void gtk_sorting_graph_update(GtkSorting_Graph *sorting_graph)
{
    //   sorting_lesion_level = 4;
    gtk_sorting_graph_init(sorting_graph);

    double sample_mean_general_name[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL], sample_mean_specific_name[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL], sample_mean_general_visual[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL], sample_mean_specific_visual[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL];


    // if (sorting_graph->type == 0) {
    int s, p, l, d; // p is target, q is network's response
    for (l = 0; l < sorting_lesion_level; l++) {
        sorting_graph->general_name[l] = 0.0;
        sorting_graph->specific_name[l] = 0.0;
        sorting_graph->general_visual[l] = 0.0;
        sorting_graph->specific_visual[l] = 0.0;

        sorting_graph->se_general_name[l] = 0.0;
        sorting_graph->se_specific_name[l] = 0.0;
        sorting_graph->se_general_visual[l] = 0.0;
        sorting_graph->se_specific_visual[l] = 0.0;


        point_x[l] = 0.0;
        point_y_general_name[l] = 0.0;
        point_y_general_visual[l] = 0.0;
        point_y_specific_name[l] = 0.0;
        point_y_specific_visual[l] = 0.0;
        for (d = 0; d < SORTING_LESION_DISTRIBUTION; d++) {
            sample_mean_general_name[d][l] = 0.0;
            sample_mean_specific_name[d][l] = 0.0;
            sample_mean_general_visual[d][l] = 0.0;
            sample_mean_specific_visual[d][l] = 0.0;
        }
    }


    if (sorting_graph->data_type == MODEL) {


        for (d = 0; d < sorting_lesion_distribution; d++) {
            for (l = 0; l < sorting_lesion_level; l++) {
                for (p = 0; p < P; p++) {
                    if (!excluded_pattern(p)) {
                        for (s = 0; s < sorting_sample; s++) {
                            if (get_domain(p) == sorting_response_given_name[d][l][s][p][0]) {
                                (sorting_graph->general_name[l])++;
                                sample_mean_general_name[d][l]++;
                            }
                            if (get_category(p) == sorting_response_given_name[d][l][s][p][1]) {
                                (sorting_graph->specific_name[l])++;
                                sample_mean_specific_name[d][l]++;

                            }

                            if (get_domain(p) == sorting_response_given_visual[d][l][s][p][0]) {
                                (sorting_graph->general_visual[l])++;
                                sample_mean_general_visual[d][l]++;

                            }
                            if (get_category(p) == sorting_response_given_visual[d][l][s][p][1]) {
                                (sorting_graph->specific_visual[l])++;
                                sample_mean_specific_visual[d][l]++;

                            }

                        }

                    }
                }
            }

        }

        for (l = 0; l < sorting_lesion_level; l++) {
            //         printf("sorting_graph->general_name[%d] = %f\n", l, sorting_graph->general_name[l]);
            sorting_graph->general_name[l] /= (double) (sorting_lesion_distribution * sorting_sample * (n_pattern[0] + n_pattern[1]));
            sorting_graph->specific_name[l] /= (double) (sorting_lesion_distribution * sorting_sample * (n_pattern[0] + n_pattern[1]));
            sorting_graph->general_visual[l] /= (double) (sorting_lesion_distribution * sorting_sample * (n_pattern[0] + n_pattern[1]));
            sorting_graph->specific_visual[l] /= (double) (sorting_lesion_distribution * sorting_sample * (n_pattern[0] + n_pattern[1]));
            for (d = 0; d < sorting_lesion_distribution; d++) {
                sample_mean_general_name[d][l] /= (double) (sorting_sample * (n_pattern[0] + n_pattern[1]));
                sample_mean_specific_name[d][l] /= (double) (sorting_sample * (n_pattern[0] + n_pattern[1]));
                sample_mean_general_visual[d][l] /= (double) (sorting_sample * (n_pattern[0] + n_pattern[1]));
                sample_mean_specific_visual[d][l] /= (double) (sorting_sample * (n_pattern[0] + n_pattern[1]));
            }
        }

        for (d = 0; d < sorting_lesion_distribution; d++) {
            for (l = 0; l < sorting_lesion_level; l++) {
                sorting_graph->se_general_name[l] += sqr(sample_mean_general_name[d][l] - sorting_graph->general_name[l]);
                sorting_graph->se_specific_name[l] += sqr(sample_mean_specific_name[d][l] - sorting_graph->specific_name[l]);
                sorting_graph->se_general_visual[l] += sqr(sample_mean_general_visual[d][l] - sorting_graph->general_visual[l]);
                sorting_graph->se_specific_visual[l] += sqr(sample_mean_specific_visual[d][l] - sorting_graph->specific_visual[l]);
            }

        }

        for (l = 0; l < sorting_lesion_level; l++) {
            sorting_graph->se_general_name[l] /= (double) sorting_lesion_distribution;
            sorting_graph->se_specific_name[l] /= (double) sorting_lesion_distribution;
            sorting_graph->se_general_visual[l] /= (double) sorting_lesion_distribution;
            sorting_graph->se_specific_visual[l] /= (double) sorting_lesion_distribution;

            sorting_graph->se_general_name[l] = sqrt(sorting_graph->se_general_name[l]) / (double) sqrt(sorting_lesion_distribution);
            sorting_graph->se_specific_name[l] = sqrt(sorting_graph->se_specific_name[l]) / (double) sqrt(sorting_lesion_distribution);
            sorting_graph->se_general_visual[l] = sqrt(sorting_graph->se_general_visual[l]) / (double) sqrt(sorting_lesion_distribution);
            sorting_graph->se_specific_visual[l] = sqrt(sorting_graph->se_specific_visual[l]) / (double) sqrt(sorting_lesion_distribution);

            //      printf("General Name: %f +/- %f\n", sorting_graph->general_name[l], sorting_graph->se_general_name[l]);
            //      printf("General Visual: %f +/- %f\n", sorting_graph->general_visual[l], sorting_graph->se_general_visual[l]);
            //      printf("Specific Name: %f +/- %f\n", sorting_graph->specific_name[l], sorting_graph->se_specific_name[l]);
            //      printf("Specific Visual: %f +/- %f\n", sorting_graph->specific_visual[l], sorting_graph->se_specific_visual[l]);

        }

    } else if (sorting_graph->data_type == PATIENTS) {



        sorting_lesion_level = SORTING_LESION_LEVEL;
        wp_name_data_points = 6;
        wp_visual_data_points = 10;


        sorting_graph->general_visual[9] =     0.9773;
        sorting_graph->general_visual[8] =    0.9876;
        sorting_graph->general_visual[7] =     0.9918;
        sorting_graph->general_visual[6] =     0.9918;
        sorting_graph->general_visual[5] =     0.9959;
        sorting_graph->general_visual[4] =        0.9856;
        sorting_graph->general_visual[3] =     1.0000;
        sorting_graph->general_visual[2] =    0.9918;
        sorting_graph->general_visual[1] =         1.0000;
        sorting_graph->general_visual[0] =       1.0000;

        sorting_graph->specific_visual[9] =   0.5732 ;
        sorting_graph->specific_visual[8] =  0.7835 ;
        sorting_graph->specific_visual[7] = 0.8804 ;
        sorting_graph->specific_visual[6] =    0.8701;
        sorting_graph->specific_visual[5] =      0.9175 ;
        sorting_graph->specific_visual[4] =   0.9278 ;
        sorting_graph->specific_visual[3] =    0.9134;
        sorting_graph->specific_visual[2] =    0.9505 ;
        sorting_graph->specific_visual[1] =       0.9526  ;
        sorting_graph->specific_visual[0] =           0.9918;


        wp_correct_visual[0] =        48.0000      ;
        wp_correct_visual[1] =       47.1681 ;
        wp_correct_visual[2] =       46.1397;
        wp_correct_visual[3] =         44.0658;
        wp_correct_visual[4] =         43.1486;
        wp_correct_visual[5] =         41.4994 ;
        wp_correct_visual[6] =         37.3655;
        wp_correct_visual[7] =         29.0489 ;
        wp_correct_visual[8] =            24.7868  ;
        wp_correct_visual[9] =          14.4047;




        sorting_graph->general_name[0] =  1.0021;
        sorting_graph->general_name[1] =  0.9979;
        sorting_graph->general_name[2] = 0.9503;
        sorting_graph->general_name[3] = 1.0041;
        sorting_graph->general_name[4] = 0.9110;
        sorting_graph->general_name[5] = 0.7414;



        sorting_graph->specific_name[0] = 0.9524;
        sorting_graph->specific_name[1] =0.8924;
        sorting_graph->specific_name[2] = 0.9193;
        sorting_graph->specific_name[3] = 0.8903;
        sorting_graph->specific_name[4] = 0.7993;
        sorting_graph->specific_name[5] = 0.6338;


        wp_correct_name[0] = 46.2471;
        wp_correct_name[1] = 43.5873;
        wp_correct_name[2] = 41.9285;
        wp_correct_name[3] = 38.5364;
        wp_correct_name[4] = 26.8151;
        wp_correct_name[5] = 19.8617;








    } else if (sorting_graph->data_type == ROGERS) {

        sorting_lesion_level = SORTING_LESION_LEVEL;
        rogers_model_data_points = 8;









        sorting_graph->general_visual[0] =  0.9957;
        sorting_graph->general_visual[1] =  0.9951;
        sorting_graph->general_visual[2] =  0.9904;
        sorting_graph->general_visual[3] =  0.9816;
        sorting_graph->general_visual[4] =  0.9708;
        sorting_graph->general_visual[5] =  0.9394;
        sorting_graph->general_visual[6] =  0.9429;
        sorting_graph->general_visual[7] =  0.9402;










        sorting_graph->specific_visual[0] = 0.9978;
        sorting_graph->specific_visual[1] = 0.8965;
        sorting_graph->specific_visual[2] = 0.8671;
        sorting_graph->specific_visual[3] = 0.8419;
        sorting_graph->specific_visual[4] = 0.8125;
        sorting_graph->specific_visual[5] = 0.7338;
        sorting_graph->specific_visual[6] = 0.6880;
        sorting_graph->specific_visual[7] = 0.6197;










        sorting_graph->general_name[0] = 1.0038;
        sorting_graph->general_name[1] = 0.9985;
        sorting_graph->general_name[2] =  0.9702;
        sorting_graph->general_name[3] =  0.9357;
        sorting_graph->general_name[4] = 0.9470;
        sorting_graph->general_name[5] = 0.9104;
        sorting_graph->general_name[6] = 0.7988;
        sorting_graph->general_name[7] = 0.7497;













        sorting_graph->specific_name[0] = 1.0017;
        sorting_graph->specific_name[1] =  0.9026;
        sorting_graph->specific_name[2] = 0.8452;
        sorting_graph->specific_name[3] = 0.7794;
        sorting_graph->specific_name[4] = 0.7219;
        sorting_graph->specific_name[5] =  0.6479;
        sorting_graph->specific_name[6] = 0.5800;
        sorting_graph->specific_name[7] =0.4642 ;


        lesioning_percent_rogers[0] = 0.0;
        lesioning_percent_rogers[1] = 5.0;
        lesioning_percent_rogers[2] = 10.0;
        lesioning_percent_rogers[3] = 15.0;
        lesioning_percent_rogers[4] = 20.0;
        lesioning_percent_rogers[5] = 25.0;
        lesioning_percent_rogers[6] = 30.0;
        lesioning_percent_rogers[7] = 35.0;

    }



}



static void draw_sorting_subgraph(cairo_t *cr, GtkSorting_Graph *sorting_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p, int subgraph)
{

    x_width = width - (x_offset * 2);
    y_height = height - (y_offset * 2);

    if (sorting_graph->data_type == PATIENTS) {
        x_segments = 8.0;

    }
    else if (sorting_graph->data_type == ROGERS) {
        x_segments = rogers_model_data_points;

    }
    else {
        x_segments = (double) sorting_lesion_level;
    }
    y_segments = (y_height > 100) ? 10.0 : 5.0;
    x_space = x_width / x_segments;
    y_space = y_height / y_segments;
    x_axis = width - x_offset;
    y_axis = height - y_offset;




    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);

    int i;

    // Transparent background
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width(cr, 2.0);


    /*
     *    	  for (i = 0; i < ((int)y_segments + 1); i++) {
     *    	    cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
     *    	    cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
     *    	    g_snprintf(buffer, 64, "%1.1f", 1 - i/ (double) y_segments);
     *    	    cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
     *    	    cairox_paint_pango_text(cr, &p, layout, buffer);
     }*/
    // * (100.0 / (double) SORTING_LESION_LEVEL),

    cairo_set_source_rgb(cr, WORDS_COLOUR);
    for (i = 0; i < sorting_lesion_level; i++) {
        if (sorting_graph->data_type == PATIENTS) {

            if (i >= wp_name_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            // 	  x_offset + i*x_space
            cairo_line_to(cr, x_offset + (10.0 - wp_correct_name[i]/5.0) * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis);

        }
        else if (sorting_graph->data_type == ROGERS) {

            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            // 	  x_offset + i*x_space
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis);

        }
        else {
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis);
        }
        // 	  printf("x=%i y=%f\n", i, sorting_graph->general_name[i]);
    }
    cairo_stroke(cr);

    for (i = 0; i < sorting_lesion_level; i++) {
        if (sorting_graph->data_type == PATIENTS) {
            break;
            // 	cairo_move_to(cr, x_offset + (wp_correct[i]-15.0)/5.0  * x_space, -y_space * 10 * (sorting_graph->general_name[i] - sorting_graph->se_general_name[i]) + y_axis);
            // 	cairo_line_to(cr, x_offset + (wp_correct[i]-15.0)/5.0  * x_space, -y_space * 10 * (sorting_graph->general_name[i] + sorting_graph->se_general_name[i]) + y_axis);
            //
            // 	cairo_move_to(cr, x_offset + (wp_correct[i]-15.0)/5.0  * x_space + 4, -y_space * 10 * (sorting_graph->general_name[i] - sorting_graph->se_general_name[i]) + y_axis);
            // 	cairo_line_to(cr, x_offset + (wp_correct[i]-15.0)/5.0  * x_space - 4, -y_space * 10 * (sorting_graph->general_name[i] - sorting_graph->se_general_name[i]) + y_axis);
            //
            // 	cairo_move_to(cr, x_offset + (wp_correct[i]-15.0)/5.0  * x_space + 4, -y_space * 10 * (sorting_graph->general_name[i] + sorting_graph->se_general_name[i]) + y_axis);
            // 	cairo_line_to(cr, x_offset + (wp_correct[i]-15.0)/5.0  * x_space - 4, -y_space * 10 * (sorting_graph->general_name[i] + sorting_graph->se_general_name[i]) + y_axis);
        }
        else if (sorting_graph->data_type == ROGERS) {
            break;

        }
        else {
            cairo_move_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->general_name[i] - sorting_graph->se_general_name[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->general_name[i] + sorting_graph->se_general_name[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->general_name[i] - sorting_graph->se_general_name[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->general_name[i] - sorting_graph->se_general_name[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->general_name[i] + sorting_graph->se_general_name[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->general_name[i] + sorting_graph->se_general_name[i]) + y_axis);
        }
        // 	  printf("x=%i y=%f\n", i, sorting_graph->general_name[i]);
    }
    cairo_stroke(cr);


    // //     for (i = 0; i < sorting_lesion_level; i++) {
    // // // //     	cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * patient_general_name[i] + y_axis);
    // 	  printf("x=%i y=%f\n", i, sorting_graph->general_name[i]);
    //     }

    //     cairo_stroke(cr);
    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {
            if (i >= wp_name_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_arc(cr, x_offset + (10.0 - wp_correct_name[i]/5.0) * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis, radius, 0.0, 2*M_PI);
            point_x[i] = x_offset + (10.0 - wp_correct_name[i]/5.0) * x_space;

        }
        else if (sorting_graph->data_type == ROGERS) {
            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis, radius, 0.0, 2*M_PI);
            point_x[i] = x_offset + i * x_space;

        }
        else {
            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis, radius, 0.0, 2*M_PI);
            point_x[i] = x_offset + i * x_space;

        }
        point_y_general_name[i] = -y_space * 10 * sorting_graph->general_name[i] + y_axis;

        cairo_fill(cr);

    }
    // 	cairo_set_source_rgb(cr, WORDS_COLOUR);
    cairo_set_dash (cr, superordinate_dash, 1, 0);
    for (i = 0; i < sorting_lesion_level; i++) {
        if (sorting_graph->data_type == PATIENTS) {


            if (i >= wp_name_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_line_to(cr, x_offset + (10.0 - wp_correct_name[i]/5.0) * x_space, -y_space * 10 * sorting_graph->specific_name[i]  + y_axis);


        }

        else if (sorting_graph->data_type == ROGERS) {


            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->specific_name[i]  + y_axis);


        }
        else {
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->specific_name[i]  + y_axis);
        }
        // 	    printf("x=%i y=%f\n", i, sorting_graph->specific_name[i]);
    }
    cairo_stroke(cr);
    cairo_set_dash (cr, superordinate_dash, 0, 0);

    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {


            break;
            // 	cairo_move_to(cr, x_offset +(wp_correct[i]-15.0)/5.0 * x_space, -y_space * 10 * (sorting_graph->specific_name[i] - sorting_graph->se_specific_name[i]) + y_axis);
            // 	cairo_line_to(cr, x_offset + (wp_correct[i]-15.0)/5.0 * x_space, -y_space * 10 * (sorting_graph->specific_name[i] + sorting_graph->se_specific_name[i]) + y_axis);
            //
            // 	cairo_move_to(cr, x_offset + (wp_correct[i]-15.0)/5.0 * x_space - 4, -y_space * 10 * (sorting_graph->specific_name[i] + sorting_graph->se_specific_name[i]) + y_axis);
            // 	cairo_line_to(cr, x_offset + (wp_correct[i]-15.0)/5.0 * x_space + 4, -y_space * 10 * (sorting_graph->specific_name[i] + sorting_graph->se_specific_name[i]) + y_axis);
            //
            // 	cairo_move_to(cr, x_offset + (wp_correct[i]-15.0)/5.0 * x_space + 4, -y_space * 10 * (sorting_graph->specific_name[i] - sorting_graph->se_specific_name[i]) + y_axis);
            // 	cairo_line_to(cr, x_offset + (wp_correct[i]-15.0)/5.0 * x_space - 4, -y_space * 10 * (sorting_graph->specific_name[i] - sorting_graph->se_specific_name[i]) + y_axis);
        }

        else if (sorting_graph->data_type == ROGERS) {


            break;
        }
        else {
            cairo_move_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->specific_name[i] - sorting_graph->se_specific_name[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->specific_name[i] + sorting_graph->se_specific_name[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->specific_name[i] + sorting_graph->se_specific_name[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->specific_name[i] + sorting_graph->se_specific_name[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->specific_name[i] - sorting_graph->se_specific_name[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->specific_name[i] - sorting_graph->se_specific_name[i]) + y_axis);

        }
        // 	  printf("x=%i y=%f\n", i, sorting_graph->specific_name[i]);
    }
    cairo_stroke(cr);

    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {

            if (i >= wp_name_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_arc(cr, x_offset + (10.0 - wp_correct_name[i]/5.0) * x_space, -y_space * 10 * sorting_graph->specific_name[i]  + y_axis, radius, 0.0, 2*M_PI);

        }

        else if (sorting_graph->data_type == ROGERS) {

            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->specific_name[i]  + y_axis, radius, 0.0, 2*M_PI);

        }
        else {
            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->specific_name[i]  + y_axis, radius, 0.0, 2*M_PI);
        }
        cairo_fill(cr);

        point_y_specific_name[i] = -y_space * 10 * sorting_graph->specific_name[i] + y_axis;


    }

    cairo_set_source_rgb(cr, PICTURES_COLOUR);
    // 	  cairo_set_dash (cr, crossdomain_dash, 2, 0);
    cairo_set_dash (cr, superordinate_dash, 0, 0);

    for (i = 0; i < sorting_lesion_level; i++) {
        if (sorting_graph->data_type == PATIENTS) {
            if (i >= wp_visual_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_line_to(cr, x_offset + (10.0 - wp_correct_visual[i]/5.0) * x_space, -y_space * 10 * sorting_graph->general_visual[i] + y_axis);
            // 	printf("wp = %f\n", wp_correct[i]);
        }

        else if (sorting_graph->data_type == ROGERS) {
            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_visual[i] + y_axis);
            // 	printf("wp = %f\n", wp_correct[i]);
        }
        else {
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_visual[i]  + y_axis);
        }
        // 	    printf("x=%i y=%f\n", i, sorting_graph->general_visual[i]);
    }
    cairo_stroke(cr);


    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {
            break;
        }
        else if (sorting_graph->data_type == ROGERS) {
            break;
        }
        else {
            cairo_move_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->general_visual[i] - sorting_graph->se_general_visual[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->general_visual[i] + sorting_graph->se_general_visual[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->general_visual[i] - sorting_graph->se_general_visual[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->general_visual[i] - sorting_graph->se_general_visual[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->general_visual[i] + sorting_graph->se_general_visual[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->general_visual[i] + sorting_graph->se_general_visual[i]) + y_axis);
        }
        // 	  printf("x=%i y=%f\n", i, sorting_graph->general_visual[i]);
    }
    cairo_stroke(cr);


    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {
            if (i >= wp_visual_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }

            cairo_arc(cr, x_offset + (10.0 - wp_correct_visual[i]/5.0) * x_space, -y_space * 10 * sorting_graph->general_visual[i]  + y_axis, radius, 0.0, 2*M_PI);

        }

        else if (sorting_graph->data_type == ROGERS) {
            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }

            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_visual[i]  + y_axis, radius, 0.0, 2*M_PI);

        }
        else {


            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->general_visual[i]  + y_axis, radius, 0.0, 2*M_PI);
        }
        point_y_general_visual[i] = -y_space * 10 * sorting_graph->general_visual[i]  + y_axis;

        cairo_fill(cr);

    }

    // 	  cairo_set_source_rgb(cr, PICTURES_COLOUR);
    // 	  cairo_set_dash (cr, omission_dash, 4, 0);
    cairo_set_dash (cr, superordinate_dash, 1, 0);

    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {
            if (i >= wp_visual_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_line_to(cr, x_offset + (10.0 - wp_correct_visual[i]/5.0) * x_space, -y_space * 10 * sorting_graph->specific_visual[i] + y_axis);
        }
        else if (sorting_graph->data_type == ROGERS) {
            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }
            cairo_line_to(cr, x_offset +i * x_space, -y_space * 10 * sorting_graph->specific_visual[i] + y_axis);
        }
        else {
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->specific_visual[i] + y_axis);
        }
        // 	    printf("x=%i y=%f\n", i, sorting_graph->specific_visual[i]);
    }

    cairo_stroke(cr);
    cairo_set_dash (cr, superordinate_dash, 0, 0);


    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {
            break;
        }

        else if (sorting_graph->data_type == ROGERS) {
            break;
        }
        else {
            cairo_move_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->specific_visual[i] - sorting_graph->se_specific_visual[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * (sorting_graph->specific_visual[i] + sorting_graph->se_specific_visual[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->specific_visual[i] - sorting_graph->se_specific_visual[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->specific_visual[i] - sorting_graph->se_specific_visual[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space - 4, -y_space * 10 * (sorting_graph->specific_visual[i] + sorting_graph->se_specific_visual[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space + 4, -y_space * 10 * (sorting_graph->specific_visual[i] + sorting_graph->se_specific_visual[i]) + y_axis);
        }
        // 	  printf("x=%i y=%f\n", i, sorting_graph->specific_visual[i]);
    }
    cairo_stroke(cr);


    for (i = 0; i < sorting_lesion_level; i++) {

        if (sorting_graph->data_type == PATIENTS) {
            if (i >= wp_visual_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }

            cairo_arc(cr, x_offset + (10.0 - wp_correct_visual[i]/5.0) * x_space, -y_space * 10 * sorting_graph->specific_visual[i] + y_axis, radius, 0.0, 2*M_PI);

        }
        else if (sorting_graph->data_type == ROGERS) {
            if (i >= rogers_model_data_points) {      //disparity between patient data points for the two subgraphs
                break;
            }

            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->specific_visual[i] + y_axis, radius, 0.0, 2*M_PI);

        }
        else {


            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * sorting_graph->specific_visual[i] + y_axis, radius, 0.0, 2*M_PI);


        }
        point_y_specific_visual[i] = -y_space * 10 * sorting_graph->specific_visual[i] + y_axis;
        cairo_fill(cr);
    }
    // 	}
    // 	else
    // 	{
    // // 	  0 = omissions, 1 = semantic, 2 = superordinate, 3 = crossdomain
    //
    // 	  if (subgraph == 0) { //omissions
    // 	    cairo_set_source_rgb(cr, 0.5, 0.5, 0);
    // 	    cairo_set_dash (cr, superordinate_dash, 0, 0);
    // 	    for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 	      cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->omission[i][0]  + y_axis);
    // 	    }
    //
    // 	    cairo_stroke(cr);
    // 	    cairo_set_dash (cr, superordinate_dash, 1, 0);
    // 	    for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 	      cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->omission[i][1]  + y_axis);
    // 	    }
    //
    // 	    cairo_stroke(cr);
    // 	  }
    // 	  else if (subgraph == 1) { //semantic
    // 	    	  cairo_set_source_rgb(cr, WORDS_COLOUR);
    // 		  cairo_set_dash (cr, superordinate_dash, 0, 0);
    // 		  for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 		    cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->semantic[i][0] + y_axis);
    // 		  }
    // 		  cairo_stroke(cr);
    //
    // 		  cairo_set_source_rgb(cr, WORDS_COLOUR);
    // 		  cairo_set_dash (cr, superordinate_dash, 1, 0);
    // 		  for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 		    cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->semantic[i][1] + y_axis);
    // 		  }
    // 		  cairo_stroke(cr);
    // 	  }
    // 	  else if (subgraph == 2) {
    // 	    cairo_set_source_rgb(cr, 0.5, 0, 0.5);
    // 	    cairo_set_dash (cr, superordinate_dash, 0, 0);
    // 	    for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 	      cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->superordinate[i][0] + y_axis);
    // 	    }
    // 	    cairo_stroke(cr);
    //
    // 	    cairo_set_source_rgb(cr, 0.5, 0, 0.5);
    // 	    cairo_set_dash (cr, superordinate_dash, 1, 0);
    // 	    for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 	      cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->superordinate[i][1] + y_axis);
    // 	    }
    // 	    cairo_stroke(cr);
    // 	  }
    // 	  else if (subgraph == 3) {
    // 	    cairo_set_source_rgb(cr, PICTURES_COLOUR);
    // 	    cairo_set_dash (cr, superordinate_dash, 0, 0);
    // 	    for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 	      cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->crossdomain[i][0] + y_axis);
    // 	    }
    // 	    cairo_stroke(cr);
    //
    // 	    cairo_set_source_rgb(cr, PICTURES_COLOUR);
    // 	    cairo_set_dash (cr, superordinate_dash, 1, 0);
    // 	    for (i = 0; i < SORTING_LESION_LEVEL; i++) {
    // 	      cairo_line_to(cr, x_axis - sorting_graph->correct[i] * x_space * 10, -y_space * 10 * sorting_graph->crossdomain[i][1] + y_axis);
    // 	    }
    // 	    cairo_stroke(cr);
    // 	  }
    //
    // 	}
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


    for (i = 0; i < ((int)y_segments + 1); i++) {
        cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
        cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
        g_snprintf(buffer, 64, "%1.1f", 1 - i/ (double) y_segments);
        cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
    }
    //
    cairo_stroke(cr);
    //

    if (sorting_graph->data_type == PATIENTS) {
        //from 50 to 15
        for (i = 0; i < ((int)x_segments + 1); i++) {
            // 	  printf("i = %i\n", SORTING_LESION_LEVEL);
            cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
            cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
            g_snprintf(buffer, 64, "%i", (int)(50.0 - 5.0 * i));
            cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }

        //       cairo_line_to(cr, x_offset + (10.0 - wp_correct_name[i]/5.0) * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis);



        //       printf("x_offset = %f\n", x_offset);

    } else if (sorting_graph->data_type == ROGERS) {
        //from 50 to 15
        for (i = 0; i < ((int)x_segments + 1); i++) {
            // 	  printf("i = %i\n", SORTING_LESION_LEVEL);
            cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
            cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
            g_snprintf(buffer, 64, "%i", (int)(i*5.0));
            cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }

        //       cairo_line_to(cr, x_offset + (10.0 - wp_correct_name[i]/5.0) * x_space, -y_space * 10 * sorting_graph->general_name[i] + y_axis);



        //       printf("x_offset = %f\n", x_offset);

    }
    else {
        for (i = 0; i < ((int)x_segments + 1); i++) {
            // 	  printf("i = %i\n", SORTING_LESION_LEVEL);
            cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
            cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
            g_snprintf(buffer, 64, "%i", (int) (i * 100.0 / (double) sorting_lesion_level));
            cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }
    }

}

static void draw_sorting_legend(cairo_t *cr, GtkSorting_Graph *sorting_graph, PangoLayout *layout, CairoxTextParameters p)
{
    // 	//
    pangox_layout_set_font_size(layout, 13);
    pangox_layout_select_font_from_string(layout, "CMU Serif");
    if (sorting_graph->legend) {

        double text_width = 0.0;

//     if ((sorting_graph->data_type == MODEL)  || (sorting_graph->data_type == ROGERS) ) {

        //draw legend
//         if (sorting_graph->type == 0) {
//     if (sorting_graph->data_type == PATIENTS) {


        //
//             if (sorting_graph->data_type == ROGERS) {
//
        cairo_translate(cr, 0, 3*x_offset);
//
//             }
//             else if (sorting_graph->data_type == MODEL) {
//                 cairo_translate(cr, 2*x_offset, 0);
//
//             }
        // 	cairo_translate(cr, 8*x_offset, 0);
        g_snprintf(buffer, 64, "%s", "General Words:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 2*x_offset, 2*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "%s", "Specific Words:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 2*x_offset, 2.5*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "%s", "General Pictures:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 2*x_offset, 3*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "%s", "Specific Pictures:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 2*x_offset, 3.5*x_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        cairo_set_source_rgb(cr, WORDS_COLOUR);
        cairo_move_to(cr, 2.5*x_offset + text_width, 2*x_offset+1);
        cairo_line_to(cr, 6*x_offset-5, 2*x_offset+1);
        cairo_stroke(cr);
        cairo_set_dash (cr, superordinate_dash, 1, 0);
        cairo_move_to(cr, 2.5*x_offset + text_width, 2.5*x_offset+1);
        cairo_line_to(cr, 6*x_offset-5, 2.5*x_offset+1);
        cairo_stroke(cr);
        cairo_set_source_rgb(cr, PICTURES_COLOUR);
        cairo_set_dash (cr, superordinate_dash, 0, 0);
        cairo_move_to(cr, 2.5*x_offset + text_width, 3*x_offset+1);
        cairo_line_to(cr, 6*x_offset-5, 3*x_offset+1);
        cairo_stroke(cr);
        cairo_set_dash (cr, superordinate_dash, 1, 0);
        cairo_move_to(cr, 2.5*x_offset + text_width, 3.5*x_offset+1);
        cairo_line_to(cr, 6*x_offset-5, 3.5*x_offset+1);
        cairo_stroke(cr);

        // 	cairo_translate(cr, -(8*x_offset), 0);
//             if (sorting_graph->data_type == ROGERS) {
//
        cairo_translate(cr, 0, -3*x_offset);
//
//             }
//             else if (sorting_graph->data_type == MODEL) {
//                 cairo_translate(cr, -2*x_offset, 0);
//
//             }
        //
        //     else if (sorting_graph->type == 1) {
        //         g_snprintf(buffer, 64, "%s", "Animals:");
        //         cairox_text_parameters_set(&p, 2*x_offset, 2*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        //         cairox_paint_pango_text(cr, &p, layout, buffer);
        //         cairo_set_source_rgb(cr, 0, 0, 0);
        // 	cairo_move_to(cr, 4.5*x_offset + 5, 2*y_offset+1);
        //         cairo_line_to(cr, 6*x_offset-5, 2*y_offset+1);
        //         cairo_stroke(cr);
        //
        //         g_snprintf(buffer, 64, "%s", "Artifacts:");
        //         cairox_text_parameters_set(&p, 2*x_offset, 2.5*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        //         cairox_paint_pango_text(cr, &p, layout, buffer);
        //         cairo_set_source_rgb(cr, 0, 0, 0);
        //         cairo_set_dash (cr, superordinate_dash, 1, 0);
        // 	cairo_move_to(cr, 4.5*x_offset + 5, 2.5*y_offset+1);
        //         cairo_line_to(cr, 6*x_offset-5, 2.5*y_offset+1);
        //         cairo_stroke(cr);
        //     }
//         }
        cairo_set_dash (cr, omission_dash, 0, 0);
//     }

//     }
    }
}

static void draw_sorting_axes_labels(cairo_t *cr, GtkSorting_Graph *sorting_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p)
{

    if (sorting_graph->type == 0)
    {
        g_snprintf(buffer, 64, "Proportion Correct");
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
            cairox_text_parameters_set(&p, x_offset - 33.0, (quarter_height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_translate(cr, 0, quarter_height);
        }
        cairo_stroke(cr);

        cairo_translate(cr, 0, -height- y_offset/2.0);
        cairo_stroke(cr);


    }

    //     g_snprintf(buffer, 64, "Percent Connections Lesioned");
    //     cairox_text_parameters_set(&p, (width - x_offset)/2.0, (height - y_offset) + 57.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
    //     cairox_paint_pango_text(cr, &p, layout, buffer);
    //     cairo_stroke(cr);


    if (sorting_graph->data_type == PATIENTS) {
        g_snprintf(buffer, 64, "Word to Picture Matching (out of 48)");
        cairox_text_parameters_set(&p, width /2.0, (height - y_offset) + 37.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_stroke(cr);
    }
    else {

        g_snprintf(buffer, 64, "Percent Connections Lesioned");
        cairox_text_parameters_set(&p, width/2.0, (height - y_offset) + 37.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);


        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_stroke(cr);
    }


    //
}

static void draw_sorting_graph(cairo_t *cr, GtkWidget *widget, double width, double height)
{


    //     double width = (double) widget->allocation.width, height = (double) widget->allocation.height;
    GtkSorting_Graph *sorting_graph =  GTK_SORTING_GRAPH(widget);
    PangoLayout *layout = pango_cairo_create_layout(cr);
    CairoxTextParameters p;


    x_offset = (sorting_graph->type) ? 51.0 : 51.0;
    y_offset = (sorting_graph->type) ? 50.0 : 50.0;


    draw_sorting_legend(cr, GTK_SORTING_GRAPH(widget), layout, p);

    draw_sorting_axes_labels(cr, sorting_graph, width, height, layout, p);


    if (sorting_graph->type == 0) {
        draw_sorting_subgraph(cr, sorting_graph, width, height, layout, p, -1);
    }
    else
    {
        int i;
        for (i = 0; i < 4; i++) {

            draw_sorting_subgraph(cr, sorting_graph, width, height / 4.0, layout, p, i);
            cairo_translate(cr, 0, height / 4.0);
        }

    }
    g_object_unref(layout);
    cairo_destroy(cr);

}
void gtk_sorting_graph_pdf(GtkWidget *widget, char *filename)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = 500;//widget->allocation.width;
    int height = 500;//widget->allocation.height;
    char name[100];
    int i = 0;
    GtkSorting_Graph *sorting_graph = GTK_SORTING_GRAPH(widget);

    if (sorting_graph->data_type == MODEL) {
        while (filename[i] != '\0') {
            name[i] = filename[i];
            i++;
        }
        name[i] = '\0';
        printf("%s\n", name);
        sprintf(name, "%s_sorting.pdf", name);
    }
    else if (sorting_graph->data_type == ROGERS) {
        sprintf(name, "rogers_model_sorting.pdf");

    }
    else if (sorting_graph->data_type == PATIENTS) {
        sprintf(name, "rogers_patients_sorting.pdf");

    }


    surface = cairo_pdf_surface_create(name, width, height);
    cr = cairo_create(surface);
    // draw sorting_graph
    draw_sorting_graph(cr, widget, width, height);


    cairo_surface_destroy(surface);

}


static void gtk_sorting_graph_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    draw_sorting_graph(cr, widget, (double) widget->allocation.width, (double) widget->allocation.height);
}

void gtk_sorting_graph_reset(GtkSorting_Graph *sorting_graph)
{
    gtk_sorting_graph_init(sorting_graph);
    gtk_sorting_graph_redraw(GTK_WIDGET(sorting_graph));
}

static void gtk_sorting_graph_destroy(GtkObject *object)
{
    //     GtkSorting_Graph *sorting_graph;
    GtkSorting_GraphClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_SORTING_GRAPH(object));

    //     sorting_graph = GTK_SORTING_GRAPH(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_sorting_graph_redraw(GtkWidget *widget)
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

void gtk_sorting_graph_clear(GtkWidget *widget)
{
    init_sorting_response(0, 0, 0);
    gtk_sorting_graph_reset(GTK_SORTING_GRAPH(widget));

}
