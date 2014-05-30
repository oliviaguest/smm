#include "tyler_graph.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_tyler_graph_class_init(GtkTyler_GraphClass *klass);
GtkType gtk_tyler_graph_get_type(void);
static void gtk_tyler_graph_init(GtkTyler_Graph *tyler_graph);
static void gtk_tyler_graph_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_tyler_graph_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_tyler_graph_realize(GtkWidget *widget);
static gboolean gtk_tyler_graph_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_tyler_graph_paint(GtkWidget *widget);
static void gtk_tyler_graph_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

static char buffer[64];
double dash[1] = {5.0};
int error_bars = 1;

#define   tyler_original_lesion_level 9

double distinctive_features_error_tyler[N_DOMAIN][tyler_original_lesion_level] = {{0.01, 0.0462, 0.0861, 0.1304, 0.1573, 0.1954, 0.2104, 0.2416, 0.2766}, {0.01, 0.0325, 0.0699, 0.0862, 0.1243, 0.1729, 0.2191, 0.2615, 0.2927}};


double shared_features_error_tyler[N_DOMAIN][tyler_original_lesion_level] = {{0.0078, 0.0298, 0.0519, 0.0810, 0.1201, 0.1805, 0.2373, 0.2956, 0.3425}, {0.0127, 0.0501, 0.0953, 0.1299, 0.1962, 0.2491, 0.3147, 0.3535, 0.3775}};

double functional_features_error_tyler[N_DOMAIN][tyler_original_lesion_level] = {{0.0095, 0.0285, 0.0675, 0.0845, 0.1214, 0.1714, 0.2166, 0.2569, 0.2890}, {0.0088, 0.0223, 0.0434, 0.0797, 0.1345, 0.2086, 0.2675, 0.3272, 0.3655}};

double percent_correct_tyler[N_DOMAIN][tyler_original_lesion_level] = {{99.7661, 96.6972, 93.4349, 76.0600, 63.3250, 52.1364, 46.1673, 30.9189, 21.2771}, {99.7658, 99.9836, 98.6546, 97.9050, 85.9428, 67.7953, 38.4343, 26.8590, 14.7039}};


double percent_incorrect_tyler[2][N_DOMAIN][tyler_original_lesion_level] = {{{0.1955, 3.1603, 7.9003, 22.9419, 34.6984, 43.5236, 44.2682, 43.4125, 42.1150}, {0.2848, 0.1400, 1.1506, 2.8717, 9.5659, 20.6995, 32.1897, 28.9376, 25.0630}}, {{0, 0, 0, 0, 0, 0.0965, 0.3083, 2.5613, 13.0746}, {0, 0, 0, 0, 6.0136, 10.2208, 24.0195, 29.0268, 28.3488}}};


static double radius = 3.0;
double x_width;
double y_height;
double x_segments;
double y_segments;
double x_space;
double y_space;
double x_axis;
double y_axis;
double y_increment;

double point_x[TYLER_LESION_LEVEL][N_DOMAIN];
double point_x_incorrect[TYLER_LESION_LEVEL][N_DOMAIN][2];
double point_y_shared_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
double point_y_distinctive_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
double point_y_functional_features_error[TYLER_LESION_LEVEL][N_DOMAIN];
double point_y_features_living_error[TYLER_LESION_LEVEL][N_DOMAIN];
double point_y_percent_correct[TYLER_LESION_LEVEL][N_DOMAIN];
double point_y_percent_incorrect[TYLER_LESION_LEVEL][N_DOMAIN][2];


double sample_mean_shared_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_distinctive_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_functional_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_percent_correct[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN],  sample_mean_percent_incorrect[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN][2];


double network_mean_shared_features_error[TYLER_NETWORK][TYLER_LESION_LEVEL][N_DOMAIN], network_mean_distinctive_features_error[TYLER_NETWORK][TYLER_LESION_LEVEL][N_DOMAIN], network_mean_functional_features_error[TYLER_NETWORK][TYLER_LESION_LEVEL][N_DOMAIN], network_mean_percent_correct[TYLER_NETWORK][TYLER_LESION_LEVEL][N_DOMAIN],  network_mean_percent_incorrect[TYLER_NETWORK][TYLER_LESION_LEVEL][N_DOMAIN][2];



// double patient_distinctive_features_error[4] = {1.0, };
// double patient_shared_features_error[4] =

#define n_patterns (n_pattern[0] + n_pattern[1])
#define PURPLE		0.5, 0.0, 0.5
#define GREEN		0.5, 0.5, 0.0

#define RED		0.6, 0.25, 0.35
#define BLUE		0.25, 0.35, 0.6

GtkType gtk_tyler_graph_get_type(void)
{
    static GtkType gtk_tyler_graph_type = 0;
    if (!gtk_tyler_graph_type)
    {
        static const GtkTypeInfo gtk_tyler_graph_info = {"GtkTyler_Graph", sizeof(GtkTyler_Graph), sizeof(GtkTyler_GraphClass), (GtkClassInitFunc) gtk_tyler_graph_class_init, (GtkObjectInitFunc) gtk_tyler_graph_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_tyler_graph_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_tyler_graph_info);
    }
    return gtk_tyler_graph_type;
}



static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    double coordx = 0, coordy = 0;
    GtkTyler_Graph *tyler_graph = GTK_TYLER_GRAPH(widget);

    gtk_tyler_graph_redraw(widget);



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
    int i, domain, d;
    int break_again = 0;

//     printf("x = %f, y = %f\n", coordx, coordy);


    cr = gdk_cairo_create (widget->window);

    /* Transparent background */
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint (cr);
    layout = pango_cairo_create_layout (cr);



    for (i = 0; i < tyler_lesion_level; i++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {

            if ((coordx >= (point_x[i][domain] - radius*1.5)) && (coordx <= (point_x[i][domain] + radius*1.5))) {



                if(tyler_graph->type == DISTINCTIVE_FEATURES_ERROR) {
                    if (break_again) {
                        break;
                    }

                    if ((coordy >= (point_y_distinctive_features_error[i][domain] - radius*1.5)) && (coordy <= (point_y_distinctive_features_error[i][domain] + radius*1.5))) {

                        if(tyler_graph->data_type == MODEL) {
                            g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->distinctive_features_error[i][domain], i * 100.0 / (double) tyler_lesion_level);

                        } else if(tyler_graph->data_type == TYLER) {
                            if (i < tyler_original_lesion_level) {
                                g_snprintf (buffer, 64, "%1.2f, %1.1f%%", distinctive_features_error_tyler[domain][i], i * 100.0 / (double) tyler_lesion_level);
                            } else {
                                break;
                            }
                        }

                        pangox_layout_set_font_size(layout, 10.0);
                        cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_distinctive_features_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                        cairox_paint_pango_text (cr, &text_p, layout, buffer);
                        break_again = 1;
                        break;

                    }

                }

                else if(tyler_graph->type == SHARED_FEATURES_ERROR) {
                    if (break_again) {
                        break;
                    }

                    if ((coordy >= (point_y_shared_features_error[i][domain] - radius*1.5)) && (coordy <= (point_y_shared_features_error[i][domain] + radius*1.5))) {


                        g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->shared_features_error[i][domain], i * 100.0 / (double) tyler_lesion_level);

                        pangox_layout_set_font_size(layout, 10.0);
                        cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_shared_features_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                        cairox_paint_pango_text (cr, &text_p, layout, buffer);
                        break_again = 1;
                        break;

                    }

                }

                else if(tyler_graph->type == FEATURES_LIVING_ERROR) {
                    if (break_again) {
                        break;
                    }

                    if ((coordy >= (point_y_features_living_error[i][domain] - radius*1.5)) && (coordy <= (point_y_features_living_error[i][domain] + radius*1.5))) {

                        if (domain == 0) {
                            g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->shared_features_error[i][0], i * 100.0 / (double) tyler_lesion_level);
                        } else {
                            g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->distinctive_features_error[i][0], i * 100.0 / (double) tyler_lesion_level);

                        }

                        pangox_layout_set_font_size(layout, 10.0);
                        cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_features_living_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                        cairox_paint_pango_text (cr, &text_p, layout, buffer);
                        break_again = 1;
                        break;

                    }

                }

                else if(tyler_graph->type == FUNCTIONAL_FEATURES_ERROR) {
                    if (break_again) {
                        break;
                    }

                    if ((coordy >= (point_y_functional_features_error[i][domain] - radius*1.5)) && (coordy <= (point_y_functional_features_error[i][domain] + radius*1.5))) {


                        g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->functional_features_error[i][domain], i * 100.0 / (double) tyler_lesion_level);

                        pangox_layout_set_font_size(layout, 10.0);
                        cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_functional_features_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                        cairox_paint_pango_text (cr, &text_p, layout, buffer);
                        break_again = 1;
                        break;

                    }

                }

                else if(tyler_graph->type == PERCENT_PATTERNS_CORRECT) {
                    if (break_again) {
                        break;
                    }

                    if ((coordy >= (point_y_percent_correct[i][domain] - radius*1.5)) && (coordy <= (point_y_percent_correct[i][domain] + radius*1.5))) {


                        g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->percent_correct[i][domain], i * 100.0 / (double) tyler_lesion_level);

                        pangox_layout_set_font_size(layout, 10.0);
                        cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_percent_correct[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                        cairox_paint_pango_text (cr, &text_p, layout, buffer);
                        break_again = 1;
                        break;

                    }

                }

                else if(tyler_graph->type == PERCENT_PATTERNS_INCORRECT) {

                    for (d = 0; d < 2; d++) {
                        if (break_again) {
                            break;
                        }

                        if ((coordy >= (point_y_percent_incorrect[i][domain][d] - radius*1.5)) && (coordy <= (point_y_percent_incorrect[i][domain][d] + radius*1.5))) {


                            g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->percent_incorrect[i][domain][d], i * 100.0 / (double) tyler_lesion_level);

                            pangox_layout_set_font_size(layout, 10.0);
                            cairox_text_parameters_set (&text_p, point_x_incorrect[i][domain][d] + radius*1.5, point_y_percent_incorrect[i][domain][d] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                            cairox_paint_pango_text (cr, &text_p, layout, buffer);
                            break_again = 1;
                            break;

                        }
                    }

                }
            }

