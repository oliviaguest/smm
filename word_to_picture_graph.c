#include "word_to_picture_graph.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);

static void gtk_word_to_picture_graph_class_init(GtkWord_to_picture_GraphClass *klass);
GtkType gtk_word_to_picture_graph_get_type(void);
static void gtk_word_to_picture_graph_init(GtkWord_to_picture_Graph *word_to_picture_graph);
static void gtk_word_to_picture_graph_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_word_to_picture_graph_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_word_to_picture_graph_realize(GtkWidget *widget);
static gboolean gtk_word_to_picture_graph_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_word_to_picture_graph_paint(GtkWidget *widget);
static void gtk_word_to_picture_graph_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

static char buffer[64];

char foil[3][10]= { "Close", "Distant", "Unrelated"};


static const double superordinate_dash[1] = {5.0};
static const double crossdomain_dash[2] = {1.0, 4.0};
static const double omission_dash[4] = {1.0, 1.0, 5.0, 1.0};

static double radius = 3.0;

double point_y_close[WORD_TO_PICTURE_LESION_LEVEL];
double point_x_close[WORD_TO_PICTURE_LESION_LEVEL];

double point_y_cross_section[3];
double point_x_cross_section[3];
double value_cross_section[3];
double x_offset;
double y_offset;
// int n_pattern[2];

int n_close[N_DOMAIN], n_distant[N_DOMAIN], n_unrelated[N_DOMAIN];

GtkType gtk_word_to_picture_graph_get_type(void)
{
    static GtkType gtk_word_to_picture_graph_type = 0;
    if (!gtk_word_to_picture_graph_type)
    {
        static const GtkTypeInfo gtk_word_to_picture_graph_info = {"GtkWord_to_picture_Graph", sizeof(GtkWord_to_picture_Graph), sizeof(GtkWord_to_picture_GraphClass), (GtkClassInitFunc) gtk_word_to_picture_graph_class_init, (GtkObjectInitFunc) gtk_word_to_picture_graph_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_word_to_picture_graph_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_word_to_picture_graph_info);
    }
    return gtk_word_to_picture_graph_type;
}

GtkWidget * gtk_word_to_picture_graph_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_word_to_picture_graph_get_type()));
}



static void gtk_word_to_picture_graph_class_init(GtkWord_to_picture_GraphClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_word_to_picture_graph_realize;
    widget_class->size_request = gtk_word_to_picture_graph_size_request;
    widget_class->size_allocate = gtk_word_to_picture_graph_size_allocate;
    widget_class->expose_event = gtk_word_to_picture_graph_expose;
    object_class->destroy = gtk_word_to_picture_graph_destroy;
}



