#include "aoa_graph.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_aoa_graph_class_init(GtkAoA_GraphClass *klass);
GtkType gtk_aoa_graph_get_type(void);
static void gtk_aoa_graph_init(GtkAoA_Graph *aoa_graph);
static void gtk_aoa_graph_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_aoa_graph_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_aoa_graph_realize(GtkWidget *widget);
static gboolean gtk_aoa_graph_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_aoa_graph_paint(GtkWidget *widget);
static void gtk_aoa_graph_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

static char buffer[64];
// double dash[1] = {5.0};  defined in aoa_graph.c
double dash[1];
// int error_bars = 1;      ditto
int   error_bars;




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

double point_x[AOA_LESION_LEVEL][N_DOMAIN];
double point_x_incorrect[AOA_LESION_LEVEL][N_DOMAIN][2];
double point_y_error[AOA_LESION_LEVEL][N_DOMAIN];
double point_y_percent_correct[AOA_LESION_LEVEL][N_DOMAIN];
double point_y_percent_incorrect[AOA_LESION_LEVEL][N_DOMAIN][2];

double sample_mean_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN], sample_mean_percent_correct[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN],  sample_mean_percent_incorrect[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN][2];


double network_mean_error[AOA_NETWORK][AOA_LESION_LEVEL][N_DOMAIN], network_mean_percent_correct[AOA_NETWORK][AOA_LESION_LEVEL][N_DOMAIN],  network_mean_percent_incorrect[AOA_NETWORK][AOA_LESION_LEVEL][N_DOMAIN][2];


// double patient_error[4] = {1.0, };
// double patient_shared_features_error[4] =

#define n_patterns (n_pattern[0] + n_pattern[1])
#define PURPLE		0.5, 0.0, 0.5
#define GREEN		0.5, 0.5, 0.0

#define RED		0.6, 0.25, 0.35
#define BLUE		0.25, 0.35, 0.6

GtkType gtk_aoa_graph_get_type(void)
{
    static GtkType gtk_aoa_graph_type = 0;
    if (!gtk_aoa_graph_type)
    {
        static const GtkTypeInfo gtk_aoa_graph_info = {"GtkAoA_Graph", sizeof(GtkAoA_Graph), sizeof(GtkAoA_GraphClass), (GtkClassInitFunc) gtk_aoa_graph_class_init, (GtkObjectInitFunc) gtk_aoa_graph_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_aoa_graph_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_aoa_graph_info);
    }
    return gtk_aoa_graph_type;
}



static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    double coordx = 0, coordy = 0;
    GtkAoA_Graph *aoa_graph = GTK_AOA_GRAPH(widget);

    gtk_aoa_graph_redraw(widget);



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



    for (i = 0; i < aoa_lesion_level; i++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {

            if ((coordx >= (point_x[i][domain] - radius*1.5)) && (coordx <= (point_x[i][domain] + radius*1.5))) {



                if(aoa_graph->type == ERROR_BY_DOMAIN) {
                    if (break_again) {
                        break;
                    }

                    if ((coordy >= (point_y_error[i][domain] - radius*1.5)) && (coordy <= (point_y_error[i][domain] + radius*1.5))) {

//                         if(aoa_graph->data_type == MODEL) {
                        g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->error[i][domain], i * 100.0 / (double) aoa_lesion_level);

//                         } else if(aoa_graph->data_type == AOA) {
//                             if (i < aoa_original_lesion_level) {
//                                 g_snprintf (buffer, 64, "%1.2f, %1.1f%%", error_aoa[domain][i], i * 100.0 / (double) aoa_lesion_level);
//                             } else {
//                                 break;
//                             }
//                         }

                        pangox_layout_set_font_size(layout, 10.0);
                        cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                        cairox_paint_pango_text (cr, &text_p, layout, buffer);
                        break_again = 1;
                        break;

                    }

                }

//                 else if(aoa_graph->type == SHARED_FEATURES_ERROR) {
//                     if (break_again) {
//                         break;
//                     }
//
//                     if ((coordy >= (point_y_shared_features_error[i][domain] - radius*1.5)) && (coordy <= (point_y_shared_features_error[i][domain] + radius*1.5))) {
//
//
//                         g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->shared_features_error[i][domain], i * 100.0 / (double) aoa_lesion_level);
//
//                         pangox_layout_set_font_size(layout, 10.0);
//                         cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_shared_features_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
//                         cairox_paint_pango_text (cr, &text_p, layout, buffer);
//                         break_again = 1;
//                         break;
//
//                     }
//
//                 }
//
//                 else if(aoa_graph->type == FEATURES_LIVING_ERROR) {
//                     if (break_again) {
//                         break;
//                     }
//
//                     if ((coordy >= (point_y_features_living_error[i][domain] - radius*1.5)) && (coordy <= (point_y_features_living_error[i][domain] + radius*1.5))) {
//
//                         if (domain == 0) {
//                             g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->shared_features_error[i][0], i * 100.0 / (double) aoa_lesion_level);
//                         } else {
//                             g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->error[i][0], i * 100.0 / (double) aoa_lesion_level);
//
//                         }
//
//                         pangox_layout_set_font_size(layout, 10.0);
//                         cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_features_living_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
//                         cairox_paint_pango_text (cr, &text_p, layout, buffer);
//                         break_again = 1;
//                         break;
//
//                     }
//
//                 }
//
//                 else if(aoa_graph->type == FUNCTIONAL_FEATURES_ERROR) {
//                     if (break_again) {
//                         break;
//                     }
//
//                     if ((coordy >= (point_y_functional_features_error[i][domain] - radius*1.5)) && (coordy <= (point_y_functional_features_error[i][domain] + radius*1.5))) {
//
//
//                         g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->functional_features_error[i][domain], i * 100.0 / (double) aoa_lesion_level);
//
//                         pangox_layout_set_font_size(layout, 10.0);
//                         cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_functional_features_error[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
//                         cairox_paint_pango_text (cr, &text_p, layout, buffer);
//                         break_again = 1;
//                         break;
//
//                     }
//
//                 }

                else if(aoa_graph->type == PERCENT_PATTERNS_CORRECT) {
                    if (break_again) {
                        break;
                    }

                    if ((coordy >= (point_y_percent_correct[i][domain] - radius*1.5)) && (coordy <= (point_y_percent_correct[i][domain] + radius*1.5))) {


                        g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->percent_correct[i][domain], i * 100.0 / (double) aoa_lesion_level);

                        pangox_layout_set_font_size(layout, 10.0);
                        cairox_text_parameters_set (&text_p, point_x[i][domain] + radius*1.5, point_y_percent_correct[i][domain] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                        cairox_paint_pango_text (cr, &text_p, layout, buffer);
                        break_again = 1;
                        break;

                    }

                }

                else if(aoa_graph->type == PERCENT_PATTERNS_INCORRECT) {

                    for (d = 0; d < 2; d++) {
                        if (break_again) {
                            break;
                        }

                        if ((coordy >= (point_y_percent_incorrect[i][domain][d] - radius*1.5)) && (coordy <= (point_y_percent_incorrect[i][domain][d] + radius*1.5))) {


                            g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->percent_incorrect[i][domain][d], i * 100.0 / (double) aoa_lesion_level);

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
//                     g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->functional_features_error[i], i * 100.0 / (double) aoa_lesion_level);
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
//                     g_snprintf (buffer, 64, "%1.2f, %1.1f%%", aoa_graph->shared_features_error[i], i * 100.0 / (double) aoa_lesion_level);
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

GtkWidget * gtk_aoa_graph_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_aoa_graph_get_type()));
}



static void gtk_aoa_graph_class_init(GtkAoA_GraphClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_aoa_graph_realize;
    widget_class->size_request = gtk_aoa_graph_size_request;
    widget_class->size_allocate = gtk_aoa_graph_size_allocate;
    widget_class->expose_event = gtk_aoa_graph_expose;
    object_class->destroy = gtk_aoa_graph_destroy;
}


static char y_label[6][100];
double x_offset;
double y_offset;
int n_pattern[2];
static void gtk_aoa_graph_init(GtkAoA_Graph *aoa_graph)
{

    int l, domain, d, n;

    for (l = 0; l < AOA_LESION_LEVEL; l++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {
            aoa_graph->error[l][domain] = 0.0;
//             aoa_graph->shared_features_error[l][domain] = 0.0;
//             aoa_graph->functional_features_error[l][domain] = 0.0;

//             aoa_graph->se_error[l][domain] = 0.0;
//             aoa_graph->se_shared_features_error[l][domain] = 0.0;
//             aoa_graph->se_functional_features_error[l][domain] = 0.0;
            aoa_graph->se_percent_correct[l][domain] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][0] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][1] = 0.0;
        }
    }
    sprintf(y_label[0], "Error for Each Category");