//             if ((coordy >= (point_y_functional_features_error[i] - radius*1.5)) && (coordy <= (point_y_functional_features_error[i] + radius*1.5))) {
//
//                     g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->functional_features_error[i], i * 100.0 / (double) tyler_lesion_level);
//
//                 pangox_layout_set_font_size(layout, 10.0);
//                 cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_functional_features_error[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
//                 cairox_paint_pango_text (cr, &text_p, layout, buffer);
//                 break;
//
//             }
//
//
//
//             if ((coordy >= (point_y_shared_features_error[i] - radius*1.5)) && (coordy <= (point_y_shared_features_error[i] + radius*1.5))) {
//
//                     g_snprintf (buffer, 64, "%1.2f, %1.1f%%", tyler_graph->shared_features_error[i], i * 100.0 / (double) tyler_lesion_level);
//
//                 pangox_layout_set_font_size(layout, 10.0);
//                 cairox_text_parameters_set (&text_p, point_x[i] + radius*1.5, point_y_shared_features_error[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
//                 cairox_paint_pango_text (cr, &text_p, layout, buffer);
//                 break;
//
//             }




        }


    }



    g_object_unref (layout);
    cairo_destroy (cr);

    return TRUE;
}

GtkWidget * gtk_tyler_graph_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_tyler_graph_get_type()));
}



static void gtk_tyler_graph_class_init(GtkTyler_GraphClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_tyler_graph_realize;
    widget_class->size_request = gtk_tyler_graph_size_request;
    widget_class->size_allocate = gtk_tyler_graph_size_allocate;
    widget_class->expose_event = gtk_tyler_graph_expose;
    object_class->destroy = gtk_tyler_graph_destroy;
}


static char y_label[6][100];
double x_offset;
double y_offset;
int n_pattern[2];
static void gtk_tyler_graph_init(GtkTyler_Graph *tyler_graph)
{

    int l, domain, d, n;

    for (l = 0; l < TYLER_LESION_LEVEL; l++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {
            tyler_graph->distinctive_features_error[l][domain] = 0.0;
            tyler_graph->shared_features_error[l][domain] = 0.0;
            tyler_graph->functional_features_error[l][domain] = 0.0;

            tyler_graph->se_distinctive_features_error[l][domain] = 0.0;
            tyler_graph->se_shared_features_error[l][domain] = 0.0;
            tyler_graph->se_functional_features_error[l][domain] = 0.0;
            tyler_graph->se_percent_correct[l][domain] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][0] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][1] = 0.0;
        }
    }
    sprintf(y_label[0], "Error for Distinctive Features");
    sprintf(y_label[1], "Error for Features");
    sprintf(y_label[2], "Error for Shared Features");
    sprintf(y_label[3], "Error for Functional Features");
    sprintf(y_label[4], "Percent Patterns Correct");
    sprintf(y_label[5], "Percent Patterns Incorrect");

    tyler_graph->type = DISTINCTIVE_FEATURES_ERROR;
    tyler_graph->networks = 0;

    if (EXCLUDE_FRUIT) {

        n_pattern[0] = get_domain_n_pattern(0) - get_n_fruit();// animals
        n_pattern[1] = get_domain_n_pattern(1);
    }
    else
    {

        n_pattern[0] = get_domain_n_pattern(0);
        n_pattern[1] = get_domain_n_pattern(1);

    }




    for (l = 0; l < TYLER_LESION_LEVEL; l++) {



        for (domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                tyler_graph->percent_incorrect[l][domain][d] = 0.0; // between/within per domain
                point_x_incorrect[l][domain][d] = 0.0;
                point_y_percent_incorrect[l][domain][d] = 0.0;
            }
            point_y_distinctive_features_error[l][domain] = 0.0;
            point_y_shared_features_error[l][domain] = 0.0;
            point_y_features_living_error[l][domain] = 0.0;
            point_y_functional_features_error[l][domain] = 0.0;


            tyler_graph->shared_features_error[l][domain] = 0.0;
            tyler_graph->distinctive_features_error[l][domain] = 0.0;
            tyler_graph->functional_features_error[l][domain] = 0.0;


            tyler_graph->se_shared_features_error[l][domain] = 0.0;
            tyler_graph->se_distinctive_features_error[l][domain] = 0.0;
            tyler_graph->se_functional_features_error[l][domain] = 0.0;
            tyler_graph->se_percent_correct[l][domain] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][0] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][1] = 0.0;
            point_x[l][domain] = 0.0;

            tyler_graph->percent_correct[l][domain] = 0.0;


        }
        for (d = 0; d < TYLER_LESION_DISTRIBUTION; d++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_distinctive_features_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                sample_mean_shared_features_error[d][l][domain] = 0.0;
                sample_mean_distinctive_features_error[d][l][domain] = 0.0;
                sample_mean_functional_features_error[d][l][domain] = 0.0;
                sample_mean_percent_correct[d][l][domain] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][0] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][1] = 0.0;
            }
        }
        for (n = 0; n < TYLER_NETWORK; n++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_distinctive_features_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                network_mean_shared_features_error[n][l][domain] = 0.0;
                network_mean_distinctive_features_error[n][l][domain] = 0.0;
                network_mean_functional_features_error[n][l][domain] = 0.0;
                network_mean_percent_correct[n][l][domain] = 0.0;
                network_mean_percent_incorrect[n][l][domain][0] = 0.0;
                network_mean_percent_incorrect[n][l][domain][1] = 0.0;
            }
        }
    }


    gtk_widget_add_events(GTK_WIDGET(tyler_graph), GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(tyler_graph), "button-press-event", G_CALLBACK(clicked), NULL);

}

void gtk_tyler_graph_networks(GtkTyler_Graph *tyler_graph, int value)
{
    // whether or not the graph uses more than one file, to average over multiple networks
    tyler_graph->networks = value;
    printf("Number of networks:%d\n",tyler_graph->networks);

}

void gtk_tyler_graph_set_type(GtkTyler_Graph *tyler_graph, int type)
{
    tyler_graph->type = type;

}

void gtk_tyler_graph_set_data_type(GtkTyler_Graph *tyler_graph, int type)
{
    tyler_graph->data_type = type;
//     printf("Type is Tyler (2000):%d\n",tyler_graph->data_type == TYLER);

    gtk_tyler_graph_update(tyler_graph);

    gtk_tyler_graph_redraw(GTK_WIDGET(tyler_graph));

}