static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    double coordx = 0, coordy = 0;
    GtkWord_to_picture_Graph *word_to_picture_graph = GTK_WORD_TO_PICTURE_GRAPH(widget);

    gtk_word_to_picture_graph_redraw(widget);



    if (event->button == 1) {
        coordx = event->x;
        coordy = event->y;
    }


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
    layout = pango_cairo_create_layout(cr);




    for (i = 0; i < word_to_picture_lesion_level; i++) {

        if ((coordx >= (point_x_close[i] - radius*1.5)) && (coordx <= (point_x_close[i] + radius*1.5))) {

            if ((coordy >= (point_y_close[i] - radius*1.5)) && (coordy <= (point_y_close[i] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f, %1.1f%%", (word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]), i * 100.0 / (double) word_to_picture_lesion_level);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x_close[i] + radius*1.5, point_y_close[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }

        }

    }

    for (i = 0; i < 3; i++) {

        if ((coordx >= (point_x_cross_section[i] - radius*1.5)) && (coordx <= (point_x_cross_section[i] + radius*1.5))) {

            if ((coordy >= (point_y_cross_section[i] - radius*1.5)) && (coordy <= (point_y_cross_section[i] + radius*1.5))) {
                g_snprintf (buffer, 64, "%1.2f", value_cross_section[i]);
                pangox_layout_set_font_size(layout, 10.0);
                cairox_text_parameters_set (&text_p, point_x_cross_section[i] + radius*1.5, point_y_cross_section[i] + radius*1.5 - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
                cairox_paint_pango_text (cr, &text_p, layout, buffer);
                break;

            }

        }

    }


    g_object_unref (layout);
    cairo_destroy (cr);

    return TRUE;
}


static void gtk_word_to_picture_graph_init(GtkWord_to_picture_Graph *word_to_picture_graph)
{

    int l, domain;
    word_to_picture_graph->data_type = MODEL;
    word_to_picture_graph->legend = TRUE;
    for (l = 0; l < WORD_TO_PICTURE_LESION_LEVEL; l++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {
            word_to_picture_graph->close[l][domain] = 0.0;
            word_to_picture_graph->distant[l][domain] = 0.0;
            word_to_picture_graph->unrelated[l][domain] = 0.0;
        }

        word_to_picture_graph->se_close[l] = 0.0;
        word_to_picture_graph->se_distant[l] = 0.0;
        word_to_picture_graph->se_unrelated[l] = 0.0;

    }

    word_to_picture_graph->type = 0;
    word_to_picture_graph->current_level = -1;

    get_n_close_trials(&n_close[0]);
    get_n_distant_trials(&n_distant[0]);
    get_n_unrelated_trials(&n_unrelated[0]);
//     if (EXCLUDE_FRUIT) {
//
//         n_pattern[0] = get_domain_n_pattern(0) - get_n_fruit();// animals
//         n_pattern[1] = get_domain_n_pattern(1);
//     }
//     else
//     {
//
//         n_pattern[0] = get_domain_n_pattern(0);
//         n_pattern[1] = get_domain_n_pattern(1);
//     }
//
//     n_pattern[0] -= get_domain_n_general_names(0);
//     n_pattern[1] -= get_domain_n_general_names(1);

    gtk_widget_add_events(GTK_WIDGET(word_to_picture_graph), GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(word_to_picture_graph), "button-press-event", G_CALLBACK(clicked), NULL);

}

void gtk_word_to_picture_graph_set_current_level(GtkWord_to_picture_Graph *word_to_picture_graph, int level)
{
    word_to_picture_graph->current_level = level;

}

void gtk_word_to_picture_graph_set_type(GtkWord_to_picture_Graph *word_to_picture_graph, int type)
{
    word_to_picture_graph->type = type;

}
void gtk_word_to_picture_graph_set_legend(GtkWord_to_picture_Graph *word_to_picture_graph, int legend)
{
    word_to_picture_graph->legend = legend;

}

void gtk_word_to_picture_graph_set_data_type(GtkWord_to_picture_Graph *word_to_picture_graph, int type)
{
    word_to_picture_graph->data_type = type;
    gtk_word_to_picture_graph_update(GTK_WORD_TO_PICTURE_GRAPH(word_to_picture_graph));

    gtk_word_to_picture_graph_redraw(GTK_WIDGET(word_to_picture_graph));

}

static void gtk_word_to_picture_graph_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_WORD_TO_PICTURE_GRAPH(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

//     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_word_to_picture_graph_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_WORD_TO_PICTURE_GRAPH(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

//     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_word_to_picture_graph_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_WORD_TO_PICTURE_GRAPH(widget));

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

static gboolean gtk_word_to_picture_graph_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_WORD_TO_PICTURE_GRAPH(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_word_to_picture_graph_paint(widget);

    return FALSE;
}


void gtk_word_to_picture_graph_update(GtkWord_to_picture_Graph *word_to_picture_graph)
{




    gtk_word_to_picture_graph_init(word_to_picture_graph);


    int s, p, l, d, domain; // p is target, q is network's response


    for (l = 0; l < word_to_picture_lesion_level; l++) {
        for (domain = 0; domain < N_DOMAIN; domain++) {
            /*	printf("%p\n", &word_to_picture_graph->close[l][domain]);*/
            word_to_picture_graph->close[l][domain]= 0.0;
            word_to_picture_graph->distant[l][domain] = 0.0;
            word_to_picture_graph->unrelated[l][domain] = 0.0;
        }
        point_x_close[l] = 0.0;
        point_y_close[l] = 0.0;
    }

    for (l = 0; l < 3; l++) {
        point_y_cross_section[l] = 0.0;
        point_x_cross_section[l] = 0.0;
    }

    if (word_to_picture_graph->data_type == MODEL) {


        for (d = 0; d < word_to_picture_lesion_distribution; d++) {
            for (l = 0; l < word_to_picture_lesion_level; l++) {
                for (s = 0; s < word_to_picture_sample; s++) {
                    for (p = 0; p < P; p++) {
                        if (!is_general_name(p))  {

                            if (!excluded_pattern(p)) {
                                word_to_picture_graph->close[l][get_domain(p)] += word_to_picture_close[d][l][s][p];
                                word_to_picture_graph->distant[l][get_domain(p)] += word_to_picture_distant[d][l][s][p];
                                word_to_picture_graph->unrelated[l][get_domain(p)] += word_to_picture_unrelated[d][l][s][p];


                            }
                        }

                    }
                }
            }
        }

        for (l = 0; l < word_to_picture_lesion_level; l++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {
                word_to_picture_graph->close[l][domain] /= (double) (word_to_picture_lesion_distribution * word_to_picture_sample * n_close[domain]);
                word_to_picture_graph->distant[l][domain] /= (double) (word_to_picture_lesion_distribution * word_to_picture_sample * n_distant[domain]);
                word_to_picture_graph->unrelated[l][domain] /= (double) (word_to_picture_lesion_distribution * word_to_picture_sample * n_unrelated[domain]);


            }

            word_to_picture_graph->se_close[l] = 0.0;
            word_to_picture_graph->se_distant[l] = 0.0;
            word_to_picture_graph->se_unrelated[l] = 0.0;
        }
        //calculate standard deviations
        for (d = 0; d < word_to_picture_lesion_distribution; d++) {
            for (l = 0; l < word_to_picture_lesion_level; l++) {
                for (s = 0; s < word_to_picture_sample; s++) {
                    for (p = 0; p < P; p++) {
                        if (!is_general_name(p))  {

                            if (!excluded_pattern(p)) {
                                word_to_picture_graph->se_close[l] += sqr(word_to_picture_close[d][l][s][p] - (word_to_picture_graph->close[l][0]+word_to_picture_graph->close[l][1])/2.0);
                                word_to_picture_graph->se_distant[l] += sqr(word_to_picture_distant[d][l][s][p] - (word_to_picture_graph->distant[l][0]+word_to_picture_graph->distant[l][1])/2.0);
                                word_to_picture_graph->se_unrelated[l] += sqr(word_to_picture_unrelated[d][l][s][p] - (word_to_picture_graph->unrelated[l][0]+word_to_picture_graph->unrelated[l][1])/2.0);
                            }
                        }

                    }
                }
            }
        }

        for (l = 0; l < word_to_picture_lesion_level; l++) {

            word_to_picture_graph->se_close[l] /= (double) (word_to_picture_lesion_distribution * word_to_picture_sample * (n_close[0] + n_close[1]) -1);
            word_to_picture_graph->se_distant[l] /= (double) (word_to_picture_lesion_distribution * word_to_picture_sample * (n_close[0] + n_close[1]) -1);
            word_to_picture_graph->se_unrelated[l] /= (double) (word_to_picture_lesion_distribution * word_to_picture_sample *  (n_close[0] + n_close[1]) -1);

            word_to_picture_graph->se_close[l] = sqrt(word_to_picture_graph->se_close[l]) / (double) sqrt(word_to_picture_lesion_distribution * word_to_picture_sample * (n_close[0] + n_close[1]));
            word_to_picture_graph->se_distant[l] = sqrt(word_to_picture_graph->se_distant[l]) / (double) sqrt(word_to_picture_lesion_distribution * word_to_picture_sample * (n_close[0] + n_close[1]));
            word_to_picture_graph->se_unrelated[l] = sqrt(word_to_picture_graph->se_unrelated[l]) / (double) sqrt(word_to_picture_lesion_distribution * word_to_picture_sample *  (n_close[0] + n_close[1]));


            //         printf("Close SE[%d] = %f\n", l ,word_to_picture_graph->se_close[l]);
            //         printf("Distant SE[%d] = %f\n", l ,word_to_picture_graph->se_distant[l]);
            //         printf("Unrelated SE[%d] = %f\n", l ,word_to_picture_graph->se_unrelated[l]);

        }
    } else if (word_to_picture_graph->data_type == PATIENTS) {
    }


    else if (word_to_picture_graph->data_type == ROGERS) {
    }



}

// void draw_word_to_picture_subgraph(cairo_t *cr, GtkWord_to_picture_Graph *word_to_picture_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p, int subgraph)
// {
//   double x_width = width - (x_offset * 2);
//   double y_height = height - (y_offset * 2);
//   double x_segments = (double) word_to_picture_lesion_level;
//   double y_segments = (y_height > 100) ? 10.0 : 5.0;
//   double x_space = x_width / x_segments;
//   double y_space = y_height / y_segments;
//   double x_axis = width - x_offset;
//   double y_axis = height - y_offset;
//
//
//     int i;
//
//   cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
//
//
//
//
//       // Transparent background
//       cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
//       cairo_paint(cr);
//       cairo_set_line_width(cr, 2.0);
//
//
//       int n_close[N_DOMAIN], n_distant[N_DOMAIN], n_unrelated[N_DOMAIN];
//       get_n_close_trials(&n_close[0]);
//       get_n_distant_trials(&n_distant[0]);
//       get_n_unrelated_trials(&n_unrelated[0]);
//
//       cairo_set_source_rgb(cr, 0.75, 0.1, 0.1);
//       if (word_to_picture_graph->type == 0) {
//
// 	  for (i = 0; i < word_to_picture_lesion_level; i++) {
// 	    cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * (word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis);
// 	  }
// 	  cairo_stroke(cr);
//       }
//       else {
//
// // 	for (i = 0; i < word_to_picture_lesion_level; i++) {
//   cairo_line_to(cr, (width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis);
//   cairo_line_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) + y_axis);
//   cairo_line_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) + y_axis);
//   // 	}(
//
// 	cairo_stroke(cr);
//
//       }
//
//       // draw axes
//       cairo_set_source_rgb(cr, 0, 0, 0);
//       cairo_set_line_width (cr, 2.5);
//       cairo_move_to(cr, x_axis, y_axis);
//       cairo_line_to(cr, x_offset, y_axis);
//       cairo_move_to(cr, x_offset, y_offset);
//       cairo_line_to(cr, x_offset, y_axis);
//       cairo_stroke(cr);
//       //
//       // draw axes' ticks
//       cairo_set_line_width (cr, 1.0);
//
//       for (i = 0; i < ((int)y_segments + 1); i++) {
// 	cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
// 	cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
// 	g_snprintf(buffer, 64, "%1.1f", 1 - i/ (double) y_segments);
// 	cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
// 	cairox_paint_pango_text(cr, &p, layout, buffer);
//       }
//       //
//       cairo_stroke(cr);
//       //
//
//       if (word_to_picture_graph->type == 0) {
// 	  for (i = 0; i < ((int)x_segments + 1); i++) {
// 	    // 	  printf("i = %i\n", WORD_TO_PICTURE_LESION_LEVEL);
// 	    cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
// 	    cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
// 	    g_snprintf(buffer, 64, "%1.1f", i * 100.0 / (double) word_to_picture_lesion_level);
// 	    cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
// 	    cairox_paint_pango_text(cr, &p, layout, buffer);
// 	    cairo_stroke(cr);
//
// 	  }
//       }
//       else {
// 	  for (i = 0; i < 3; i++) {
// 	    // 	  printf("i = %i\n", WORD_TO_PICTURE_LESION_LEVEL);
// 	    cairo_move_to(cr, (width - x_offset)/3.0 * i+(width - x_offset)/6.0, y_axis - 1.0);
// 	    cairo_line_to(cr, (width - x_offset)/3.0 * i+(width - x_offset)/6.0, y_axis + 4.0);
// 	    g_snprintf(buffer, 64, "%s", foil[i]);
// 	    cairox_text_parameters_set(&p, (width - x_offset)/3.0 * i+(width - x_offset)/6.0, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
// 	    cairox_paint_pango_text(cr, &p, layout, buffer);
// 	    cairo_stroke(cr);
//
//
// 	  }
//       }
//
//
//
//
//

//   }

static void draw_word_to_picture_legend(cairo_t *cr, GtkWord_to_picture_Graph *word_to_picture_graph, PangoLayout *layout, CairoxTextParameters p)
{
    // 	//

    pangox_layout_set_font_size(layout, 13);
    //draw legend

    pangox_layout_select_font_from_string(layout, "CMU Serif");
    if (word_to_picture_graph->legend) {



        if (word_to_picture_graph->type == 0) {

            g_snprintf(buffer, 64, "%s", "Close:");
            cairox_text_parameters_set(&p, 2*x_offset, 2*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_set_source_rgb(cr, 0.75, 0.1, 0.1);

        }
        else {
            if (word_to_picture_graph->current_level == -1) {
                word_to_picture_graph->current_level = (int) rintf(word_to_picture_lesion_level / 2.0);
            }
            char s[64];
            sprintf(s, "At %1.1f%%: ",  word_to_picture_graph->current_level * 100.0 / (double) word_to_picture_lesion_level);
            g_snprintf(buffer, 64, "%s", s);
            cairox_text_parameters_set(&p, 2*x_offset, 2*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_set_source_rgb(cr, 0.1, 0.25, 0.75);


        }



        cairo_move_to(cr, 4.5*x_offset-5, 2*y_offset+1);
        cairo_line_to(cr, 6*x_offset-5, 2*y_offset+1);
        cairo_stroke(cr);

    }
}

static void draw_word_to_picture_axes_labels(cairo_t *cr, GtkWord_to_picture_Graph *word_to_picture_graph, double width, double height, PangoLayout *layout, CairoxTextParameters p)
{


    g_snprintf(buffer, 64, "Proportion Correct");
    cairox_text_parameters_set(&p, x_offset - 33.0, (height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
    cairox_paint_pango_text(cr, &p, layout, buffer);
    cairo_stroke(cr);

    if (word_to_picture_graph->type == 0)
    {
        g_snprintf(buffer, 64, "Percent Connections Lesioned");
        cairox_text_parameters_set(&p, (width - x_offset)/2.0, (height - y_offset) + 27.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_stroke(cr);
    }
    else {
        g_snprintf(buffer, 64, "Distance of Foils");
        cairox_text_parameters_set(&p, (width - x_offset)/2.0, (height - y_offset) + 27.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_stroke(cr);
    }
}

static void draw_word_to_picture_graph(cairo_t *cr, GtkWidget *widget, double width, double height)
{


//     double width = (double) widget->allocation.width, height = (double) widget->allocation.height;
    GtkWord_to_picture_Graph *word_to_picture_graph =  GTK_WORD_TO_PICTURE_GRAPH(widget);
    PangoLayout *layout = pango_cairo_create_layout(cr);
    CairoxTextParameters p;


    x_offset = 40.0;
    y_offset = 40.0;

    draw_word_to_picture_legend(cr, word_to_picture_graph, layout, p);
    draw_word_to_picture_axes_labels(cr, word_to_picture_graph, width, height, layout, p);





    double x_width = width - (x_offset * 2);
    double y_height = height - (y_offset * 2);
    double x_segments = (double) word_to_picture_lesion_level;
    double y_segments = (y_height > 100) ? 10.0 : 5.0;
    double x_space = x_width / x_segments;
    double y_space = y_height / y_segments;
    double x_axis = width - x_offset;
    double y_axis = height - y_offset;


    int i;

    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);




    // Transparent background
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width(cr, 2.0);


    int n_close[N_DOMAIN], n_distant[N_DOMAIN], n_unrelated[N_DOMAIN];
    get_n_close_trials(&n_close[0]);
    get_n_distant_trials(&n_distant[0]);
    get_n_unrelated_trials(&n_unrelated[0]);

    if (word_to_picture_graph->type == 0) {
        cairo_set_source_rgb(cr, 0.75, 0.1, 0.1);
        i = 0;
        cairo_move_to(cr, x_offset + i * x_space, -y_space * 10 * (word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis);

        for (i = 0; i < word_to_picture_lesion_level; i++) {
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * (word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis);
        }
        cairo_stroke(cr);

        for (i = 0; i < word_to_picture_lesion_level; i++) {

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_space * 10 * ((word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + word_to_picture_graph->se_close[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_space * 10 * ((word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + word_to_picture_graph->se_close[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space, -y_space * 10 * ((word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + word_to_picture_graph->se_close[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space, -y_space * 10 * ((word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) - word_to_picture_graph->se_close[i]) + y_axis);

            cairo_move_to(cr, x_offset + i * x_space + 4, -y_space * 10 * ((word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) - word_to_picture_graph->se_close[i]) + y_axis);
            cairo_line_to(cr, x_offset + i * x_space - 4, -y_space * 10 * ((word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) - word_to_picture_graph->se_close[i]) + y_axis);

        }
        cairo_stroke(cr);

        for (i = 0; i < word_to_picture_lesion_level; i++) {
            cairo_arc(cr, x_offset + i * x_space, -y_space * 10 * (word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis, 3.0, 0.0, 2*M_PI);
            cairo_fill(cr);
            point_x_close[i] = x_offset + i * x_space;
            point_y_close[i] = -y_space * 10 * (word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis;
            //CIRKICKLES!!!! ZOMGZYEAH I'm very serious normally, but circacious cirkikcles made me sillzies. SILLIZORS.

// 	    printf("data = %f\n", (word_to_picture_graph->close[i][0] * n_close[0] + word_to_picture_graph->close[i][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]));
        }
    }
    else {
        cairo_set_source_rgb(cr, 0.1, 0.25, 0.75);
        cairo_move_to(cr, (width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis);

        cairo_line_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) + y_axis);

        cairo_line_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) + y_axis);
        // 	}(

        cairo_stroke(cr);

        // error bars
        cairo_move_to(cr, (width - x_offset)/6.0, -y_space * 10 * ((word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + word_to_picture_graph->se_close[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/6.0, -y_space * 10 * ((word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) - word_to_picture_graph->se_close[word_to_picture_graph->current_level]) + y_axis);

        cairo_move_to(cr, (width - x_offset)/6.0 + 4, -y_space * 10 * ((word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + word_to_picture_graph->se_close[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/6.0 - 4, -y_space * 10 * ((word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + word_to_picture_graph->se_close[word_to_picture_graph->current_level]) + y_axis);

        cairo_move_to(cr, (width - x_offset)/6.0 + 4, -y_space * 10 * ((word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) - word_to_picture_graph->se_close[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/6.0 - 4, -y_space * 10 * ((word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) - word_to_picture_graph->se_close[word_to_picture_graph->current_level]) + y_axis);
        //error bars for distant
        cairo_move_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0, -y_space * 10 * ((word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) + word_to_picture_graph->se_distant[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0, -y_space * 10 * ((word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) - word_to_picture_graph->se_distant[word_to_picture_graph->current_level]) + y_axis);

        cairo_move_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0 + 4, -y_space * 10 * ((word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) + word_to_picture_graph->se_distant[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0 - 4, -y_space * 10 * ((word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) + word_to_picture_graph->se_distant[word_to_picture_graph->current_level]) + y_axis);

        cairo_move_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0 + 4, -y_space * 10 * ((word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) - word_to_picture_graph->se_distant[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/3.0+(width - x_offset)/6.0 - 4, -y_space * 10 * ((word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) - word_to_picture_graph->se_distant[word_to_picture_graph->current_level]) + y_axis);

        cairo_move_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0, -y_space * 10 * ((word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) + word_to_picture_graph->se_unrelated[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0, -y_space * 10 * ((word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) - word_to_picture_graph->se_unrelated[word_to_picture_graph->current_level]) + y_axis);

        cairo_move_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0 + 4, -y_space * 10 * ((word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) + word_to_picture_graph->se_unrelated[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0 - 4, -y_space * 10 * ((word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) + word_to_picture_graph->se_unrelated[word_to_picture_graph->current_level]) + y_axis);

        cairo_move_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0 + 4, -y_space * 10 * ((word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) - word_to_picture_graph->se_unrelated[word_to_picture_graph->current_level]) + y_axis);
        cairo_line_to(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0 - 4, -y_space * 10 * ((word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) - word_to_picture_graph->se_unrelated[word_to_picture_graph->current_level]) + y_axis);


        cairo_stroke(cr);

        cairo_arc(cr, (width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis, 3.0, 0.0, 2*M_PI);
        cairo_fill(cr);
        value_cross_section[0] = (word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]);
        point_y_cross_section[0] = -y_space * 10 * (word_to_picture_graph->close[word_to_picture_graph->current_level][0] * n_close[0] + word_to_picture_graph->close[word_to_picture_graph->current_level][1] * n_close[1]) / (double)  (n_close[0] + n_close[1]) + y_axis;
        point_x_cross_section[0] = (width - x_offset)/6.0;

        cairo_arc(cr, (width - x_offset)/3.0+(width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) + y_axis, 3.0, 0.0, 2*M_PI);
        cairo_fill(cr);

        value_cross_section[1] = (word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]);
        point_y_cross_section[1] = -y_space * 10 * (word_to_picture_graph->distant[word_to_picture_graph->current_level][0] * n_distant[0] + word_to_picture_graph->distant[word_to_picture_graph->current_level][1] * n_distant[1]) / (double)  (n_distant[0] + n_distant[1]) + y_axis;
        point_x_cross_section[1] = (width - x_offset)/3.0+(width - x_offset)/6.0;

        cairo_arc(cr, (width - x_offset)/3.0 * 2+(width - x_offset)/6.0, -y_space * 10 * (word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) + y_axis, 3.0, 0.0, 2*M_PI);

        value_cross_section[2] = (word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]);
        point_y_cross_section[2] = -y_space * 10 * (word_to_picture_graph->unrelated[word_to_picture_graph->current_level][0] * n_unrelated[0] + word_to_picture_graph->unrelated[word_to_picture_graph->current_level][1] * n_unrelated[1]) / (double)  (n_unrelated[0] + n_unrelated[1]) + y_axis;
        point_x_cross_section[2] = (width - x_offset)/3.0 * 2+(width - x_offset)/6.0;

        cairo_fill(cr);

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

    if (word_to_picture_graph->type == 0) {
        for (i = 0; i < ((int)x_segments + 1); i++) {
            cairo_move_to(cr, x_offset + i*x_space, y_axis - 1.0);
            cairo_line_to(cr, x_offset + i*x_space, y_axis + 4.0);
            g_snprintf(buffer, 64, "%1.1f", i * 100.0 / (double) word_to_picture_lesion_level);
            cairox_text_parameters_set(&p, x_offset + i*x_space, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_stroke(cr);

        }
    }
    else {
        for (i = 0; i < 3; i++) {
            cairo_move_to(cr, (width - x_offset)/3.0 * i+(width - x_offset)/6.0, y_axis - 1.0);
            cairo_line_to(cr, (width - x_offset)/3.0 * i+(width - x_offset)/6.0, y_axis + 4.0);
            g_snprintf(buffer, 64, "%s", foil[i]);
            cairox_text_parameters_set(&p, (width - x_offset)/3.0 * i+(width - x_offset)/6.0, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_stroke(cr);


        }
    }

    g_object_unref(layout);
    cairo_destroy(cr);

}
void gtk_word_to_picture_graph_pdf(GtkWidget *widget, char *filename)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = 500;//widget->allocation.width;
    int height = 500;//widget->allocation.height;
    char name[100];
    int i = 0;

    while (filename[i] != '\0') {
        name[i] = filename[i];
        i++;
    }
    name[i] = '\0';
    printf("%s\n", name);
    sprintf(name, "%s_word_to_picture.pdf", name);


    surface = cairo_pdf_surface_create(name, width, height);
    cr = cairo_create(surface);

    // draw word_to_picture_graph
    draw_word_to_picture_graph(cr, widget, width, height);


    cairo_surface_destroy(surface);

}


static void gtk_word_to_picture_graph_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    draw_word_to_picture_graph(cr, widget, (double) widget->allocation.width, (double) widget->allocation.height);
}

void gtk_word_to_picture_graph_reset(GtkWord_to_picture_Graph *word_to_picture_graph)
{
    gtk_word_to_picture_graph_init(word_to_picture_graph);
    gtk_word_to_picture_graph_redraw(GTK_WIDGET(word_to_picture_graph));
}

static void gtk_word_to_picture_graph_destroy(GtkObject *object)
{
//     GtkWord_to_picture_Graph *word_to_picture_graph;
    GtkWord_to_picture_GraphClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_WORD_TO_PICTURE_GRAPH(object));

//     word_to_picture_graph = GTK_WORD_TO_PICTURE_GRAPH(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_word_to_picture_graph_redraw(GtkWidget *widget)
{
    GdkRegion *region;
    if (!widget->window) return;

    region = gdk_drawable_get_clip_region (widget->window);
    /* redraw the cairo canvas completely by exposing it */
    gdk_window_invalidate_region (widget->window, region, TRUE);
    gdk_window_process_updates (widget->window, TRUE);
    gdk_region_destroy (region);

}


void gtk_word_to_picture_graph_clear(GtkWidget *widget)
{
    init_word_to_picture_response(0, 0, 0);
    gtk_word_to_picture_graph_reset(GTK_WORD_TO_PICTURE_GRAPH(widget));

}