// //     sprintf(y_label[1], "Error for Features");
//     sprintf(y_label[2], "Error for Shared Features");
// // //     sprintf(y_label[3], "Error for Functional Features");
// //     sprintf(y_label[4], "Percent Patterns Correct");
//     sprintf(y_label[5], "Percent Patterns Incorrect");

    aoa_graph->type = ERROR_BY_DOMAIN;
    aoa_graph->networks = 0;

//     if (EXCLUDE_FRUIT) {
//
//         n_pattern[0] = get_aoa_domain_n_pattern(0) - get_n_fruit();// animals
//         n_pattern[1] = get_aoa_domain_n_pattern(1);
//     }
//     else
//     {
    //We need to calculate our own domains

    n_pattern[0] = abs(P/2.0);
    n_pattern[1] = abs(P/2.0);
//     }




    for (l = 0; l < AOA_LESION_LEVEL; l++) {



        for (domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                aoa_graph->percent_incorrect[l][domain][d] = 0.0; // between/within per domain
                point_x_incorrect[l][domain][d] = 0.0;
                point_y_percent_incorrect[l][domain][d] = 0.0;
            }
            point_y_error[l][domain] = 0.0;



            aoa_graph->error[l][domain] = 0.0;
            aoa_graph->se_error[l][domain] = 0.0;
            aoa_graph->se_percent_correct[l][domain] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][0] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][1] = 0.0;
            point_x[l][domain] = 0.0;

            aoa_graph->percent_correct[l][domain] = 0.0;


        }
        for (d = 0; d < AOA_LESION_DISTRIBUTION; d++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                sample_mean_error[d][l][domain] = 0.0;
                sample_mean_percent_correct[d][l][domain] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][0] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][1] = 0.0;
            }
        }
        for (n = 0; n < AOA_NETWORK; n++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                network_mean_error[n][l][domain] = 0.0;
                network_mean_percent_correct[n][l][domain] = 0.0;
                network_mean_percent_incorrect[n][l][domain][0] = 0.0;
                network_mean_percent_incorrect[n][l][domain][1] = 0.0;
            }
        }
    }


    gtk_widget_add_events(GTK_WIDGET(aoa_graph), GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(aoa_graph), "button-press-event", G_CALLBACK(clicked), NULL);

}

void gtk_aoa_graph_networks(GtkAoA_Graph *aoa_graph, int value)
{
    // whether or not the graph uses more than one file, to average over multiple networks
    aoa_graph->networks = value;

    printf("aoa_graph->networks = %i\n", aoa_graph->networks);


}

int gtk_aoa_graph_get_networks(GtkAoA_Graph *aoa_graph)
{
    // whether or not the graph uses more than one file, to average over multiple networks
    return aoa_graph->networks;



}

void gtk_aoa_graph_set_type(GtkAoA_Graph *aoa_graph, int type)
{
    aoa_graph->type = type;

}

void gtk_aoa_graph_set_data_type(GtkAoA_Graph *aoa_graph, int type)
{
    aoa_graph->data_type = type;

    gtk_aoa_graph_update(aoa_graph);

    gtk_aoa_graph_redraw(GTK_WIDGET(aoa_graph));

}