static void gtk_tyler_graph_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_TYLER_GRAPH(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

//     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_tyler_graph_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_TYLER_GRAPH(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

//     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_tyler_graph_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_TYLER_GRAPH(widget));

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

static gboolean gtk_tyler_graph_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_TYLER_GRAPH(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_tyler_graph_paint(widget);

    return FALSE;
}


extern double activation_threshold;
void gtk_tyler_set_auto_name(GtkTyler_Graph *tyler_graph, const char auto_name[]) {
    tyler_graph->auto_name = &auto_name[0];
}



void gtk_tyler_graph_update(GtkTyler_Graph *tyler_graph)
{


    //   tyler_lesion_level = 4;
    gtk_tyler_graph_init(tyler_graph);

    //  sample_mean_shared_features_error_overall[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL], sample_mean_distinctive_features_error_overall[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL], sample_mean_functional_features_error_overall[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL],

//     double sample_mean_shared_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_distinctive_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_functional_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_percent_correct[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN],  sample_mean_percent_incorrect[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN][2];


    // if (tyler_graph->type == 0) {
    int s, p, l, d, domain; // p is target, q is network's response
    for (l = 0; l < TYLER_LESION_LEVEL; l++) {


        //         tyler_graph->se_shared_features_error_overall[l] = 0.0;
        //         tyler_graph->se_distinctive_features_error_overall[l] = 0.0;
        //         tyler_graph->se_functional_features_error_overall[l] = 0.0;



        for (domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                tyler_graph->percent_incorrect[l][domain][d] = 0.0; // between/within per domain
                point_x_incorrect[l][domain][d] = 0.0;
                point_y_percent_incorrect[l][domain][d] = 0.0;
            }
            point_y_distinctive_features_error[l][domain] = 0.0;
            point_y_shared_features_error[l][domain] = 0.0;
            point_y_features_living_error[l][domain] = 0.0;
            point_y_functional_features_error[l][domain] = 0.0;


            tyler_graph->shared_features_error[l][domain] = 0.0;
            tyler_graph->distinctive_features_error[l][domain] = 0.0;
            tyler_graph->functional_features_error[l][domain] = 0.0;


            tyler_graph->se_shared_features_error[l][domain] = 0.0;
            tyler_graph->se_distinctive_features_error[l][domain] = 0.0;
            tyler_graph->se_functional_features_error[l][domain] = 0.0;
            tyler_graph->se_percent_correct[l][domain] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][0] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][1] = 0.0;
            point_x[l][domain] = 0.0;

            tyler_graph->percent_correct[l][domain] = 0.0;


        }
        for (d = 0; d < TYLER_LESION_DISTRIBUTION; d++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_distinctive_features_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                sample_mean_shared_features_error[d][l][domain] = 0.0;
                sample_mean_distinctive_features_error[d][l][domain] = 0.0;
                sample_mean_functional_features_error[d][l][domain] = 0.0;
                sample_mean_percent_correct[d][l][domain] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][0] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][1] = 0.0;
            }
        }

    }




    for (d = 0; d < tyler_lesion_distribution; d++) {
        for (l = 0; l < tyler_lesion_level; l++) {
            for (p = 0; p < P; p++) {
//                 if (!excluded_pattern(p)) {
                for (s = 0; s < tyler_sample; s++) {

                    tyler_graph->distinctive_features_error[l][get_domain(p)] += tyler_distinctive_error[d][l][s][p];
                    sample_mean_distinctive_features_error[d][l][get_domain(p)] += tyler_distinctive_error[d][l][s][p];

                    tyler_graph->shared_features_error[l][get_domain(p)] += tyler_shared_error[d][l][s][p];
                    sample_mean_shared_features_error[d][l][get_domain(p)] += tyler_shared_error[d][l][s][p];

                    tyler_graph->functional_features_error[l][get_domain(p)] += tyler_functional_error[d][l][s][p];
                    sample_mean_functional_features_error[d][l][get_domain(p)] += tyler_functional_error[d][l][s][p];


                    if (tyler_response[d][l][s][p] == p) {
                        (tyler_graph->percent_correct[l][get_domain(p)])++;
                        (sample_mean_percent_correct[d][l][get_domain(p)])++;
                        //                             printf("percent_correct[%i][%i] = %f\n", l,get_domain(p), tyler_graph->percent_correct[l][get_domain(p)]);
                    }
                    else if (get_domain(tyler_response[d][l][s][p]) == get_domain(p)) { //responce and target pattern are in the same domain
                        (tyler_graph->percent_incorrect[l][get_domain(p)][0])++;
                        (sample_mean_percent_incorrect[d][l][get_domain(p)][0])++;
                    }
                    else {
                        (tyler_graph->percent_incorrect[l][get_domain(p)][1])++;
                        (sample_mean_percent_incorrect[d][l][get_domain(p)][1])++;
                    }
                }

//                 }
            }
        }
    }


    for (l = 0; l < tyler_lesion_level; l++) {
        //         printf("tyler_graph->distinctive_features_error[%d] = %f\n", l, tyler_graph->distinctive_features_error[l]);
        //         tyler_graph->distinctive_features_error_overall[l] /= (double) (tyler_lesion_distribution * tyler_sample * n_patterns);
        // 	tyler_graph->shared_features_error_overall[l] /= (double) (tyler_lesion_distribution * tyler_sample * n_patterns);
        // 	tyler_graph->functional_features_error_overall[l] /= (double) (tyler_lesion_distribution * tyler_sample * n_patterns);
        for (domain = 0; domain < N_DOMAIN; domain++) {

            tyler_graph->distinctive_features_error[l][domain] /= (double) (tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->shared_features_error[l][domain] /= (double) (tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->functional_features_error[l][domain] /= (double) (tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_correct[l][domain] /= (double) (tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_correct[l][domain] *= 100.0;
            tyler_graph->percent_incorrect[l][domain][0] /= (double) (tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_incorrect[l][domain][1] /= (double) (tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_incorrect[l][domain][0] *= 100.0;
            tyler_graph->percent_incorrect[l][domain][1] *= 100.0;


        }

        //this is going to have to be [d][l][domain]
        for (d = 0; d < tyler_lesion_distribution; d++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {
                sample_mean_distinctive_features_error[d][l][domain] /= (double) (tyler_sample * n_pattern[domain]);
                sample_mean_shared_features_error[d][l][domain] /= (double) (tyler_sample * n_pattern[domain]);
                sample_mean_functional_features_error[d][l][domain] /= (double) (tyler_sample * n_pattern[domain]);

                sample_mean_percent_correct[d][l][domain] /=  (double) (tyler_sample * n_pattern[domain]);
                sample_mean_percent_correct[d][l][domain] *=  100.0;
                sample_mean_percent_incorrect[d][l][domain][0] /=  (double) (tyler_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][0] *= 100.0;
                sample_mean_percent_incorrect[d][l][domain][1] /=  (double) (tyler_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][1] *= 100.0;


            }


            //             sample_mean_distinctive_features_error_overall[d][l] /= (double) (tyler_sample * n_patterns);
            //             sample_mean_shared_features_error_overall[d][l] /= (double) (tyler_sample * n_patterns);
            //             sample_mean_functional_features_error_overall[d][l] /= (double) (tyler_sample * n_patterns);
        }
    }

    for (d = 0; d < tyler_lesion_distribution; d++) {
        for (l = 0; l < tyler_lesion_level; l++) {
            //             tyler_graph->se_distinctive_features_error_overall[l] += sqr(sample_mean_distinctive_features_error_overall[d][l] - tyler_graph->distinctive_features_error_overall[l]);
            //             tyler_graph->se_shared_features_error_overall[l] += sqr(sample_mean_shared_features_error_overall[d][l] - tyler_graph->shared_features_error_overall[l]);
            //             tyler_graph->se_functional_features_error_overall[l] += sqr(sample_mean_functional_features_error_overall[d][l] - tyler_graph->functional_features_error_overall[l]);

            for (domain = 0; domain < N_DOMAIN; domain++) {
                tyler_graph->se_distinctive_features_error[l][domain] += sqr(sample_mean_distinctive_features_error[d][l][domain] - tyler_graph->distinctive_features_error[l][domain]);
                tyler_graph->se_shared_features_error[l][domain] += sqr(sample_mean_shared_features_error[d][l][domain] - tyler_graph->shared_features_error[l][domain]);
                tyler_graph->se_functional_features_error[l][domain] += sqr(sample_mean_functional_features_error[d][l][domain] - tyler_graph->functional_features_error[l][domain]);
                tyler_graph->se_percent_correct[l][domain] += sqr(sample_mean_percent_correct[d][l][domain] - tyler_graph->percent_correct[l][domain]);
                tyler_graph->se_percent_incorrect[l][domain][0] += sqr(sample_mean_percent_incorrect[d][l][domain][0] - tyler_graph->percent_incorrect[l][domain][0]);
                tyler_graph->se_percent_incorrect[l][domain][1] += sqr(sample_mean_percent_incorrect[d][l][domain][1] - tyler_graph->percent_incorrect[l][domain][1]);
            }
        }

    }

    for (l = 0; l < tyler_lesion_level; l++) {
        //         tyler_graph->se_distinctive_features_error_overall[l] /= (double) tyler_lesion_distribution;
        //         tyler_graph->se_shared_features_error_overall[l] /= (double) tyler_lesion_distribution;
        //         tyler_graph->se_functional_features_error_overall[l] /= (double) tyler_lesion_distribution;

        for (domain = 0; domain < N_DOMAIN; domain++) {
            tyler_graph->se_distinctive_features_error[l][domain] /= (double) tyler_lesion_distribution;
            tyler_graph->se_shared_features_error[l][domain] /= (double) tyler_lesion_distribution;
            tyler_graph->se_functional_features_error[l][domain] /= (double) tyler_lesion_distribution;
            tyler_graph->se_percent_correct[l][domain] /= (double) tyler_lesion_distribution;
            tyler_graph->se_percent_incorrect[l][domain][0] /= (double) tyler_lesion_distribution;
            tyler_graph->se_percent_incorrect[l][domain][1] /= (double) tyler_lesion_distribution;
            tyler_graph->se_distinctive_features_error[l][domain] = sqrt(tyler_graph->se_distinctive_features_error[l][domain]) / (double) sqrt(tyler_lesion_distribution);
            tyler_graph->se_shared_features_error[l][domain] = sqrt(tyler_graph->se_shared_features_error[l][domain]) / (double) sqrt(tyler_lesion_distribution);
            tyler_graph->se_functional_features_error[l][domain] = sqrt(tyler_graph->se_functional_features_error[l][domain]) / (double) sqrt(tyler_lesion_distribution);
            tyler_graph->se_percent_correct[l][domain] = sqrt(tyler_graph->se_percent_correct[l][domain]);
            tyler_graph->se_percent_incorrect[l][domain][0] = sqrt(tyler_graph->se_percent_incorrect[l][domain][0]);
            tyler_graph->se_percent_incorrect[l][domain][1] = sqrt(tyler_graph->se_percent_incorrect[l][domain][1]);
        }

        //         tyler_graph->se_distinctive_features_error_overall[l] = sqrt(tyler_graph->se_distinctive_features_error_overall[l]) / (double) sqrt(tyler_lesion_distribution);
        //         tyler_graph->se_shared_features_error_overall[l] = sqrt(tyler_graph->se_shared_features_error_overall[l]) / (double) sqrt(tyler_lesion_distribution);
        //         tyler_graph->se_functional_features_error_overall[l] = sqrt(tyler_graph->se_functional_features_error_overall[l]) / (double) sqrt(tyler_lesion_distribution);


    }





}



void gtk_tyler_graph_update_networks(GtkTyler_Graph *tyler_graph)
{
    //   tyler_lesion_level = 4;

    //     static init_called = 0;
    //     if (!init_called) {
    //       init_called = 1;
    //     }
    //  sample_mean_shared_features_error_overall[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL], sample_mean_distinctive_features_error_overall[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL], sample_mean_functional_features_error_overall[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL],
    char filename[256];


//     double sample_mean_shared_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_distinctive_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_functional_features_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN], sample_mean_percent_correct[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN],  sample_mean_percent_incorrect[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][N_DOMAIN][2];


    // if (tyler_graph->type == 0) {
    int s, p, l, d, domain, n; // p is target, q is network's response
    for (l = 0; l < TYLER_LESION_LEVEL; l++) {


        //         tyler_graph->se_shared_features_error_overall[l] = 0.0;
        //         tyler_graph->se_distinctive_features_error_overall[l] = 0.0;
        //         tyler_graph->se_functional_features_error_overall[l] = 0.0;



        for (domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                tyler_graph->percent_incorrect[l][domain][d] = 0.0; // between/within per domain
                point_x_incorrect[l][domain][d] = 0.0;
                point_y_percent_incorrect[l][domain][d] = 0.0;
            }
            point_y_distinctive_features_error[l][domain] = 0.0;
            point_y_shared_features_error[l][domain] = 0.0;
            point_y_features_living_error[l][domain] = 0.0;
            point_y_functional_features_error[l][domain] = 0.0;


            tyler_graph->shared_features_error[l][domain] = 0.0;
            tyler_graph->distinctive_features_error[l][domain] = 0.0;
            tyler_graph->functional_features_error[l][domain] = 0.0;


            tyler_graph->se_shared_features_error[l][domain] = 0.0;
            tyler_graph->se_distinctive_features_error[l][domain] = 0.0;
            tyler_graph->se_functional_features_error[l][domain] = 0.0;
            tyler_graph->se_percent_correct[l][domain] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][0] = 0.0;
            tyler_graph->se_percent_incorrect[l][domain][1] = 0.0;
            point_x[l][domain] = 0.0;

            tyler_graph->percent_correct[l][domain] = 0.0;


        }
        for (d = 0; d < TYLER_LESION_DISTRIBUTION; d++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_distinctive_features_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                sample_mean_shared_features_error[d][l][domain] = 0.0;
                sample_mean_distinctive_features_error[d][l][domain] = 0.0;
                sample_mean_functional_features_error[d][l][domain] = 0.0;
                sample_mean_percent_correct[d][l][domain] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][0] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][1] = 0.0;
            }
        }

    }



    for (n = 0; n < tyler_graph->networks; n++) {


        sprintf(filename, "%s%d.tyler_task", tyler_graph->auto_name, n);
        if (file_exists(filename)) {
//             printf("%s\n", filename);

            open_tyler_responses(filename);
            //       set_word_to_picture_combo_box_lesion_levels();
        }
        else
        {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, filename, "(set automated_training flag?)");

        }



        for (d = 0; d < tyler_lesion_distribution; d++) {
            for (l = 0; l < tyler_lesion_level; l++) {
                for (p = 0; p < P; p++) {
//                     if (!excluded_pattern(p)) {
                    for (s = 0; s < tyler_sample; s++) {
                        s= 0;

                        tyler_graph->distinctive_features_error[l][get_domain(p)] += tyler_distinctive_error[d][l][s][p];
                        sample_mean_distinctive_features_error[d][l][get_domain(p)] += tyler_distinctive_error[d][l][s][p];

                        tyler_graph->shared_features_error[l][get_domain(p)] += tyler_shared_error[d][l][s][p];
                        sample_mean_shared_features_error[d][l][get_domain(p)] += tyler_shared_error[d][l][s][p];

                        tyler_graph->functional_features_error[l][get_domain(p)] += tyler_functional_error[d][l][s][p];
                        sample_mean_functional_features_error[d][l][get_domain(p)] += tyler_functional_error[d][l][s][p];


                        if (tyler_response[d][l][s][p] == p) {
                            (tyler_graph->percent_correct[l][get_domain(p)])++;
                            (sample_mean_percent_correct[d][l][get_domain(p)])++;
                            //                             printf("percent_correct[%i][%i] = %f\n", l,get_domain(p), tyler_graph->percent_correct[l][get_domain(p)]);
                        }
                        else if (get_domain(tyler_response[d][l][s][p]) == get_domain(p)) { //responce and target pattern are in the same domain
                            (tyler_graph->percent_incorrect[l][get_domain(p)][0])++;
                            (sample_mean_percent_incorrect[d][l][get_domain(p)][0])++;
                        }
                        else {
                            (tyler_graph->percent_incorrect[l][get_domain(p)][1])++;
                            (sample_mean_percent_incorrect[d][l][get_domain(p)][1])++;
                        }
                    }

                }
            }
//             }
        }
    }


    for (l = 0; l < tyler_lesion_level; l++) {
        //         printf("tyler_graph->distinctive_features_error[%d] = %f\n", l, tyler_graph->distinctive_features_error[l]);
        //         tyler_graph->distinctive_features_error_overall[l] /= (double) (tyler_lesion_distribution * tyler_sample * n_patterns);
        // 	tyler_graph->shared_features_error_overall[l] /= (double) (tyler_lesion_distribution * tyler_sample * n_patterns);
        // 	tyler_graph->functional_features_error_overall[l] /= (double) (tyler_lesion_distribution * tyler_sample * n_patterns);
        for (domain = 0; domain < N_DOMAIN; domain++) {

            tyler_graph->distinctive_features_error[l][domain] /= (double) (tyler_graph->networks * tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->shared_features_error[l][domain] /= (double) (tyler_graph->networks* tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->functional_features_error[l][domain] /= (double) (tyler_graph->networks* tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_correct[l][domain] /= (double) (tyler_graph->networks * tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_correct[l][domain] *= 100.0;
            tyler_graph->percent_incorrect[l][domain][0] /= (double) (tyler_graph->networks * tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_incorrect[l][domain][1] /= (double) (tyler_graph->networks * tyler_lesion_distribution * tyler_sample * n_pattern[domain]);
            tyler_graph->percent_incorrect[l][domain][0] *= 100.0;
            tyler_graph->percent_incorrect[l][domain][1] *= 100.0;


        }

        //this is going to have to be [d][l][domain]
        for (d = 0; d < tyler_lesion_distribution; d++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {
                sample_mean_distinctive_features_error[d][l][domain] /= (double) (tyler_graph->networks * tyler_sample * n_pattern[domain]);
                sample_mean_shared_features_error[d][l][domain] /= (double) (tyler_graph->networks * tyler_sample * n_pattern[domain]);
                sample_mean_functional_features_error[d][l][domain] /= (double) (tyler_graph->networks* tyler_sample * n_pattern[domain]);

                sample_mean_percent_correct[d][l][domain] /=  (double) (tyler_graph->networks* tyler_sample * n_pattern[domain]);
                sample_mean_percent_correct[d][l][domain] *=  100.0;
                sample_mean_percent_incorrect[d][l][domain][0] /=  (double) (tyler_graph->networks* tyler_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][0] *= 100.0;
                sample_mean_percent_incorrect[d][l][domain][1] /=  (double) (tyler_graph->networks * tyler_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][1] *= 100.0;


            }


            //             sample_mean_distinctive_features_error_overall[d][l] /= (double) (tyler_sample * n_patterns);
            //             sample_mean_shared_features_error_overall[d][l] /= (double) (tyler_sample * n_patterns);
            //             sample_mean_functional_features_error_overall[d][l] /= (double) (tyler_sample * n_patterns);
        }
    }

    for (d = 0; d < tyler_lesion_distribution; d++) {
        for (l = 0; l < tyler_lesion_level; l++) {
            //             tyler_graph->se_distinctive_features_error_overall[l] += sqr(sample_mean_distinctive_features_error_overall[d][l] - tyler_graph->distinctive_features_error_overall[l]);
            //             tyler_graph->se_shared_features_error_overall[l] += sqr(sample_mean_shared_features_error_overall[d][l] - tyler_graph->shared_features_error_overall[l]);
            //             tyler_graph->se_functional_features_error_overall[l] += sqr(sample_mean_functional_features_error_overall[d][l] - tyler_graph->functional_features_error_overall[l]);

            for (domain = 0; domain < N_DOMAIN; domain++) {
                tyler_graph->se_distinctive_features_error[l][domain] += sqr(sample_mean_distinctive_features_error[d][l][domain] - tyler_graph->distinctive_features_error[l][domain]);
                tyler_graph->se_shared_features_error[l][domain] += sqr(sample_mean_shared_features_error[d][l][domain] - tyler_graph->shared_features_error[l][domain]);
                tyler_graph->se_functional_features_error[l][domain] += sqr(sample_mean_functional_features_error[d][l][domain] - tyler_graph->functional_features_error[l][domain]);
                tyler_graph->se_percent_correct[l][domain] += sqr(sample_mean_percent_correct[d][l][domain] - tyler_graph->percent_correct[l][domain]);
                tyler_graph->se_percent_incorrect[l][domain][0] += sqr(sample_mean_percent_incorrect[d][l][domain][0] - tyler_graph->percent_incorrect[l][domain][0]);
                tyler_graph->se_percent_incorrect[l][domain][1] += sqr(sample_mean_percent_incorrect[d][l][domain][1] - tyler_graph->percent_incorrect[l][domain][1]);
            }
        }

    }

    for (l = 0; l < tyler_lesion_level; l++) {
        //         tyler_graph->se_distinctive_features_error_overall[l] /= (double) tyler_lesion_distribution;
        //         tyler_graph->se_shared_features_error_overall[l] /= (double) tyler_lesion_distribution;
        //         tyler_graph->se_functional_features_error_overall[l] /= (double) tyler_lesion_distribution;

        for (domain = 0; domain < N_DOMAIN; domain++) {
            tyler_graph->se_distinctive_features_error[l][domain] /= (double) tyler_graph->networks * tyler_lesion_distribution;
            tyler_graph->se_shared_features_error[l][domain] /= (double)tyler_graph->networks * tyler_lesion_distribution;
            tyler_graph->se_functional_features_error[l][domain] /= (double)tyler_graph->networks* tyler_lesion_distribution;
            tyler_graph->se_percent_correct[l][domain] /= (double)tyler_graph->networks * tyler_lesion_distribution;
            tyler_graph->se_percent_incorrect[l][domain][0] /= (double)tyler_graph->networks* tyler_lesion_distribution;
            tyler_graph->se_percent_incorrect[l][domain][1] /= (double) tyler_graph->networks* tyler_lesion_distribution;
            tyler_graph->se_distinctive_features_error[l][domain] = sqrt(tyler_graph->se_distinctive_features_error[l][domain]) / (double) sqrt(tyler_graph->networks * tyler_lesion_distribution);
            tyler_graph->se_shared_features_error[l][domain] = sqrt(tyler_graph->se_shared_features_error[l][domain]) / (double) sqrt(tyler_graph->networks * tyler_lesion_distribution);
            tyler_graph->se_functional_features_error[l][domain] = sqrt(tyler_graph->se_functional_features_error[l][domain]) / (double) sqrt(tyler_graph->networks * tyler_lesion_distribution);
            tyler_graph->se_percent_correct[l][domain] = sqrt(tyler_graph->se_percent_correct[l][domain]);
            tyler_graph->se_percent_incorrect[l][domain][0] = sqrt(tyler_graph->se_percent_incorrect[l][domain][0]);
            tyler_graph->se_percent_incorrect[l][domain][1] = sqrt(tyler_graph->se_percent_incorrect[l][domain][1]);
        }

        //         tyler_graph->se_distinctive_features_error_overall[l] = sqrt(tyler_graph->se_distinctive_features_error_overall[l]) / (double) sqrt(tyler_lesion_distribution);
        //         tyler_graph->se_shared_features_error_overall[l] = sqrt(tyler_graph->se_shared_features_error_overall[l]) / (double) sqrt(tyler_lesion_distribution);
        //         tyler_graph->se_functional_features_error_overall[l] = sqrt(tyler_graph->se_functional_features_error_overall[l]) / (double) sqrt(tyler_lesion_distribution);


    }





}
void gtk_tyler_graph_update_networks_accumulate(GtkTyler_Graph *tyler_graph, int n)
{

    int s, p, l, d;


    printf("%s: network number = %d of %d\n", __FUNCTION__, n, tyler_graph->networks);



    for (d = 0; d < tyler_lesion_distribution; d++) {
        for (l = 0; l < tyler_lesion_level; l++) {
            for (p = 0; p < P; p++) {
//                 if (!excluded_pattern(p)) {
                for (s = 0; s < tyler_sample; s++) {


                    tyler_graph->distinctive_features_error[l][get_domain(p)] += tyler_distinctive_error[d][l][s][p];
//                         sample_mean_distinctive_features_error[d][l][get_domain(p)] += tyler_distinctive_error[d][l][s][p];
                    network_mean_distinctive_features_error[n][l][get_domain(p)] += tyler_distinctive_error[d][l][s][p];


                    tyler_graph->shared_features_error[l][get_domain(p)] += tyler_shared_error[d][l][s][p];
// 			sample_mean_shared_features_error[d][l][get_domain(p)] += tyler_shared_error[d][l][s][p];
                    network_mean_shared_features_error[n][l][get_domain(p)] += tyler_shared_error[d][l][s][p];

                    tyler_graph->functional_features_error[l][get_domain(p)] += tyler_functional_error[d][l][s][p];
// 			sample_mean_functional_features_error[d][l][get_domain(p)] += tyler_functional_error[d][l][s][p];
                    network_mean_functional_features_error[n][l][get_domain(p)] += tyler_functional_error[d][l][s][p];


                    if (tyler_response[d][l][s][p] == p) {
                        (tyler_graph->percent_correct[l][get_domain(p)])++;
// 			    (sample_mean_percent_correct[d][l][get_domain(p)])++;
                        (network_mean_percent_correct[n][l][get_domain(p)])++;
                    }
                    else if (get_domain(tyler_response[d][l][s][p]) == get_domain(p)) { //responce and target pattern are in the same domain
                        (tyler_graph->percent_incorrect[l][get_domain(p)][0])++;
// 			    (sample_mean_percent_incorrect[d][l][get_domain(p)][0])++;
                        (network_mean_percent_incorrect[n][l][get_domain(p)][0])++;
                    }
                    else {
                        (tyler_graph->percent_incorrect[l][get_domain(p)][1])++;
// 			    (sample_mean_percent_incorrect[d][l][get_domain(p)][1])++;
                        (network_mean_percent_incorrect[n][l][get_domain(p)][1])++;
                    }
                }

            }
//             }
        }
    }




}

void gtk_tyler_graph_update_networks_final(GtkTyler_Graph *tyler_graph)
{
    int l, n, domain, d;


    for (n = 0; n <(tyler_graph->networks); n++) {

        printf("%s: network number = %d of %d\n", __FUNCTION__, n, tyler_graph->networks);

        for (l = 0; l < tyler_lesion_level; l++) {

            for (domain = 0; domain < N_DOMAIN; domain++) {
                network_mean_distinctive_features_error[n][l][domain] /= (double) (tyler_lesion_distribution * n_pattern[domain]);
                network_mean_shared_features_error[n][l][domain] /= (double) (tyler_lesion_distribution * n_pattern[domain]);
                network_mean_functional_features_error[n][l][domain] /= (double) (tyler_lesion_distribution * n_pattern[domain]);

                network_mean_percent_correct[n][l][domain] /=  (double) (tyler_lesion_distribution * n_pattern[domain]);
                network_mean_percent_correct[n][l][domain] *=  100.0;
                network_mean_percent_incorrect[n][l][domain][0] /=  (double) (tyler_lesion_distribution * n_pattern[domain]);
                network_mean_percent_incorrect[n][l][domain][0] *= 100.0;
                network_mean_percent_incorrect[n][l][domain][1] /=  (double) (tyler_lesion_distribution * n_pattern[domain]);
                network_mean_percent_incorrect[n][l][domain][1] *= 100.0;
            }

        }
    }



    // These are the global means
    for (l = 0; l < tyler_lesion_level; l++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {

            tyler_graph->distinctive_features_error[l][domain] /= (double) (tyler_graph->networks * tyler_lesion_distribution * n_pattern[domain]);
            tyler_graph->shared_features_error[l][domain] /= (double) (tyler_graph->networks * tyler_lesion_distribution * n_pattern[domain]);
            tyler_graph->functional_features_error[l][domain] /= (double) (tyler_graph->networks * tyler_lesion_distribution * n_pattern[domain]);
            tyler_graph->percent_correct[l][domain] /= (double) (tyler_graph->networks * tyler_lesion_distribution * n_pattern[domain]);
            tyler_graph->percent_correct[l][domain] *= 100.0;

            for (d = 0; d < 2; d++) {

// 	      printf("tyler_graph->percent_incorrect[%d][%d][%d]=%f\n", l, domain, d, tyler_graph->percent_incorrect[l][domain][d]);

                tyler_graph->percent_incorrect[l][domain][d] /= (double) (tyler_graph->networks * tyler_lesion_distribution * n_pattern[domain]);
                tyler_graph->percent_incorrect[l][domain][d] *= 100.0;
// 	    printf("tyler_graph->percent_incorrect[%d][%d][%d]=%f\n", l, domain, d, tyler_graph->percent_incorrect[l][domain][d]);
// 	    printf("tyler_graph->networks = %d, tyler_lesion_distribution = %d, n_pattern[%d] = %d\n",tyler_graph->networks, tyler_lesion_distribution, domain,n_pattern[domain] );
            }


        }
    }
//         //this is going to have to be [d][l][domain]
//         for (d = 0; d < TYLER_LESION_DISTRIBUTION; d++) {
//             for (domain = 0; domain < N_DOMAIN; domain++) {
//                 sample_mean_distinctive_features_error[d][l][domain] /= (double) (tyler_graph->networks * TYLER_SAMPLE * n_pattern[domain]);
//                 sample_mean_shared_features_error[d][l][domain] /= (double) (tyler_graph->networks * TYLER_SAMPLE * n_pattern[domain]);
//                 sample_mean_functional_features_error[d][l][domain] /= (double) (tyler_graph->networks * TYLER_SAMPLE * n_pattern[domain]);
//
//                 sample_mean_percent_correct[d][l][domain] /=  (double) (tyler_graph->networks * TYLER_SAMPLE * n_pattern[domain]);
//                 sample_mean_percent_correct[d][l][domain] *=  100.0;
//                 sample_mean_percent_incorrect[d][l][domain][0] /=  (double) (tyler_graph->networks * TYLER_SAMPLE * n_pattern[domain]);
//                 sample_mean_percent_incorrect[d][l][domain][0] *= 100.0;
//                 sample_mean_percent_incorrect[d][l][domain][1] /=  (double) (tyler_graph->networks * TYLER_SAMPLE * n_pattern[domain]);
//                 sample_mean_percent_incorrect[d][l][domain][1] *= 100.0;
//
//
//             }
//
//         }

    for (l = 0; l < tyler_lesion_level; l++) {

        //sum squared difference of sample and mean
        for (n= 0; n < tyler_graph->networks; n++) {
            for (l = 0; l < tyler_lesion_level; l++) {
                for (domain = 0; domain < N_DOMAIN; domain++) {
                    tyler_graph->se_distinctive_features_error[l][domain] += sqr(network_mean_distinctive_features_error[n][l][domain] - tyler_graph->distinctive_features_error[l][domain]);
                    tyler_graph->se_shared_features_error[l][domain] += sqr(network_mean_shared_features_error[n][l][domain] - tyler_graph->shared_features_error[l][domain]);
                    tyler_graph->se_functional_features_error[l][domain] += sqr(network_mean_functional_features_error[n][l][domain] - tyler_graph->functional_features_error[l][domain]);
                    tyler_graph->se_percent_correct[l][domain] += sqr(network_mean_percent_correct[n][l][domain] - tyler_graph->percent_correct[l][domain]);
                    tyler_graph->se_percent_incorrect[l][domain][0] += sqr(network_mean_percent_incorrect[n][l][domain][0] - tyler_graph->percent_incorrect[l][domain][0]);
                    tyler_graph->se_percent_incorrect[l][domain][1] += sqr(network_mean_percent_incorrect[n][l][domain][1] - tyler_graph->percent_incorrect[l][domain][1]);
                }
            }

        }
        // rest of calculation for SE
        for (l = 0; l < tyler_lesion_level; l++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {



                tyler_graph->se_distinctive_features_error[l][domain] = sqrt(tyler_graph->se_distinctive_features_error[l][domain] / (double) (tyler_graph->networks - 1.0)) / (double) sqrt((double) tyler_graph->networks);
                tyler_graph->se_shared_features_error[l][domain] = sqrt(tyler_graph->se_shared_features_error[l][domain] / (double) (tyler_graph->networks - 1.0)) / (double) sqrt((double) tyler_graph->networks);
                tyler_graph->se_functional_features_error[l][domain] =  sqrt(tyler_graph->se_functional_features_error[l][domain]/ (double) (tyler_graph->networks - 1.0)) / (double) sqrt((double) tyler_graph->networks);
                tyler_graph->se_percent_correct[l][domain]  = sqrt(tyler_graph->se_percent_correct[l][domain] / (double) (tyler_graph->networks - 1.0)) / (double) sqrt((double) tyler_graph->networks);
                tyler_graph->se_percent_incorrect[l][domain][0] = sqrt(tyler_graph->se_percent_incorrect[l][domain][0] / (double) (tyler_graph->networks - 1.0)) / (double) sqrt((double) tyler_graph->networks);
                tyler_graph->se_percent_incorrect[l][domain][1] = sqrt( tyler_graph->se_percent_incorrect[l][domain][1]/ (double) (tyler_graph->networks - 1.0)) / (double) sqrt((double) tyler_graph->networks);

            }



        }

    }
}


static void draw_tyler_subgraph(cairo_t *cr, GtkTyler_Graph *tyler_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p, int subgraph)
{
    int i, domain, d;
    double y_max = 0.0;

    for (i = 0; i < tyler_lesion_level; i++) {

        if ((tyler_graph->type == DISTINCTIVE_FEATURES_ERROR) || (tyler_graph->type == FEATURES_LIVING_ERROR) || (tyler_graph->type == SHARED_FEATURES_ERROR) || (tyler_graph->type == SHARED_FEATURES_ERROR) || (tyler_graph->type == FUNCTIONAL_FEATURES_ERROR)) {

            for (domain = 0; domain < N_DOMAIN; domain++) {

                y_max = fmax(y_max, (tyler_graph->distinctive_features_error[i][domain] + tyler_graph->se_distinctive_features_error[i][domain]));
                y_max = fmax(y_max, (tyler_graph->distinctive_features_error[i][0] + tyler_graph->se_distinctive_features_error[i][0]));
                y_max = fmax(y_max, (tyler_graph->shared_features_error[i][0] + tyler_graph->se_distinctive_features_error[i][0]));
                y_max = fmax(y_max, (tyler_graph->shared_features_error[i][domain] + tyler_graph->se_shared_features_error[i][domain]));
                y_max = fmax(y_max, (tyler_graph->functional_features_error[i][domain] + tyler_graph->se_functional_features_error[i][domain]));
            }
            y_max = ceilf(y_max * 10.0) / 10.0;



        } else if ((tyler_graph->type == PERCENT_PATTERNS_CORRECT) ||  (tyler_graph->type == PERCENT_PATTERNS_INCORRECT)) {

            y_max = 100.0;
// 	for (domain = 0; domain < N_DOMAIN; domain++) {
//
//             y_max = fmax(y_max, (tyler_graph->percent_correct[i][domain] + tyler_graph->se_percent_correct[i][domain]));
// 	    y_max = fmax(y_max, (tyler_graph->percent_incorrect[i][domain][0] + tyler_graph->se_percent_incorrect[i][domain][0]));
// 	    y_max = fmax(y_max, (tyler_graph->percent_incorrect[i][domain][1] + tyler_graph->se_percent_incorrect[i][domain][1]));
// 	}
// 	    y_max = ceilf(y_max / 10.0) * 10.0;

        }
    }

//     printf("y_max = %f\n", y_max);
    x_width = width - (x_offset * 2);
    y_height = height - (y_offset * 2);

    x_segments = (double) tyler_lesion_level;

    y_segments = (y_height > 100) ? 10.0 : 5.0;
    x_space = x_width / x_segments;
    y_space = y_height / y_segments;
    y_increment = y_max / y_segments;
    x_axis = width - x_offset;
    y_axis = height - y_offset;




    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);


    // Transparent background
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width(cr, 2.0);


//     cairo_set_source_rgb(cr, 239/255.0, 187/255.0, 204/255.0);
    cairo_set_source_rgb(cr, 0, 0, 0);

    if (tyler_graph->type == DISTINCTIVE_FEATURES_ERROR) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * tyler_graph->distinctive_features_error[0][domain] / y_max + y_axis);
            for (i = 1; i < tyler_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * tyler_graph->distinctive_features_error[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            if (error_bars) {

                for (i = 0; i < tyler_lesion_level; i++) {


//                     printf("SE  = %f\n", tyler_graph->se_distinctive_features_error[i][domain]);

                    cairo_move_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->distinctive_features_error[i][domain] - tyler_graph->se_distinctive_features_error[i][domain]) / y_max  + y_axis);
                    cairo_line_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->distinctive_features_error[i][domain] + tyler_graph->se_distinctive_features_error[i][domain]) / y_max + y_axis);

                    cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->distinctive_features_error[i][domain] - tyler_graph->se_distinctive_features_error[i][domain]) / y_max  + y_axis);
                    cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->distinctive_features_error[i][domain] - tyler_graph->se_distinctive_features_error[i][domain]) / y_max  + y_axis);

                    cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->distinctive_features_error[i][domain] + tyler_graph->se_distinctive_features_error[i][domain]) / y_max  + y_axis);
                    cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->distinctive_features_error[i][domain] + tyler_graph->se_distinctive_features_error[i][domain]) / y_max  + y_axis);
                }
                cairo_stroke(cr);

            }

            for (i = 0; i < tyler_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * tyler_graph->distinctive_features_error[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_distinctive_features_error[i][domain] = -y_height * tyler_graph->distinctive_features_error[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }

    } else if (tyler_graph->type == FEATURES_LIVING_ERROR) {

        cairo_set_source_rgb(cr, RED);


        cairo_move_to(cr, x_offset + 0 * x_space, -y_height * tyler_graph->shared_features_error[0][0] / y_max + y_axis);
        for (i = 1; i < tyler_lesion_level; i++) {
            cairo_line_to(cr, x_offset + i * x_space, -y_height * tyler_graph->shared_features_error[i][0]/ y_max + y_axis);

        }
        cairo_stroke(cr);

        for (i = 0; i < tyler_lesion_level; i++) {

//             printf("SE  = %f\n", tyler_graph->shared_features_error[i][0]);
            cairo_move_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->shared_features_error[i][0] - tyler_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->shared_features_error[i][0] + tyler_graph->se_shared_features_error[i][0]) / y_max + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->shared_features_error[i][0] - tyler_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->shared_features_error [i][0] - tyler_graph->se_shared_features_error[i][0]) / y_max  + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->shared_features_error [i][0] + tyler_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->shared_features_error [i][0] + tyler_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
        }
        cairo_stroke(cr);

        for (i = 0; i < tyler_lesion_level; i++) {

            cairo_arc(cr, x_offset + i * x_space, -y_height * tyler_graph->shared_features_error[i][0]  / y_max + y_axis, radius, 0.0, 2*M_PI);
            point_x[i][0] = x_offset + i * x_space;

            //         }

            point_y_features_living_error[i][0] = -y_height * tyler_graph->shared_features_error[i][0] / y_max + y_axis;

            cairo_fill(cr);

        }

        cairo_set_source_rgb(cr, BLUE);

        cairo_move_to(cr, x_offset + 0 * x_space, -y_height * tyler_graph->distinctive_features_error[0][0] / y_max + y_axis);
        for (i = 1; i < tyler_lesion_level; i++) {
            cairo_line_to(cr, x_offset + i * x_space, -y_height * tyler_graph->distinctive_features_error[i][0]/ y_max + y_axis);

        }
        cairo_stroke(cr);

        for (i = 0; i < tyler_lesion_level; i++) {

//             printf("SE  = %f\n", tyler_graph->distinctive_features_error[i][0]);

            cairo_move_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->distinctive_features_error[i][0] - tyler_graph->se_distinctive_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->distinctive_features_error[i][0] + tyler_graph->se_distinctive_features_error[i][0]) / y_max + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->distinctive_features_error[i][0] - tyler_graph->se_distinctive_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->distinctive_features_error[i][0] - tyler_graph->se_distinctive_features_error[i][0]) / y_max  + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->distinctive_features_error[i][0] + tyler_graph->se_distinctive_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->distinctive_features_error[i][0] + tyler_graph->se_distinctive_features_error[i][0]) / y_max  + y_axis);
        }
        cairo_stroke(cr);

        for (i = 0; i < tyler_lesion_level; i++) {

            cairo_arc(cr, x_offset + i * x_space, -y_height * tyler_graph->distinctive_features_error[i][0]  / y_max + y_axis, radius, 0.0, 2*M_PI);
            point_x[i][1] = x_offset + i * x_space;

            //         }

            point_y_features_living_error[i][1] = -y_height * tyler_graph->distinctive_features_error[i][0] / y_max + y_axis;

            cairo_fill(cr);

        }

    } else if (tyler_graph->type == SHARED_FEATURES_ERROR) {
        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * tyler_graph->shared_features_error[0][domain] / y_max + y_axis);
            for (i = 1; i < tyler_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * tyler_graph->shared_features_error[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            for (i = 0; i < tyler_lesion_level; i++) {
//                 printf("SE  = %f\n", tyler_graph->shared_features_error[i][domain]);


                cairo_move_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->shared_features_error[i][domain] - tyler_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->shared_features_error[i][domain] + tyler_graph->se_shared_features_error[i][domain]) / y_max + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->shared_features_error[i][domain] - tyler_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->shared_features_error[i][domain] - tyler_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->shared_features_error[i][domain] + tyler_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->shared_features_error[i][domain] + tyler_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < tyler_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * tyler_graph->shared_features_error[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_shared_features_error[i][domain] = -y_height * tyler_graph->shared_features_error[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }
    }

    else if (tyler_graph->type == FUNCTIONAL_FEATURES_ERROR) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * tyler_graph->functional_features_error[0][domain] / y_max + y_axis);
            for (i = 1; i < tyler_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * tyler_graph->functional_features_error[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            for (i = 0; i < tyler_lesion_level; i++) {
//                 printf("SE  = %f\n", tyler_graph->functional_features_error[i][domain]);


                cairo_move_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->functional_features_error[i][domain] - tyler_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->functional_features_error[i][domain] + tyler_graph->se_functional_features_error[i][domain]) / y_max + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->functional_features_error[i][domain] - tyler_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->functional_features_error[i][domain] - tyler_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->functional_features_error[i][domain] + tyler_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->functional_features_error[i][domain] + tyler_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < tyler_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * tyler_graph->functional_features_error[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_functional_features_error[i][domain] = -y_height * tyler_graph->functional_features_error[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }
    }

    else if (tyler_graph->type == PERCENT_PATTERNS_CORRECT) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * tyler_graph->percent_correct[0][domain] / y_max + y_axis);
            for (i = 1; i < tyler_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * tyler_graph->percent_correct[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            for (i = 0; i < tyler_lesion_level; i++) {

//                 printf("SE  = %f\n", tyler_graph->percent_correct[i][domain]);

                cairo_move_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->percent_correct[i][domain] - tyler_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->percent_correct[i][domain] + tyler_graph->se_percent_correct[i][domain]) / y_max + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->percent_correct[i][domain] - tyler_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->percent_correct[i][domain] - tyler_graph->se_percent_correct[i][domain]) / y_max  + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->percent_correct[i][domain] + tyler_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->percent_correct[i][domain] + tyler_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < tyler_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * tyler_graph->percent_correct[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_percent_correct[i][domain] = -y_height * tyler_graph->percent_correct[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }


    }
    else if (tyler_graph->type == PERCENT_PATTERNS_INCORRECT) {

        for(domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                if (domain == 0) {
                    cairo_set_source_rgb(cr, PURPLE);
                }
                else  {
                    cairo_set_source_rgb(cr, GREEN);

                }

                if (d) {
                    cairo_set_dash (cr, dash, 1, 0);

                }
                else {
                    cairo_set_dash (cr, dash, 0, 0);

                }

                cairo_move_to(cr, x_offset + 0 * x_space, -y_height * tyler_graph->percent_incorrect[0][domain][d] / y_max + y_axis);
                for (i = 1; i < tyler_lesion_level; i++) {
                    cairo_line_to(cr, x_offset + i * x_space, -y_height * tyler_graph->percent_incorrect[i][domain][d]/ y_max + y_axis);


                }

                cairo_stroke(cr);

                cairo_set_dash (cr, dash, 0, 0);

                if (error_bars) {
                    for (i = 0; i < tyler_lesion_level; i++) {


//                         printf("SE  = %f\n", tyler_graph->percent_incorrect[i][domain][d]);

                        cairo_move_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->percent_incorrect[i][domain][d] - tyler_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                        cairo_line_to(cr, x_offset + i * x_space, -y_height * (tyler_graph->percent_incorrect[i][domain][d] + tyler_graph->se_percent_incorrect[i][domain][d]) / y_max + y_axis);

                        cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->percent_incorrect[i][domain][d] - tyler_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                        cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->percent_incorrect[i][domain][d] - tyler_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);

                        cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (tyler_graph->percent_incorrect[i][domain][d] + tyler_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                        cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (tyler_graph->percent_incorrect[i][domain][d] + tyler_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                    }
                    cairo_stroke(cr);
                }

                for (i = 0; i < tyler_lesion_level; i++) {

                    cairo_arc(cr, x_offset + i * x_space, -y_height * tyler_graph->percent_incorrect[i][domain][d] / y_max + y_axis, radius, 0.0, 2*M_PI);
                    point_x_incorrect[i][domain][d] = x_offset + i * x_space;

                    //         }

                    point_y_percent_incorrect[i][domain][d] = -y_height * tyler_graph->percent_incorrect[i][domain][d] / y_max + y_axis;

                    cairo_fill(cr);


                }
            }
        }
    }

    // draw axes
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width (cr, 2.5);
    cairo_move_to(cr, x_axis, y_axis);
    cairo_line_to(cr, x_offset, y_axis);
    cairo_move_to(cr, x_offset, y_offset);
    cairo_line_to(cr, x_offset, y_axis);
    cairo_stroke(cr);
    //
    // draw axes' ticks
    cairo_set_line_width (cr, 1.0);


//     for (i = 0; i < ((int)y_segments + 1); i++) {
//         cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
//         cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
//         g_snprintf(buffer, 64, "%1.1f", 1 - i/ (double) y_segments);
//         cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
//         cairox_paint_pango_text(cr, &p, layout, buffer);
//     }
    for (i = 0; i < ((int)y_segments + 1); i++) {

        if ((tyler_graph->type == PERCENT_PATTERNS_CORRECT) || (tyler_graph->type == PERCENT_PATTERNS_INCORRECT)) {
            cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
            cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
            g_snprintf(buffer, 64, "%1.1f", (double) y_increment * (y_segments - i));// 1.0 - i);// / (double) y_segments);
            cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
//             printf("y_segments = %f\n", y_segments);
        } else {
            cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
            cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
            g_snprintf(buffer, 64, "%1.2f", (double) y_increment * (y_segments - i));// 1.0 - i);// / (double) y_segments);
            cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }

    }
    //
    cairo_stroke(cr);
    //


    for (i = 0; i < ((int)x_segments + 1); i++) {
        // 	  printf("i = %i\n", TYLER_LESION_LEVEL);
        cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
        cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
        g_snprintf(buffer, 64, "%1.1f", i * 100.0 / (double) tyler_lesion_level);
        cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
    }


}

static void draw_tyler_subgraph_original_model(cairo_t *cr, GtkTyler_Graph *tyler_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p, int subgraph)
{
    int i, domain, d;
    double y_max = 0.0;


    for (i = 0; i < tyler_lesion_level; i++) {

        if ((tyler_graph->type == DISTINCTIVE_FEATURES_ERROR) || (tyler_graph->type == FEATURES_LIVING_ERROR) || (tyler_graph->type == SHARED_FEATURES_ERROR) || (tyler_graph->type == SHARED_FEATURES_ERROR) || (tyler_graph->type == FUNCTIONAL_FEATURES_ERROR)) {

            for (domain = 0; domain < N_DOMAIN; domain++) {

                y_max = fmax(y_max, (tyler_graph->distinctive_features_error[i][domain] + tyler_graph->se_distinctive_features_error[i][domain]));
                if (i < tyler_original_lesion_level) {
                    y_max = fmax(y_max, distinctive_features_error_tyler[domain][i]);
                    y_max = fmax(y_max, shared_features_error_tyler[domain][i]);
                    y_max = fmax(y_max, functional_features_error_tyler[domain][i]);
                }
                y_max = fmax(y_max, (tyler_graph->distinctive_features_error[i][0] + tyler_graph->se_distinctive_features_error[i][0]));
                y_max = fmax(y_max, (tyler_graph->shared_features_error[i][0] + tyler_graph->se_distinctive_features_error[i][0]));
                y_max = fmax(y_max, (tyler_graph->shared_features_error[i][domain] + tyler_graph->se_shared_features_error[i][domain]));
                y_max = fmax(y_max, (tyler_graph->functional_features_error[i][domain] + tyler_graph->se_functional_features_error[i][domain]));
            }
            y_max = ceilf(y_max * 10.0) / 10.0;



        } else if ((tyler_graph->type == PERCENT_PATTERNS_CORRECT) ||  (tyler_graph->type == PERCENT_PATTERNS_INCORRECT)) {

            y_max = 100.0;
            // 	for (domain = 0; domain < N_DOMAIN; domain++) {
            //
            //             y_max = fmax(y_max, (tyler_graph->percent_correct[i][domain] + tyler_graph->se_percent_correct[i][domain]));
            // 	    y_max = fmax(y_max, (tyler_graph->percent_incorrect[i][domain][0] + tyler_graph->se_percent_incorrect[i][domain][0]));
            // 	    y_max = fmax(y_max, (tyler_graph->percent_incorrect[i][domain][1] + tyler_graph->se_percent_incorrect[i][domain][1]));
            // 	}
            // 	    y_max = ceilf(y_max / 10.0) * 10.0;

        }
    }

//     printf("y_max = %f\n", y_max);
    x_width = width - (x_offset * 2);
    y_height = height - (y_offset * 2);

    x_segments = (double) tyler_lesion_level;

    y_segments = (y_height > 100) ? 10.0 : 5.0;
    x_space = x_width / x_segments;
    y_space = y_height / y_segments;
    y_increment = y_max / y_segments;
    x_axis = width - x_offset;
    y_axis = height - y_offset;




    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);


    // Transparent background
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width(cr, 2.0);


    //     cairo_set_source_rgb(cr, 239/255.0, 187/255.0, 204/255.0);
    cairo_set_source_rgb(cr, 0, 0, 0);

    if (tyler_graph->type == DISTINCTIVE_FEATURES_ERROR) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * distinctive_features_error_tyler[domain][0] / y_max + y_axis);
            for (i = 1; i < tyler_original_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i *2* x_space, -y_height * distinctive_features_error_tyler[domain][i]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            for (i = 0; i < tyler_original_lesion_level; i++) {

                cairo_arc(cr, x_offset + 2*i * x_space, -y_height * distinctive_features_error_tyler[domain][i] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * 2*x_space;

                //         }

                point_y_distinctive_features_error[i][domain] = -y_height * distinctive_features_error_tyler[domain][i] / y_max + y_axis;

                cairo_fill(cr);

            }
        }

    } else if (tyler_graph->type == FEATURES_LIVING_ERROR) {

        cairo_set_source_rgb(cr, RED);


        cairo_move_to(cr, x_offset + 0 * x_space, -y_height * shared_features_error_tyler[0][0] / y_max + y_axis);
        for (i = 1; i < tyler_original_lesion_level; i++) {
            cairo_line_to(cr, x_offset + i *2* x_space, -y_height * shared_features_error_tyler[0][i]/ y_max + y_axis);

        }
        cairo_stroke(cr);


        for (i = 0; i < tyler_original_lesion_level; i++) {

            cairo_arc(cr, x_offset + i*2 * x_space, -y_height * shared_features_error_tyler[0][i]  / y_max + y_axis, radius, 0.0, 2*M_PI);
            point_x[i][0] = x_offset + i*2 * x_space;

            //         }

            point_y_features_living_error[i][0] = -y_height * shared_features_error_tyler[0][i] / y_max + y_axis;

            cairo_fill(cr);

        }

        cairo_set_source_rgb(cr, BLUE);

        cairo_move_to(cr, x_offset + 0 * x_space, -y_height * distinctive_features_error_tyler[0][0] / y_max + y_axis);
        for (i = 1; i < tyler_original_lesion_level; i++) {
            cairo_line_to(cr, x_offset + i*2 * x_space, -y_height * distinctive_features_error_tyler[0][i]/ y_max + y_axis);

        }
        cairo_stroke(cr);


        for (i = 0; i < tyler_original_lesion_level; i++) {

            cairo_arc(cr, x_offset + i*2 * x_space, -y_height * distinctive_features_error_tyler[0][i]  / y_max + y_axis, radius, 0.0, 2*M_PI);
            point_x[i][1] = x_offset + i*2 * x_space;

            //         }

            point_y_features_living_error[i][1] = -y_height * distinctive_features_error_tyler[0][i] / y_max + y_axis;

            cairo_fill(cr);

        }

    } else if (tyler_graph->type == SHARED_FEATURES_ERROR) {
        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * shared_features_error_tyler[domain][0] / y_max + y_axis);
            for (i = 1; i < tyler_original_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i*2 * x_space, -y_height * shared_features_error_tyler[domain][i]/ y_max + y_axis);

            }
            cairo_stroke(cr);


            for (i = 0; i < tyler_original_lesion_level; i++) {

                cairo_arc(cr, x_offset + i*2 * x_space, -y_height * shared_features_error_tyler[domain][i] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i *2* x_space;

                //         }

                point_y_shared_features_error[i][domain] = -y_height * shared_features_error_tyler[domain][i] / y_max + y_axis;

                cairo_fill(cr);

            }
        }
    }

    else if (tyler_graph->type == FUNCTIONAL_FEATURES_ERROR) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * functional_features_error_tyler[domain][0] / y_max + y_axis);
            for (i = 1; i < tyler_original_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i *2* x_space, -y_height * functional_features_error_tyler[domain][i]/ y_max + y_axis);

            }
            cairo_stroke(cr);


            for (i = 0; i < tyler_original_lesion_level; i++) {

                cairo_arc(cr, x_offset + i*2 * x_space, -y_height * functional_features_error_tyler[domain][i] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i*2 * x_space;

                //         }

                point_y_functional_features_error[i][domain] = -y_height * functional_features_error_tyler[domain][i] / y_max + y_axis;

                cairo_fill(cr);

            }
        }
    }

    else if (tyler_graph->type == PERCENT_PATTERNS_CORRECT) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * percent_correct_tyler[domain][0] / y_max + y_axis);
            for (i = 1; i < tyler_original_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i *2* x_space, -y_height *  percent_correct_tyler[domain][i]/ y_max + y_axis);

            }
            cairo_stroke(cr);


            for (i = 0; i < tyler_original_lesion_level; i++) {

                cairo_arc(cr, x_offset + i *2* x_space, -y_height * percent_correct_tyler[domain][i] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i *2* x_space;

                //         }

                point_y_percent_correct[i][domain] = -y_height * percent_correct_tyler[domain][i] / y_max + y_axis;

                cairo_fill(cr);

            }
        }


    }
    else if (tyler_graph->type == PERCENT_PATTERNS_INCORRECT) {

        for(domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                if (domain == 0) {
                    cairo_set_source_rgb(cr, PURPLE);
                }
                else  {
                    cairo_set_source_rgb(cr, GREEN);

                }

                if (d) {
                    cairo_set_dash (cr, dash, 1, 0);

                }
                else {
                    cairo_set_dash (cr, dash, 0, 0);

                }

                cairo_move_to(cr, x_offset + 0 * x_space, -y_height * percent_incorrect_tyler[d][domain] [0]/ y_max + y_axis);
                for (i = 1; i < tyler_original_lesion_level; i++) {
                    cairo_line_to(cr, x_offset + i*2 * x_space, -y_height * percent_incorrect_tyler[d][domain][i]/ y_max + y_axis);

                }

                cairo_stroke(cr);

                cairo_set_dash (cr, dash, 0, 0);



                for (i = 0; i < tyler_original_lesion_level; i++) {

                    cairo_arc(cr, x_offset + i*2 * x_space, -y_height * percent_incorrect_tyler[d][domain][i] / y_max + y_axis, radius, 0.0, 2*M_PI);
                    point_x_incorrect[i][domain][d] = x_offset + i *2* x_space;

                    //         }

                    point_y_percent_incorrect[i][domain][d] = -y_height * percent_incorrect_tyler[d][domain][i] / y_max + y_axis;

                    cairo_fill(cr);


                }
            }
        }
    }