static void gtk_aoa_graph_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_AOA_GRAPH(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

//     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_aoa_graph_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_AOA_GRAPH(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

//     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_aoa_graph_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_AOA_GRAPH(widget));

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

static gboolean gtk_aoa_graph_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_AOA_GRAPH(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_aoa_graph_paint(widget);

    return FALSE;
}


extern double activation_threshold;
void gtk_aoa_set_auto_name(GtkAoA_Graph *aoa_graph, const char auto_name[]) {
    aoa_graph->auto_name = &auto_name[0];
}



void gtk_aoa_graph_update(GtkAoA_Graph *aoa_graph)
{


    //   aoa_lesion_level = 4;
    gtk_aoa_graph_init(aoa_graph);

    //  sample_mean_shared_features_error_overall[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL], sample_mean_error_overall[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL], sample_mean_functional_features_error_overall[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL],

//     double sample_mean_shared_features_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN], sample_mean_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN], sample_mean_functional_features_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN], sample_mean_percent_correct[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN],  sample_mean_percent_incorrect[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN][2];


    // if (aoa_graph->type == 0) {
    int s, p, l, d, domain; // p is target, q is network's response
    for (l = 0; l < AOA_LESION_LEVEL; l++) {


        //         aoa_graph->se_shared_features_error_overall[l] = 0.0;
        //         aoa_graph->se_error_overall[l] = 0.0;
        //         aoa_graph->se_functional_features_error_overall[l] = 0.0;



        for (domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                aoa_graph->percent_incorrect[l][domain][d] = 0.0; // between/within per domain
                point_x_incorrect[l][domain][d] = 0.0;
                point_y_percent_incorrect[l][domain][d] = 0.0;
            }
            point_y_error[l][domain] = 0.0;  /*
            point_y_shared_features_error[l][domain] = 0.0;
            point_y_features_living_error[l][domain] = 0.0;
            point_y_functional_features_error[l][domain] = 0.0;  */


            aoa_graph->error[l][domain] = 0.0;   /*
            aoa_graph->error[l][domain] = 0.0;
            aoa_graph->functional_features_error[l][domain] = 0.0;  */


            aoa_graph->se_error[l][domain] = 0.0;/*
            aoa_graph->se_error[l][domain] = 0.0;
            aoa_graph->se_functional_features_error[l][domain] = 0.0; */
            aoa_graph->se_percent_correct[l][domain] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][0] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][1] = 0.0;
            point_x[l][domain] = 0.0;

            aoa_graph->percent_correct[l][domain] = 0.0;


        }
        for (d = 0; d < AOA_LESION_DISTRIBUTION; d++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                sample_mean_error[d][l][domain] = 0.0;/*
                sample_mean_error[d][l][domain] = 0.0;
                sample_mean_functional_features_error[d][l][domain] = 0.0;*/
                sample_mean_percent_correct[d][l][domain] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][0] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][1] = 0.0;
            }
        }

    }




    for (d = 0; d < aoa_lesion_distribution; d++) {
        for (l = 0; l < aoa_lesion_level; l++) {
            for (p = 0; p < P; p++) {
//                 if (!excluded_pattern(p)) {
                for (s = 0; s < aoa_sample; s++) {

                    aoa_graph->error[l][get_aoa_domain(p)] += aoa_error[d][l][s][p];
                    sample_mean_error[d][l][get_aoa_domain(p)] += aoa_error[d][l][s][p];

//                     aoa_graph->shared_features_error[l][get_aoa_domain(p)] += aoa_shared_error[d][l][s][p];
//                     sample_mean_shared_features_error[d][l][get_aoa_domain(p)] += aoa_shared_error[d][l][s][p];
//
//                     aoa_graph->functional_features_error[l][get_aoa_domain(p)] += aoa_functional_error[d][l][s][p];
//                     sample_mean_functional_features_error[d][l][get_aoa_domain(p)] += aoa_functional_error[d][l][s][p];


                    if (aoa_response[d][l][s][p] == p) {
                        (aoa_graph->percent_correct[l][get_aoa_domain(p)])++;
                        (sample_mean_percent_correct[d][l][get_aoa_domain(p)])++;
                        //                             printf("percent_correct[%i][%i] = %f\n", l,get_aoa_domain(p), aoa_graph->percent_correct[l][get_aoa_domain(p)]);
                    }
                    else if (get_aoa_domain(aoa_response[d][l][s][p]) == get_aoa_domain(p)) { //responce and target pattern are in the same domain
                        (aoa_graph->percent_incorrect[l][get_aoa_domain(p)][0])++;
                        (sample_mean_percent_incorrect[d][l][get_aoa_domain(p)][0])++;
                    }
                    else {
                        (aoa_graph->percent_incorrect[l][get_aoa_domain(p)][1])++;
                        (sample_mean_percent_incorrect[d][l][get_aoa_domain(p)][1])++;
                    }
                }

//                 }
            }
        }
    }


    for (l = 0; l < aoa_lesion_level; l++) {
        //         printf("aoa_graph->error[%d] = %f\n", l, aoa_graph->error[l]);
        //         aoa_graph->error_overall[l] /= (double) (aoa_lesion_distribution * aoa_sample * n_patterns);
        // 	aoa_graph->shared_features_error_overall[l] /= (double) (aoa_lesion_distribution * aoa_sample * n_patterns);
        // 	aoa_graph->functional_features_error_overall[l] /= (double) (aoa_lesion_distribution * aoa_sample * n_patterns);
        for (domain = 0; domain < N_DOMAIN; domain++) {

            aoa_graph->error[l][domain] /= (double) (aoa_lesion_distribution * aoa_sample * n_pattern[domain]);/*
            aoa_graph->shared_features_error[l][domain] /= (double) (aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->functional_features_error[l][domain] /= (double) (aoa_lesion_distribution * aoa_sample * n_pattern[domain]);       */
            aoa_graph->percent_correct[l][domain] /= (double) (aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->percent_correct[l][domain] *= 100.0;
            aoa_graph->percent_incorrect[l][domain][0] /= (double) (aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->percent_incorrect[l][domain][1] /= (double) (aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->percent_incorrect[l][domain][0] *= 100.0;
            aoa_graph->percent_incorrect[l][domain][1] *= 100.0;


        }

        //this is going to have to be [d][l][domain]
        for (d = 0; d < aoa_lesion_distribution; d++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {
                sample_mean_error[d][l][domain] /= (double) (aoa_sample * n_pattern[domain]);
// //                 sample_mean_shared_features_error[d][l][domain] /= (double) (aoa_sample * n_pattern[domain]);
//                 sample_mean_functional_features_error[d][l][domain] /= (double) (aoa_sample * n_pattern[domain]);

                sample_mean_percent_correct[d][l][domain] /=  (double) (aoa_sample * n_pattern[domain]);
                sample_mean_percent_correct[d][l][domain] *=  100.0;
                sample_mean_percent_incorrect[d][l][domain][0] /=  (double) (aoa_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][0] *= 100.0;
                sample_mean_percent_incorrect[d][l][domain][1] /=  (double) (aoa_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][1] *= 100.0;


            }


            //             sample_mean_error_overall[d][l] /= (double) (aoa_sample * n_patterns);
            //             sample_mean_shared_features_error_overall[d][l] /= (double) (aoa_sample * n_patterns);
            //             sample_mean_functional_features_error_overall[d][l] /= (double) (aoa_sample * n_patterns);
        }
    }

    for (d = 0; d < aoa_lesion_distribution; d++) {
        for (l = 0; l < aoa_lesion_level; l++) {
            //             aoa_graph->se_error_overall[l] += sqr(sample_mean_error_overall[d][l] - aoa_graph->error_overall[l]);
            //             aoa_graph->se_shared_features_error_overall[l] += sqr(sample_mean_shared_features_error_overall[d][l] - aoa_graph->shared_features_error_overall[l]);
            //             aoa_graph->se_functional_features_error_overall[l] += sqr(sample_mean_functional_features_error_overall[d][l] - aoa_graph->functional_features_error_overall[l]);

            for (domain = 0; domain < N_DOMAIN; domain++) {
                aoa_graph->se_error[l][domain] += sqr(sample_mean_error[d][l][domain] - aoa_graph->error[l][domain]);
//                 aoa_graph->se_shared_features_error[l][domain] += sqr(sample_mean_shared_features_error[d][l][domain] - aoa_graph->shared_features_error[l][domain]);
//                 aoa_graph->se_functional_features_error[l][domain] += sqr(sample_mean_functional_features_error[d][l][domain] - aoa_graph->functional_features_error[l][domain]);
                aoa_graph->se_percent_correct[l][domain] += sqr(sample_mean_percent_correct[d][l][domain] - aoa_graph->percent_correct[l][domain]);
                aoa_graph->se_percent_incorrect[l][domain][0] += sqr(sample_mean_percent_incorrect[d][l][domain][0] - aoa_graph->percent_incorrect[l][domain][0]);
                aoa_graph->se_percent_incorrect[l][domain][1] += sqr(sample_mean_percent_incorrect[d][l][domain][1] - aoa_graph->percent_incorrect[l][domain][1]);
            }
        }

    }

    for (l = 0; l < aoa_lesion_level; l++) {
        //         aoa_graph->se_error_overall[l] /= (double) aoa_lesion_distribution;
        //         aoa_graph->se_shared_features_error_overall[l] /= (double) aoa_lesion_distribution;
        //         aoa_graph->se_functional_features_error_overall[l] /= (double) aoa_lesion_distribution;

        for (domain = 0; domain < N_DOMAIN; domain++) {
            aoa_graph->se_error[l][domain] /= (double) aoa_lesion_distribution;
//             aoa_graph->se_shared_features_error[l][domain] /= (double) aoa_lesion_distribution;
//             aoa_graph->se_functional_features_error[l][domain] /= (double) aoa_lesion_distribution;
            aoa_graph->se_percent_correct[l][domain] /= (double) aoa_lesion_distribution;
            aoa_graph->se_percent_incorrect[l][domain][0] /= (double) aoa_lesion_distribution;
            aoa_graph->se_percent_incorrect[l][domain][1] /= (double) aoa_lesion_distribution;
            aoa_graph->se_error[l][domain] = sqrt(aoa_graph->se_error[l][domain]) / (double) sqrt(aoa_lesion_distribution);
//             aoa_graph->se_shared_features_error[l][domain] = sqrt(aoa_graph->se_shared_features_error[l][domain]) / (double) sqrt(aoa_lesion_distribution);
//             aoa_graph->se_functional_features_error[l][domain] = sqrt(aoa_graph->se_functional_features_error[l][domain]) / (double) sqrt(aoa_lesion_distribution);
            aoa_graph->se_percent_correct[l][domain] = sqrt(aoa_graph->se_percent_correct[l][domain]);
            aoa_graph->se_percent_incorrect[l][domain][0] = sqrt(aoa_graph->se_percent_incorrect[l][domain][0]);
            aoa_graph->se_percent_incorrect[l][domain][1] = sqrt(aoa_graph->se_percent_incorrect[l][domain][1]);
        }

        //         aoa_graph->se_error_overall[l] = sqrt(aoa_graph->se_error_overall[l]) / (double) sqrt(aoa_lesion_distribution);
        //         aoa_graph->se_shared_features_error_overall[l] = sqrt(aoa_graph->se_shared_features_error_overall[l]) / (double) sqrt(aoa_lesion_distribution);
        //         aoa_graph->se_functional_features_error_overall[l] = sqrt(aoa_graph->se_functional_features_error_overall[l]) / (double) sqrt(aoa_lesion_distribution);


    }





}



void gtk_aoa_graph_update_networks(GtkAoA_Graph *aoa_graph)
{
    //   aoa_lesion_level = 4;

    //     static init_called = 0;
    //     if (!init_called) {
    //       init_called = 1;
    //     }
    //  sample_mean_shared_features_error_overall[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL], sample_mean_error_overall[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL], sample_mean_functional_features_error_overall[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL],
    char filename[256];


//     double sample_mean_shared_features_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN], sample_mean_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN], sample_mean_functional_features_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN], sample_mean_percent_correct[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN],  sample_mean_percent_incorrect[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][N_DOMAIN][2];


    // if (aoa_graph->type == 0) {
    int s, p, l, d, domain, n; // p is target, q is network's response
    for (l = 0; l < AOA_LESION_LEVEL; l++) {


        //         aoa_graph->se_shared_features_error_overall[l] = 0.0;
        //         aoa_graph->se_error_overall[l] = 0.0;
        //         aoa_graph->se_functional_features_error_overall[l] = 0.0;



        for (domain = 0; domain < N_DOMAIN; domain++) {
            for (d = 0; d < 2; d++) {
                aoa_graph->percent_incorrect[l][domain][d] = 0.0; // between/within per domain
                point_x_incorrect[l][domain][d] = 0.0;
                point_y_percent_incorrect[l][domain][d] = 0.0;
            }
            point_y_error[l][domain] = 0.0;
//             point_y_shared_features_error[l][domain] = 0.0;
//             point_y_features_living_error[l][domain] = 0.0;
//             point_y_functional_features_error[l][domain] = 0.0;


            aoa_graph->error[l][domain] = 0.0;
//             aoa_graph->error[l][domain] = 0.0;
//             aoa_graph->functional_features_error[l][domain] = 0.0;


            aoa_graph->se_error[l][domain] = 0.0;
//             aoa_graph->se_error[l][domain] = 0.0;
//             aoa_graph->se_functional_features_error[l][domain] = 0.0;
            aoa_graph->se_percent_correct[l][domain] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][0] = 0.0;
            aoa_graph->se_percent_incorrect[l][domain][1] = 0.0;
            point_x[l][domain] = 0.0;

            aoa_graph->percent_correct[l][domain] = 0.0;


        }
        for (d = 0; d < AOA_LESION_DISTRIBUTION; d++) {
            //             sample_mean_shared_features_error_overall[d][l] = 0.0;
            //             sample_mean_error_overall[d][l] = 0.0;
            //             sample_mean_functional_features_error_overall[d][l] = 0.0;

            for (domain = 0; domain < N_DOMAIN; domain++) {

                sample_mean_error[d][l][domain] = 0.0;
//                 sample_mean_error[d][l][domain] = 0.0;
//                 sample_mean_functional_features_error[d][l][domain] = 0.0;
                sample_mean_percent_correct[d][l][domain] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][0] = 0.0;
                sample_mean_percent_incorrect[d][l][domain][1] = 0.0;
            }
        }

    }



    for (n = 0; n < aoa_graph->networks; n++) {


        sprintf(filename, "%s%d.aoa_task", aoa_graph->auto_name, n);
        if (file_exists(filename)) {
//             printf("%s\n", filename);

            open_aoa_responses(filename);
            //       set_word_to_picture_combo_box_lesion_levels();
        }
        else
        {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, filename, "(set automated_training flag?)");

        }



        for (d = 0; d < aoa_lesion_distribution; d++) {
            for (l = 0; l < aoa_lesion_level; l++) {
                for (p = 0; p < P; p++) {
//                     if (!excluded_pattern(p)) {
                    for (s = 0; s < aoa_sample; s++) {
                        s= 0;

//                         aoa_graph->error[l][get_aoa_domain(p)] += aoa_distinctive_error[d][l][s][p];
//                         sample_mean_error[d][l][get_aoa_domain(p)] += aoa_distinctive_error[d][l][s][p];

                        aoa_graph->error[l][get_aoa_domain(p)] += aoa_error[d][l][s][p];
                        sample_mean_error[d][l][get_aoa_domain(p)] += aoa_error[d][l][s][p];

//                         aoa_graph->functional_features_error[l][get_aoa_domain(p)] += aoa_functional_error[d][l][s][p];
//                         sample_mean_functional_features_error[d][l][get_aoa_domain(p)] += aoa_functional_error[d][l][s][p];


                        if (aoa_response[d][l][s][p] == p) {
                            (aoa_graph->percent_correct[l][get_aoa_domain(p)])++;
                            (sample_mean_percent_correct[d][l][get_aoa_domain(p)])++;
                            //                             printf("percent_correct[%i][%i] = %f\n", l,get_aoa_domain(p), aoa_graph->percent_correct[l][get_aoa_domain(p)]);
                        }
                        else if (get_aoa_domain(aoa_response[d][l][s][p]) == get_aoa_domain(p)) { //responce and target pattern are in the same domain
                            (aoa_graph->percent_incorrect[l][get_aoa_domain(p)][0])++;
                            (sample_mean_percent_incorrect[d][l][get_aoa_domain(p)][0])++;
                        }
                        else {
                            (aoa_graph->percent_incorrect[l][get_aoa_domain(p)][1])++;
                            (sample_mean_percent_incorrect[d][l][get_aoa_domain(p)][1])++;
                        }
                    }

                }
            }
//             }
        }
    }


    for (l = 0; l < aoa_lesion_level; l++) {
        //         aoa_graph->error_overall[l] /= (double) (aoa_lesion_distribution * aoa_sample * n_patterns);
        // 	aoa_graph->shared_features_error_overall[l] /= (double) (aoa_lesion_distribution * aoa_sample * n_patterns);
        // 	aoa_graph->functional_features_error_overall[l] /= (double) (aoa_lesion_distribution * aoa_sample * n_patterns);
        for (domain = 0; domain < N_DOMAIN; domain++) {

            aoa_graph->error[l][domain] /= (double) (aoa_graph->networks * aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
//             aoa_graph->shared_features_error[l][domain] /= (double) (aoa_graph->networks* aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
//             aoa_graph->functional_features_error[l][domain] /= (double) (aoa_graph->networks* aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->percent_correct[l][domain] /= (double) (aoa_graph->networks * aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->percent_correct[l][domain] *= 100.0;
            aoa_graph->percent_incorrect[l][domain][0] /= (double) (aoa_graph->networks * aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->percent_incorrect[l][domain][1] /= (double) (aoa_graph->networks * aoa_lesion_distribution * aoa_sample * n_pattern[domain]);
            aoa_graph->percent_incorrect[l][domain][0] *= 100.0;
            aoa_graph->percent_incorrect[l][domain][1] *= 100.0;
            printf("aoa_graph->percent_incorrect[%d][%d][%d] = %f\n", l, domain, 1, aoa_graph->percent_incorrect[l][domain][1]);


        }

        //this is going to have to be [d][l][domain]
        for (d = 0; d < aoa_lesion_distribution; d++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {
//                 sample_mean_error[d][l][domain] /= (double) (aoa_graph->networks * aoa_sample * n_pattern[domain]);
                sample_mean_error[d][l][domain] /= (double) (aoa_graph->networks * aoa_sample * n_pattern[domain]);
                sample_mean_error[d][l][domain]  *=  100.0;
//                 sample_mean_functional_features_error[d][l][domain] /= (double) (aoa_graph->networks* aoa_sample * n_pattern[domain]);

                sample_mean_percent_correct[d][l][domain] /=  (double) (aoa_graph->networks* aoa_sample * n_pattern[domain]);
                sample_mean_percent_correct[d][l][domain] *=  100.0;

                sample_mean_percent_incorrect[d][l][domain][0] /=  (double) (aoa_graph->networks* aoa_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][0] *= 100.0;

                sample_mean_percent_incorrect[d][l][domain][1] /=  (double) (aoa_graph->networks * aoa_sample * n_pattern[domain]);
                sample_mean_percent_incorrect[d][l][domain][1] *= 100.0;


            }


            //             sample_mean_error_overall[d][l] /= (double) (aoa_sample * n_patterns);
            //             sample_mean_shared_features_error_overall[d][l] /= (double) (aoa_sample * n_patterns);
            //             sample_mean_functional_features_error_overall[d][l] /= (double) (aoa_sample * n_patterns);
        }
    }

    for (d = 0; d < aoa_lesion_distribution; d++) {
        for (l = 0; l < aoa_lesion_level; l++) {
            //             aoa_graph->se_error_overall[l] += sqr(sample_mean_error_overall[d][l] - aoa_graph->error_overall[l]);
            //             aoa_graph->se_shared_features_error_overall[l] += sqr(sample_mean_shared_features_error_overall[d][l] - aoa_graph->shared_features_error_overall[l]);
            //             aoa_graph->se_functional_features_error_overall[l] += sqr(sample_mean_functional_features_error_overall[d][l] - aoa_graph->functional_features_error_overall[l]);

            for (domain = 0; domain < N_DOMAIN; domain++) {
//                 aoa_graph->se_error[l][domain] += sqr(sample_mean_error[d][l][domain] - aoa_graph->error[l][domain]);
                aoa_graph->se_error[l][domain] += sqr(sample_mean_error[d][l][domain] - aoa_graph->error[l][domain]);
//                 aoa_graph->se_functional_features_error[l][domain] += sqr(sample_mean_functional_features_error[d][l][domain] - aoa_graph->functional_features_error[l][domain]);
                aoa_graph->se_percent_correct[l][domain] += sqr(sample_mean_percent_correct[d][l][domain] - aoa_graph->percent_correct[l][domain]);
                aoa_graph->se_percent_incorrect[l][domain][0] += sqr(sample_mean_percent_incorrect[d][l][domain][0] - aoa_graph->percent_incorrect[l][domain][0]);
                aoa_graph->se_percent_incorrect[l][domain][1] += sqr(sample_mean_percent_incorrect[d][l][domain][1] - aoa_graph->percent_incorrect[l][domain][1]);
            }
        }

    }

    for (l = 0; l < aoa_lesion_level; l++) {
        //         aoa_graph->se_error_overall[l] /= (double) aoa_lesion_distribution;
        //         aoa_graph->se_shared_features_error_overall[l] /= (double) aoa_lesion_distribution;
        //         aoa_graph->se_functional_features_error_overall[l] /= (double) aoa_lesion_distribution;

        for (domain = 0; domain < N_DOMAIN; domain++) {
//             aoa_graph->se_error[l][domain] /= (double) aoa_graph->networks * aoa_lesion_distribution;
            aoa_graph->se_error[l][domain] /= (double)aoa_graph->networks * aoa_lesion_distribution;
//             aoa_graph->se_functional_features_error[l][domain] /= (double)aoa_graph->networks* aoa_lesion_distribution;
            aoa_graph->se_percent_correct[l][domain] /= (double)aoa_graph->networks * aoa_lesion_distribution;
            aoa_graph->se_percent_incorrect[l][domain][0] /= (double)aoa_graph->networks* aoa_lesion_distribution;
            aoa_graph->se_percent_incorrect[l][domain][1] /= (double) aoa_graph->networks* aoa_lesion_distribution;
            aoa_graph->se_error[l][domain] = sqrt(aoa_graph->se_error[l][domain]) / (double) sqrt(aoa_graph->networks * aoa_lesion_distribution);
//             aoa_graph->se_shared_features_error[l][domain] = sqrt(aoa_graph->se_shared_features_error[l][domain]) / (double) sqrt(aoa_graph->networks * aoa_lesion_distribution);
//             aoa_graph->se_functional_features_error[l][domain] = sqrt(aoa_graph->se_functional_features_error[l][domain]) / (double) sqrt(aoa_graph->networks * aoa_lesion_distribution);
            aoa_graph->se_percent_correct[l][domain] = sqrt(aoa_graph->se_percent_correct[l][domain]);
            aoa_graph->se_percent_incorrect[l][domain][0] = sqrt(aoa_graph->se_percent_incorrect[l][domain][0]);
            aoa_graph->se_percent_incorrect[l][domain][1] = sqrt(aoa_graph->se_percent_incorrect[l][domain][1]);
        }

        //         aoa_graph->se_error_overall[l] = sqrt(aoa_graph->se_error_overall[l]) / (double) sqrt(aoa_lesion_distribution);
        //         aoa_graph->se_shared_features_error_overall[l] = sqrt(aoa_graph->se_shared_features_error_overall[l]) / (double) sqrt(aoa_lesion_distribution);
        //         aoa_graph->se_functional_features_error_overall[l] = sqrt(aoa_graph->se_functional_features_error_overall[l]) / (double) sqrt(aoa_lesion_distribution);


    }





}
void gtk_aoa_graph_update_networks_accumulate(GtkAoA_Graph *aoa_graph, int n)
{

    int s, p, l, d;


    printf("%s: network number = %d of %d\n", __FUNCTION__, n, aoa_graph->networks);



    for (d = 0; d < aoa_lesion_distribution; d++) {
        for (l = 0; l < aoa_lesion_level; l++) {
            for (p = 0; p < P; p++) {
//                 if (!excluded_pattern(p)) {
                for (s = 0; s < aoa_sample; s++) {


                    aoa_graph->error[l][get_aoa_domain(p)] += aoa_error[d][l][s][p];
//                         sample_mean_error[d][l][get_aoa_domain(p)] += aoa_distinctive_error[d][l][s][p];
//                     network_mean_error[n][l][get_aoa_domain(p)] += aoa_distinctive_error[d][l][s][p];


//                     aoa_graph->shared_features_error[l][get_aoa_domain(p)] += aoa_shared_error[d][l][s][p];
// 			sample_mean_shared_features_error[d][l][get_aoa_domain(p)] += aoa_shared_error[d][l][s][p];
//                     network_mean_shared_features_error[n][l][get_aoa_domain(p)] += aoa_shared_error[d][l][s][p];

//                     aoa_graph->functional_features_error[l][get_aoa_domain(p)] += aoa_functional_error[d][l][s][p];
// 			sample_mean_functional_features_error[d][l][get_aoa_domain(p)] += aoa_functional_error[d][l][s][p];
//                     network_mean_functional_features_error[n][l][get_aoa_domain(p)] += aoa_functional_error[d][l][s][p];


                    if (aoa_response[d][l][s][p] == p) {
                        (aoa_graph->percent_correct[l][get_aoa_domain(p)])++;
// 			    (sample_mean_percent_correct[d][l][get_aoa_domain(p)])++;
                        (network_mean_percent_correct[n][l][get_aoa_domain(p)])++;
                    }
                    else if (get_aoa_domain(aoa_response[d][l][s][p]) == get_aoa_domain(p)) { //responce and target pattern are in the same domain
                        (aoa_graph->percent_incorrect[l][get_aoa_domain(p)][0])++;
// 			    (sample_mean_percent_incorrect[d][l][get_aoa_domain(p)][0])++;
                        (network_mean_percent_incorrect[n][l][get_aoa_domain(p)][0])++;
                    }
                    else {
                        (aoa_graph->percent_incorrect[l][get_aoa_domain(p)][1])++;
// 			    (sample_mean_percent_incorrect[d][l][get_aoa_domain(p)][1])++;
                        (network_mean_percent_incorrect[n][l][get_aoa_domain(p)][1])++;
                    }
                }

            }
//             }
        }
    }




}

void gtk_aoa_graph_update_networks_final(GtkAoA_Graph *aoa_graph)
{
    int l, n, domain, d;


    for (n = 0; n <(aoa_graph->networks); n++) {

        printf("%s: network number = %d of %d\n", __FUNCTION__, n, aoa_graph->networks);

        for (l = 0; l < aoa_lesion_level; l++) {

            for (domain = 0; domain < N_DOMAIN; domain++) {
                network_mean_error[n][l][domain] /= (double) (aoa_lesion_distribution * n_pattern[domain]);
//                 network_mean_shared_features_error[n][l][domain] /= (double) (aoa_lesion_distribution * n_pattern[domain]);
//                 network_mean_functional_features_error[n][l][domain] /= (double) (aoa_lesion_distribution * n_pattern[domain]);

                network_mean_percent_correct[n][l][domain] /=  (double) (aoa_lesion_distribution * n_pattern[domain]);
                network_mean_percent_correct[n][l][domain] *=  100.0;
                network_mean_percent_incorrect[n][l][domain][0] /=  (double) (aoa_lesion_distribution * n_pattern[domain]);
                network_mean_percent_incorrect[n][l][domain][0] *= 100.0;
                network_mean_percent_incorrect[n][l][domain][1] /=  (double) (aoa_lesion_distribution * n_pattern[domain]);
                network_mean_percent_incorrect[n][l][domain][1] *= 100.0;
            }

        }
    }



    // These are the global means
    for (l = 0; l < aoa_lesion_level; l++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {

            aoa_graph->error[l][domain] /= (double) (aoa_graph->networks * aoa_lesion_distribution * n_pattern[domain]);
//             aoa_graph->shared_features_error[l][domain] /= (double) (aoa_graph->networks * aoa_lesion_distribution * n_pattern[domain]);
//             aoa_graph->functional_features_error[l][domain] /= (double) (aoa_graph->networks * aoa_lesion_distribution * n_pattern[domain]);
            aoa_graph->percent_correct[l][domain] /= (double) (aoa_graph->networks * aoa_lesion_distribution * n_pattern[domain]);
            aoa_graph->percent_correct[l][domain] *= 100.0;

            for (d = 0; d < 2; d++) {

// 	      printf("aoa_graph->percent_incorrect[%d][%d][%d]=%f\n", l, domain, d, aoa_graph->percent_incorrect[l][domain][d]);

                aoa_graph->percent_incorrect[l][domain][d] /= (double) (aoa_graph->networks * aoa_lesion_distribution * n_pattern[domain]);
                aoa_graph->percent_incorrect[l][domain][d] *= 100.0;
// 	    printf("aoa_graph->percent_incorrect[%d][%d][%d]=%f\n", l, domain, d, aoa_graph->percent_incorrect[l][domain][d]);
// 	    printf("aoa_graph->networks = %d, aoa_lesion_distribution = %d, n_pattern[%d] = %d\n",aoa_graph->networks, aoa_lesion_distribution, domain,n_pattern[domain] );
            }


        }
    }
//         //this is going to have to be [d][l][domain]
//         for (d = 0; d < AOA_LESION_DISTRIBUTION; d++) {
//             for (domain = 0; domain < N_DOMAIN; domain++) {
//                 sample_mean_error[d][l][domain] /= (double) (aoa_graph->networks * AOA_SAMPLE * n_pattern[domain]);
//                 sample_mean_shared_features_error[d][l][domain] /= (double) (aoa_graph->networks * AOA_SAMPLE * n_pattern[domain]);
//                 sample_mean_functional_features_error[d][l][domain] /= (double) (aoa_graph->networks * AOA_SAMPLE * n_pattern[domain]);
//
//                 sample_mean_percent_correct[d][l][domain] /=  (double) (aoa_graph->networks * AOA_SAMPLE * n_pattern[domain]);
//                 sample_mean_percent_correct[d][l][domain] *=  100.0;
//                 sample_mean_percent_incorrect[d][l][domain][0] /=  (double) (aoa_graph->networks * AOA_SAMPLE * n_pattern[domain]);
//                 sample_mean_percent_incorrect[d][l][domain][0] *= 100.0;
//                 sample_mean_percent_incorrect[d][l][domain][1] /=  (double) (aoa_graph->networks * AOA_SAMPLE * n_pattern[domain]);
//                 sample_mean_percent_incorrect[d][l][domain][1] *= 100.0;
//
//
//             }
//
//         }

    for (l = 0; l < aoa_lesion_level; l++) {

        //sum squared difference of sample and mean
        for (n= 0; n < aoa_graph->networks; n++) {
            for (l = 0; l < aoa_lesion_level; l++) {
                for (domain = 0; domain < N_DOMAIN; domain++) {
//                     aoa_graph->se_error[l][domain] += sqr(network_mean_error[n][l][domain] - aoa_graph->error[l][domain]);
                    aoa_graph->se_error[l][domain] += sqr(network_mean_error[n][l][domain] - aoa_graph->error[l][domain]);
//                     aoa_graph->se_functional_features_error[l][domain] += sqr(network_mean_functional_features_error[n][l][domain] - aoa_graph->functional_features_error[l][domain]);
                    aoa_graph->se_percent_correct[l][domain] += sqr(network_mean_percent_correct[n][l][domain] - aoa_graph->percent_correct[l][domain]);
                    aoa_graph->se_percent_incorrect[l][domain][0] += sqr(network_mean_percent_incorrect[n][l][domain][0] - aoa_graph->percent_incorrect[l][domain][0]);
                    aoa_graph->se_percent_incorrect[l][domain][1] += sqr(network_mean_percent_incorrect[n][l][domain][1] - aoa_graph->percent_incorrect[l][domain][1]);
                }
            }

        }
        // rest of calculation for SE
        for (l = 0; l < aoa_lesion_level; l++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {



                aoa_graph->se_error[l][domain] = sqrt(aoa_graph->se_error[l][domain] / (double) (aoa_graph->networks - 1.0)) / (double) sqrt((double) aoa_graph->networks);
//                 aoa_graph->se_shared_features_error[l][domain] = sqrt(aoa_graph->se_shared_features_error[l][domain] / (double) (aoa_graph->networks - 1.0)) / (double) sqrt((double) aoa_graph->networks);
//                 aoa_graph->se_functional_features_error[l][domain] =  sqrt(aoa_graph->se_functional_features_error[l][domain]/ (double) (aoa_graph->networks - 1.0)) / (double) sqrt((double) aoa_graph->networks);
                aoa_graph->se_percent_correct[l][domain]  = sqrt(aoa_graph->se_percent_correct[l][domain] / (double) (aoa_graph->networks - 1.0)) / (double) sqrt((double) aoa_graph->networks);
                aoa_graph->se_percent_incorrect[l][domain][0] = sqrt(aoa_graph->se_percent_incorrect[l][domain][0] / (double) (aoa_graph->networks - 1.0)) / (double) sqrt((double) aoa_graph->networks);
                aoa_graph->se_percent_incorrect[l][domain][1] = sqrt( aoa_graph->se_percent_incorrect[l][domain][1]/ (double) (aoa_graph->networks - 1.0)) / (double) sqrt((double) aoa_graph->networks);

            }



        }

    }
}


static void draw_aoa_subgraph(cairo_t *cr, GtkAoA_Graph *aoa_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p, int subgraph)
{
    int i, domain, d;
    double y_max = 0.0;

    for (i = 0; i < aoa_lesion_level; i++) {

        if (aoa_graph->type == ERROR_BY_DOMAIN) {

            for (domain = 0; domain < N_DOMAIN; domain++) {

//                 y_max = fmax(y_max, (aoa_graph->error[i][domain] + aoa_graph->se_error[i][domain]));
//                 y_max = fmax(y_max, (aoa_graph->error[i][0] + aoa_graph->se_error[i][0]));
                y_max = fmax(y_max, (aoa_graph->error[i][domain] + aoa_graph->se_error[i][domain]));
//                 y_max = fmax(y_max, (aoa_graph->shared_features_error[i][domain] + aoa_graph->se_shared_features_error[i][domain]));
//                 y_max = fmax(y_max, (aoa_graph->functional_features_error[i][domain] + aoa_graph->se_functional_features_error[i][domain]));
            }
            y_max = ceilf(y_max * 10.0) / 10.0;



        } else if ((aoa_graph->type == PERCENT_PATTERNS_CORRECT) ||  (aoa_graph->type == PERCENT_PATTERNS_INCORRECT)) {

            y_max = 100.0;
// 	for (domain = 0; domain < N_DOMAIN; domain++) {
//
//             y_max = fmax(y_max, (aoa_graph->percent_correct[i][domain] + aoa_graph->se_percent_correct[i][domain]));
// 	    y_max = fmax(y_max, (aoa_graph->percent_incorrect[i][domain][0] + aoa_graph->se_percent_incorrect[i][domain][0]));
// 	    y_max = fmax(y_max, (aoa_graph->percent_incorrect[i][domain][1] + aoa_graph->se_percent_incorrect[i][domain][1]));
// 	}
// 	    y_max = ceilf(y_max / 10.0) * 10.0;

        }
    }

//     printf("y_max = %f\n", y_max);
    x_width = width - (x_offset * 2);
    y_height = height - (y_offset * 2);

    x_segments = (double) aoa_lesion_level;

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

    if (aoa_graph->type == ERROR_BY_DOMAIN) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * aoa_graph->error[0][domain] / y_max + y_axis);
            for (i = 1; i < aoa_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * aoa_graph->error[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            if (error_bars) {

                for (i = 0; i < aoa_lesion_level; i++) {


//                     printf("SE  = %f\n", aoa_graph->se_error[i][domain]);

                    cairo_move_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->error[i][domain] - aoa_graph->se_error[i][domain]) / y_max  + y_axis);
                    cairo_line_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->error[i][domain] + aoa_graph->se_error[i][domain]) / y_max + y_axis);

                    cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->error[i][domain] - aoa_graph->se_error[i][domain]) / y_max  + y_axis);
                    cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->error[i][domain] - aoa_graph->se_error[i][domain]) / y_max  + y_axis);

                    cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->error[i][domain] + aoa_graph->se_error[i][domain]) / y_max  + y_axis);
                    cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->error[i][domain] + aoa_graph->se_error[i][domain]) / y_max  + y_axis);
                }
                cairo_stroke(cr);

            }

            for (i = 0; i < aoa_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * aoa_graph->error[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_error[i][domain] = -y_height * aoa_graph->error[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }

    } /*else if (aoa_graph->type == FEATURES_LIVING_ERROR) {

        cairo_set_source_rgb(cr, RED);


        cairo_move_to(cr, x_offset + 0 * x_space, -y_height * aoa_graph->shared_features_error[0][0] / y_max + y_axis);
        for (i = 1; i < aoa_lesion_level; i++) {
            cairo_line_to(cr, x_offset + i * x_space, -y_height * aoa_graph->shared_features_error[i][0]/ y_max + y_axis);

        }
        cairo_stroke(cr);

        for (i = 0; i < aoa_lesion_level; i++) {

//             printf("SE  = %f\n", aoa_graph->shared_features_error[i][0]);
            cairo_move_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->shared_features_error[i][0] - aoa_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->shared_features_error[i][0] + aoa_graph->se_shared_features_error[i][0]) / y_max + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->shared_features_error[i][0] - aoa_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->shared_features_error [i][0] - aoa_graph->se_shared_features_error[i][0]) / y_max  + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->shared_features_error [i][0] + aoa_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->shared_features_error [i][0] + aoa_graph->se_shared_features_error[i][0]) / y_max  + y_axis);
        }
        cairo_stroke(cr);

        for (i = 0; i < aoa_lesion_level; i++) {

            cairo_arc(cr, x_offset + i * x_space, -y_height * aoa_graph->shared_features_error[i][0]  / y_max + y_axis, radius, 0.0, 2*M_PI);
            point_x[i][0] = x_offset + i * x_space;

            //         }

            point_y_features_living_error[i][0] = -y_height * aoa_graph->shared_features_error[i][0] / y_max + y_axis;

            cairo_fill(cr);

        }

        cairo_set_source_rgb(cr, BLUE);

        cairo_move_to(cr, x_offset + 0 * x_space, -y_height * aoa_graph->error[0][0] / y_max + y_axis);
        for (i = 1; i < aoa_lesion_level; i++) {
            cairo_line_to(cr, x_offset + i * x_space, -y_height * aoa_graph->error[i][0]/ y_max + y_axis);

        }
        cairo_stroke(cr);

        for (i = 0; i < aoa_lesion_level; i++) {

//             printf("SE  = %f\n", aoa_graph->error[i][0]);

            cairo_move_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->error[i][0] - aoa_graph->se_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->error[i][0] + aoa_graph->se_error[i][0]) / y_max + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->error[i][0] - aoa_graph->se_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->error[i][0] - aoa_graph->se_error[i][0]) / y_max  + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->error[i][0] + aoa_graph->se_error[i][0]) / y_max  + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->error[i][0] + aoa_graph->se_error[i][0]) / y_max  + y_axis);
        }
        cairo_stroke(cr);

        for (i = 0; i < aoa_lesion_level; i++) {

            cairo_arc(cr, x_offset + i * x_space, -y_height * aoa_graph->error[i][0]  / y_max + y_axis, radius, 0.0, 2*M_PI);
            point_x[i][1] = x_offset + i * x_space;

            //         }

            point_y_features_living_error[i][1] = -y_height * aoa_graph->error[i][0] / y_max + y_axis;

            cairo_fill(cr);

        }

    } else if (aoa_graph->type == SHARED_FEATURES_ERROR) {
        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * aoa_graph->shared_features_error[0][domain] / y_max + y_axis);
            for (i = 1; i < aoa_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * aoa_graph->shared_features_error[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            for (i = 0; i < aoa_lesion_level; i++) {
//                 printf("SE  = %f\n", aoa_graph->shared_features_error[i][domain]);


                cairo_move_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->shared_features_error[i][domain] - aoa_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->shared_features_error[i][domain] + aoa_graph->se_shared_features_error[i][domain]) / y_max + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->shared_features_error[i][domain] - aoa_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->shared_features_error[i][domain] - aoa_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->shared_features_error[i][domain] + aoa_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->shared_features_error[i][domain] + aoa_graph->se_shared_features_error[i][domain]) / y_max  + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < aoa_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * aoa_graph->shared_features_error[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_shared_features_error[i][domain] = -y_height * aoa_graph->shared_features_error[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }
    }

    else if (aoa_graph->type == FUNCTIONAL_FEATURES_ERROR) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * aoa_graph->functional_features_error[0][domain] / y_max + y_axis);
            for (i = 1; i < aoa_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * aoa_graph->functional_features_error[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            for (i = 0; i < aoa_lesion_level; i++) {
//                 printf("SE  = %f\n", aoa_graph->functional_features_error[i][domain]);


                cairo_move_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->functional_features_error[i][domain] - aoa_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->functional_features_error[i][domain] + aoa_graph->se_functional_features_error[i][domain]) / y_max + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->functional_features_error[i][domain] - aoa_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->functional_features_error[i][domain] - aoa_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->functional_features_error[i][domain] + aoa_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->functional_features_error[i][domain] + aoa_graph->se_functional_features_error[i][domain]) / y_max  + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < aoa_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * aoa_graph->functional_features_error[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_functional_features_error[i][domain] = -y_height * aoa_graph->functional_features_error[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }
    }
                    */
    else if (aoa_graph->type == PERCENT_PATTERNS_CORRECT) {

        for(domain = 0; domain < N_DOMAIN; domain++) {

            if (domain == 0) {
                cairo_set_source_rgb(cr, PURPLE);
            }
            else  {
                cairo_set_source_rgb(cr, GREEN);

            }

            cairo_move_to(cr, x_offset + 0 * x_space, -y_height * aoa_graph->percent_correct[0][domain] / y_max + y_axis);
            for (i = 1; i < aoa_lesion_level; i++) {
                cairo_line_to(cr, x_offset + i * x_space, -y_height * aoa_graph->percent_correct[i][domain]/ y_max + y_axis);

            }
            cairo_stroke(cr);

            for (i = 0; i < aoa_lesion_level; i++) {

//                 printf("SE  = %f\n", aoa_graph->percent_correct[i][domain]);

                cairo_move_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->percent_correct[i][domain] - aoa_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->percent_correct[i][domain] + aoa_graph->se_percent_correct[i][domain]) / y_max + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->percent_correct[i][domain] - aoa_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->percent_correct[i][domain] - aoa_graph->se_percent_correct[i][domain]) / y_max  + y_axis);

                cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->percent_correct[i][domain] + aoa_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
                cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->percent_correct[i][domain] + aoa_graph->se_percent_correct[i][domain]) / y_max  + y_axis);
            }
            cairo_stroke(cr);

            for (i = 0; i < aoa_lesion_level; i++) {

                cairo_arc(cr, x_offset + i * x_space, -y_height * aoa_graph->percent_correct[i][domain] / y_max + y_axis, radius, 0.0, 2*M_PI);
                point_x[i][domain] = x_offset + i * x_space;

                //         }

                point_y_percent_correct[i][domain] = -y_height * aoa_graph->percent_correct[i][domain] / y_max + y_axis;

                cairo_fill(cr);

            }
        }


    }
    else if (aoa_graph->type == PERCENT_PATTERNS_INCORRECT) {

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

                cairo_move_to(cr, x_offset + 0 * x_space, -y_height * aoa_graph->percent_incorrect[0][domain][d] / y_max + y_axis);
                for (i = 1; i < aoa_lesion_level; i++) {
                    cairo_line_to(cr, x_offset + i * x_space, -y_height * aoa_graph->percent_incorrect[i][domain][d]/ y_max + y_axis);


                }

                cairo_stroke(cr);

                cairo_set_dash (cr, dash, 0, 0);

                if (error_bars) {
                    for (i = 0; i < aoa_lesion_level; i++) {


//                         printf("SE  = %f\n", aoa_graph->percent_incorrect[i][domain][d]);

                        cairo_move_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->percent_incorrect[i][domain][d] - aoa_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                        cairo_line_to(cr, x_offset + i * x_space, -y_height * (aoa_graph->percent_incorrect[i][domain][d] + aoa_graph->se_percent_incorrect[i][domain][d]) / y_max + y_axis);

                        cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->percent_incorrect[i][domain][d] - aoa_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                        cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->percent_incorrect[i][domain][d] - aoa_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);

                        cairo_move_to(cr, x_offset + i * x_space + 4, -y_height * (aoa_graph->percent_incorrect[i][domain][d] + aoa_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                        cairo_line_to(cr, x_offset + i * x_space - 4, -y_height * (aoa_graph->percent_incorrect[i][domain][d] + aoa_graph->se_percent_incorrect[i][domain][d]) / y_max  + y_axis);
                    }
                    cairo_stroke(cr);
                }

                for (i = 0; i < aoa_lesion_level; i++) {

                    cairo_arc(cr, x_offset + i * x_space, -y_height * aoa_graph->percent_incorrect[i][domain][d] / y_max + y_axis, radius, 0.0, 2*M_PI);
                    point_x_incorrect[i][domain][d] = x_offset + i * x_space;

                    //         }

                    point_y_percent_incorrect[i][domain][d] = -y_height * aoa_graph->percent_incorrect[i][domain][d] / y_max + y_axis;

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

        if ((aoa_graph->type == PERCENT_PATTERNS_CORRECT) || (aoa_graph->type == PERCENT_PATTERNS_INCORRECT)) {
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
        // 	  printf("i = %i\n", AOA_LESION_LEVEL);
        cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
        cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
        g_snprintf(buffer, 64, "%1.1f", i * 100.0 / (double) aoa_lesion_level);
        cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
    }


}



static void draw_aoa_legend(cairo_t *cr, GtkAoA_Graph *aoa_graph, PangoLayout *layout, CairoxTextParameters p)
{
    // 	//


    pangox_layout_set_font_size(layout, 13);
    double text_width = 0.0;
    pangox_layout_select_font_from_string(layout, "Times");

    //draw legend
    cairo_translate(cr, 2*x_offset, 0);

    //
    if (aoa_graph->type == ERROR_BY_DOMAIN) {

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

    } else if (aoa_graph->type == FEATURES_LIVING_ERROR) {


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

    }     else if (aoa_graph->type == PERCENT_PATTERNS_INCORRECT) {


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

static void draw_aoa_axes_labels(cairo_t *cr, GtkAoA_Graph *aoa_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p)
{

//     if (aoa_graph->type == 0)
//     {
    g_snprintf(buffer, 64, "%s", y_label[aoa_graph->type]);
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

static void draw_aoa_graph(cairo_t *cr, GtkWidget *widget, double width, double height)
{


//     double width = (double) widget->allocation.width, height = (double) widget->allocation.height;
    GtkAoA_Graph *aoa_graph =  GTK_AOA_GRAPH(widget);
    PangoLayout *layout = pango_cairo_create_layout(cr);
    CairoxTextParameters p;


    x_offset = 50.0;
    y_offset =  40.0;


    draw_aoa_legend(cr, GTK_AOA_GRAPH(widget), layout, p);

    draw_aoa_axes_labels(cr, aoa_graph, width, height, layout, p);


    if (aoa_graph->data_type == MODEL) {
        draw_aoa_subgraph(cr, aoa_graph, width, height, layout, p, -1);
    }
//     else if (aoa_graph->data_type == AOA) {
//         draw_aoa_subgraph_original_model(cr, aoa_graph, width, height, layout, p, -1);
//
//     }

//     }
//     else
//     {
//         int i;
//         for (i = 0; i < 4; i++) {
//
//             draw_aoa_subgraph(cr, aoa_graph, width, height / 4.0, layout, p, i);
//             cairo_translate(cr, 0, height / 4.0);
//         }
//
//     }
    g_object_unref(layout);
    cairo_destroy(cr);

}
void gtk_aoa_graph_pdf(GtkWidget *widget, char *filename)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = 800;//widget->allocation.width;
    int height = 500;//widget->allocation.height;
    char name[100];
    int i = 0;
//     GtkAoA_Graph *aoa_graph = GTK_AOA_GRAPH(widget);

    while (filename[i] != '\0') {
        name[i] = filename[i];
        i++;
    }
    name[i] = '\0';
//     printf("%s\n", name);
    sprintf(name, "%s_aoa_task_%d.pdf", name, GTK_AOA_GRAPH(widget)->type);



    surface = cairo_pdf_surface_create(name, width, height);
    cr = cairo_create(surface);
    // draw aoa_graph
    draw_aoa_graph(cr, widget, width, height);


    cairo_surface_destroy(surface);

}


static void gtk_aoa_graph_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    draw_aoa_graph(cr, widget, (double) widget->allocation.width, (double) widget->allocation.height);
}

void gtk_aoa_graph_reset(GtkAoA_Graph *aoa_graph)
{
    gtk_aoa_graph_init(aoa_graph);
    gtk_aoa_graph_redraw(GTK_WIDGET(aoa_graph));


}

static void gtk_aoa_graph_destroy(GtkObject *object)
{
//     GtkAoA_Graph *aoa_graph;
    GtkAoA_GraphClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_AOA_GRAPH(object));

//     aoa_graph = GTK_AOA_GRAPH(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_aoa_graph_redraw(GtkWidget *widget)
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

void gtk_aoa_graph_clear(GtkWidget *widget)
{
    init_aoa_response(0, 0, 0);
    gtk_aoa_graph_reset(GTK_AOA_GRAPH(widget));

}