// draw axes
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width (cr, 2.5);
    cairo_move_to(cr, x_axis, y_axis);
    cairo_line_to(cr, x_offset, y_axis);
    cairo_move_to(cr, x_offset, y_offset);
    cairo_line_to(cr, x_offset, y_axis);
    cairo_stroke(cr);
//
// draw axes' ticks
    cairo_set_line_width (cr, 1.0);


//     for (i = 0; i < ((int)y_segments + 1); i++) {
    //         cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
//         cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
//         g_snprintf(buffer, 64, "%1.1f", 1 - i/ (double) y_segments);
//         cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
//         cairox_paint_pango_text(cr, &p, layout, buffer);
//     }
    for (i = 0; i < ((int)y_segments + 1); i++) {

        if ((tyler_graph->type == PERCENT_PATTERNS_CORRECT) || (tyler_graph->type == PERCENT_PATTERNS_INCORRECT)) {
            cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
            cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
            g_snprintf(buffer, 64, "%1.1f", (double) y_increment * (y_segments - i));// 1.0 - i);// / (double) y_segments);
            cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
//             printf("y_segments = %f\n", y_segments);
        } else {
            cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
            cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
            g_snprintf(buffer, 64, "%1.2f", (double) y_increment * (y_segments - i));// 1.0 - i);// / (double) y_segments);
            cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }

    }
//
    cairo_stroke(cr);
//


    for (i = 0; i < ((int)x_segments + 1); i++) {
        // 	  printf("i = %i\n", TYLER_LESION_LEVEL);
        cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
        cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
        g_snprintf(buffer, 64, "%1.1f", i * 100.0 / (double) tyler_lesion_level);
        cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
    }


}

static void draw_tyler_legend(cairo_t *cr, GtkTyler_Graph *tyler_graph, PangoLayout *layout, CairoxTextParameters p)
{
    // 	//


    pangox_layout_set_font_size(layout, 13);
    double text_width = 0.0;
    pangox_layout_select_font_from_string(layout, "CMU Serif");
    
    //draw legend
    cairo_translate(cr, 2*x_offset, 0);

    //
    if ((tyler_graph->type == DISTINCTIVE_FEATURES_ERROR) || (tyler_graph->type == SHARED_FEATURES_ERROR) || (tyler_graph->type == FUNCTIONAL_FEATURES_ERROR) || (tyler_graph->type == PERCENT_PATTERNS_CORRECT)) {

        g_snprintf(buffer, 64, "%s", "Animals:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 2*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "%s", "Artifacts:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 2.5*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);


        cairo_set_source_rgb(cr, PURPLE);
        cairo_move_to(cr, 0.5*x_offset + text_width, 2*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 2*y_offset+1);
        cairo_stroke(cr);
        cairo_set_source_rgb(cr, GREEN);
        cairo_move_to(cr, 0.5*x_offset + text_width, 2.5*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 2.5*y_offset+1);
        cairo_stroke(cr);

    } else if (tyler_graph->type == FEATURES_LIVING_ERROR) {


        g_snprintf(buffer, 64, "%s", "Shared:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 2*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "%s", "Distinctive:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 2.5*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        cairo_set_source_rgb(cr, RED);
        cairo_move_to(cr, 0.5*x_offset + text_width, 2*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 2*y_offset+1);
        cairo_stroke(cr);
        cairo_set_source_rgb(cr, BLUE);
        cairo_move_to(cr, 0.5*x_offset + text_width, 2.5*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 2.5*y_offset+1);
        cairo_stroke(cr);

    }     else if (tyler_graph->type == PERCENT_PATTERNS_INCORRECT) {


        g_snprintf(buffer, 64, "%s", "Animals Between:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 2*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "%s", "Animals Within:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 2.5*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);


        g_snprintf(buffer, 64, "%s", "Artifacts Between:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 3*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        g_snprintf(buffer, 64, "%s", "Artifacts Within:");
        text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
        cairox_text_parameters_set(&p, 0, 3.5*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);

        cairo_set_source_rgb(cr, PURPLE);
        cairo_set_dash (cr, dash, 1, 0);

        cairo_move_to(cr, 0.5*x_offset + text_width, 2*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 2*y_offset+1);
        cairo_stroke(cr);
        cairo_set_dash (cr, dash, 0, 0);

        cairo_move_to(cr, 0.5*x_offset + text_width, 2.5*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 2.5*y_offset+1);
        cairo_stroke(cr);

        cairo_set_source_rgb(cr, GREEN);
        cairo_set_dash (cr, dash, 1, 0);

        cairo_move_to(cr, 0.5*x_offset + text_width, 3*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 3*y_offset+1);
        cairo_stroke(cr);
        cairo_set_dash (cr, dash, 0, 0);

        cairo_move_to(cr, 0.5*x_offset + text_width, 3.5*y_offset+1);
        cairo_line_to(cr, 4*x_offset-5, 3.5*y_offset+1);
        cairo_stroke(cr);

    }



    cairo_translate(cr, -(2*x_offset), 0);






    //     g_snprintf(buffer, 64, "%s", "General Pictures:");
    //     text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
    //     cairox_text_parameters_set(&p, 00, 3*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
    //     cairox_paint_pango_text(cr, &p, layout, buffer);
    //
    //     g_snprintf(buffer, 64, "%s", "Specific Pictures:");
    //     text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
    //     cairox_text_parameters_set(&p, 0, 3.5*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
    //     cairox_paint_pango_text(cr, &p, layout, buffer);
//     cairo_set_source_rgb(cr, 56/255.0,58/255.0,102/255.0);
//             cairo_set_dash (cr, superordinate_dash, 0, 0);
//     cairo_move_to(cr, 0.5*x_offset + text_width, 3*y_offset+1);
//     cairo_line_to(cr, 4*x_offset-5, 3*y_offset+1);
//     cairo_stroke(cr);
// //             cairo_set_dash (cr, superordinate_dash, 1, 0);
//     cairo_move_to(cr, 0.5*x_offset + text_width, 3.5*y_offset+1);
//     cairo_line_to(cr, 4*x_offset-5, 3.5*y_offset+1);
//     cairo_stroke(cr);


//          }

}

static void draw_tyler_axes_labels(cairo_t *cr, GtkTyler_Graph *tyler_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p)
{

//     if (tyler_graph->type == 0)
//     {
    g_snprintf(buffer, 64, "%s", y_label[tyler_graph->type]);
    cairox_text_parameters_set(&p, x_offset - 43.0, (height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
    cairox_paint_pango_text(cr, &p, layout, buffer);
    cairo_stroke(cr);


//     }
//     else
//     {
//         int i;
//         double quarter_height = height / 4.0;
//         for (i = 0; i < 4; i++) {
//
//             g_snprintf(buffer, 64, "%s", y_label[i]);
//             cairox_text_parameters_set(&p, x_offset - 33.0, (quarter_height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
//             cairox_paint_pango_text(cr, &p, layout, buffer);
//             cairo_translate(cr, 0, quarter_height);
//         }
//         cairo_stroke(cr);
//
//         cairo_translate(cr, 0, -height- y_offset/2.0);
//         cairo_stroke(cr);
//
//
//     }



    g_snprintf(buffer, 64, "Percent Connections Lesioned");
    cairox_text_parameters_set(&p, (width - x_offset)/2.0, (height - y_offset) + 27.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
    cairox_paint_pango_text(cr, &p, layout, buffer);
    cairo_stroke(cr);


    //
}

static void draw_tyler_graph(cairo_t *cr, GtkWidget *widget, double width, double height)
{


//     double width = (double) widget->allocation.width, height = (double) widget->allocation.height;
    GtkTyler_Graph *tyler_graph =  GTK_TYLER_GRAPH(widget);
    PangoLayout *layout = pango_cairo_create_layout(cr);
    CairoxTextParameters p;


    x_offset = 50.0;
    y_offset =  40.0;


    draw_tyler_legend(cr, GTK_TYLER_GRAPH(widget), layout, p);

    draw_tyler_axes_labels(cr, tyler_graph, width, height, layout, p);


    if (tyler_graph->data_type == MODEL) {
        draw_tyler_subgraph(cr, tyler_graph, width, height, layout, p, -1);
    }
    else if (tyler_graph->data_type == TYLER) {
        draw_tyler_subgraph_original_model(cr, tyler_graph, width, height, layout, p, -1);

    }

//     }
//     else
//     {
//         int i;
//         for (i = 0; i < 4; i++) {
//
//             draw_tyler_subgraph(cr, tyler_graph, width, height / 4.0, layout, p, i);
//             cairo_translate(cr, 0, height / 4.0);
//         }
//
//     }
    g_object_unref(layout);
    cairo_destroy(cr);

}
void gtk_tyler_graph_pdf(GtkWidget *widget, char *filename)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = 800;//widget->allocation.width;
    int height = 500;//widget->allocation.height;
    char name[100];
    int i = 0;
//     GtkTyler_Graph *tyler_graph = GTK_TYLER_GRAPH(widget);

    while (filename[i] != '\0') {
        name[i] = filename[i];
        i++;
    }
    name[i] = '\0';
//     printf("%s\n", name);
    sprintf(name, "%s_tyler_task_%d.pdf", name, GTK_TYLER_GRAPH(widget)->type);



    surface = cairo_pdf_surface_create(name, width, height);
    cr = cairo_create(surface);
    // draw tyler_graph
    draw_tyler_graph(cr, widget, width, height);


    cairo_surface_destroy(surface);

}


static void gtk_tyler_graph_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    draw_tyler_graph(cr, widget, (double) widget->allocation.width, (double) widget->allocation.height);
}

void gtk_tyler_graph_reset(GtkTyler_Graph *tyler_graph)
{
    gtk_tyler_graph_init(tyler_graph);
    gtk_tyler_graph_redraw(GTK_WIDGET(tyler_graph));


}

static void gtk_tyler_graph_destroy(GtkObject *object)
{
//     GtkTyler_Graph *tyler_graph;
    GtkTyler_GraphClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_TYLER_GRAPH(object));

//     tyler_graph = GTK_TYLER_GRAPH(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_tyler_graph_redraw(GtkWidget *widget)
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

void gtk_tyler_graph_clear(GtkWidget *widget)
{
    init_tyler_response(0, 0, 0);
    gtk_tyler_graph_reset(GTK_TYLER_GRAPH(widget));

}

