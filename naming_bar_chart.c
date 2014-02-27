#include "naming_bar_chart.h"

extern void cairox_select_colour_scale(cairo_t *cr, double s);
#define n_patients 3
double mean_delayed_copy_errors[n_patients], mean_drawing_errors[n_patients];

static void gtk_naming_bar_chart_class_init(GtkNaming_Bar_chartClass *klass);
GtkType gtk_naming_bar_chart_get_type(void);
static void gtk_naming_bar_chart_init(GtkNaming_Bar_chart *naming_bar_chart);
static void gtk_naming_bar_chart_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_naming_bar_chart_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_naming_bar_chart_realize(GtkWidget *widget);
static gboolean gtk_naming_bar_chart_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_naming_bar_chart_paint(GtkWidget *widget);
static void gtk_naming_bar_chart_destroy(GtkObject *object);
void cairox_select_colour_scale(cairo_t *cr, double d);

static char buffer[64];
static const double superordinate_dash[1] = {5.0};
static const double crossdomain_dash[2] = {1.0, 4.0};
static const double omission_dash[4] = {1.0, 1.0, 5.0, 1.0};
static double x_space = 0;
static double x_width = 0;
static double y_height = 0;
static double x_segments = 0;
static double y_segments = 0;
static double y_space = 0;
static double x_axis = 0;
static double y_axis = 0;
static double se_threshold = 0.003;


static char patient_name[3][5] = {"D.S.\0", "D.C.\0", "I.F.\0"};
static double rogers_lesion_level[3] = {20.0, 25.0, 30.0};


GtkType gtk_naming_bar_chart_get_type(void)
{
    static GtkType gtk_naming_bar_chart_type = 0;
    if (!gtk_naming_bar_chart_type)
    {
        static const GtkTypeInfo gtk_naming_bar_chart_info = {"GtkNaming_Bar_chart", sizeof(GtkNaming_Bar_chart), sizeof(GtkNaming_Bar_chartClass), (GtkClassInitFunc) gtk_naming_bar_chart_class_init, (GtkObjectInitFunc) gtk_naming_bar_chart_init, NULL, NULL, (GtkClassInitFunc) NULL};
        gtk_naming_bar_chart_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_naming_bar_chart_info);
    }
    return gtk_naming_bar_chart_type;
}

GtkWidget * gtk_naming_bar_chart_new()
{
    return GTK_WIDGET(gtk_type_new(gtk_naming_bar_chart_get_type()));
}



static void gtk_naming_bar_chart_class_init(GtkNaming_Bar_chartClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;
//
    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_naming_bar_chart_realize;
    widget_class->size_request = gtk_naming_bar_chart_size_request;
    widget_class->size_allocate = gtk_naming_bar_chart_size_allocate;
    widget_class->expose_event = gtk_naming_bar_chart_expose;
    object_class->destroy = gtk_naming_bar_chart_destroy;
}



double bar_x[NAMING_LESION_LEVEL][2];
double bar_height[NAMING_LESION_LEVEL][2];
double bar_value[NAMING_LESION_LEVEL][2];

static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    double coordx = 0, coordy = 0;
    GtkNaming_Bar_chart *naming_bar_chart = GTK_NAMING_BAR_CHART(widget);

    gtk_naming_bar_chart_redraw(widget);



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

    int i_max = max(naming_lesion_level, n_patients);
    for (i = 0; i < i_max; i++) {

//        printf("%d x = %f, y = %f x1 = %f, x2 = %f y1 = %f, y2 = %f\n", i, coordx, coordy, bar_x[i][1], bar_x[i][1] + x_space*0.45, y_axis, bar_height[i][1] + y_axis);



        /*
        	  printf("y1 = %f, y2 = %f\n", y_axis, bar_height[i][0] + y_axis);
                  printf("y3 = %f, y4 = %f\n", y_axis, bar_height[i][2] + y_axis);*/


        if ((naming_bar_chart->data_type == PATIENTS) || (naming_bar_chart->data_type == ROGERS)) {
            if (i >= n_patients) {
                break;
            }
        }
        if ((coordx >= bar_x[i][0]) && (coordx < (bar_x[i][0] + x_space*0.45)) &&
                (coordy <= y_axis) && (coordy > (bar_height[i][0] + y_axis))) {
            g_snprintf (buffer, 64, "%1.3f", bar_value[i][0]);
            pangox_layout_set_font_size(layout, 10.0);
            cairox_text_parameters_set (&text_p, bar_x[i][0] + x_space*0.45*0.5 , bar_height[i][0] + y_axis - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text (cr, &text_p, layout, buffer);
            break;

        }
        if ((coordx >= bar_x[i][1]) && (coordx < (bar_x[i][1]+ x_space*0.45)) &&
                (coordy <= y_axis) && (coordy > (bar_height[i][1] + y_axis))) {
            g_snprintf (buffer, 64, "%1.3f", bar_value[i][1]);
            cairox_text_parameters_set (&text_p, bar_x[i][1] + x_space*0.45*0.5 , bar_height[i][1] + y_axis - pangox_layout_get_font_height(layout), PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            pangox_layout_set_font_size(layout, 10.0);
            cairox_paint_pango_text (cr, &text_p, layout, buffer);
            break;
        }
    }



    g_object_unref (layout);
    cairo_destroy (cr);

    return TRUE;
}


static char y_label[4][128] = {"Delayed Copy: Prop. Omitted", "Drawing: Prop. Omitted", "Delayed Copy: Prop. of Intrusions", "Drawing: Prop. of Intrusions"};

double x_offset;
double y_offset;
int n_pattern[2];
static void gtk_naming_bar_chart_init(GtkNaming_Bar_chart *naming_bar_chart)
{

    int l, domain;

    gtk_widget_add_events(GTK_WIDGET(naming_bar_chart), GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(naming_bar_chart), "button-press-event", G_CALLBACK(clicked), NULL);

    naming_bar_chart->data_type = MODEL;

    for (l = 0; l < NAMING_LESION_LEVEL; l++) {

        for (domain = 0; domain < N_DOMAIN; domain++) {
            naming_bar_chart->drawing_omission[l][domain] = 0.0;
            naming_bar_chart->drawing_intrusion[l][domain] = 0.0;
            naming_bar_chart->copying_omission[l][domain] = 0.0;
            naming_bar_chart->copying_intrusion[l][domain] = 0.0;

            naming_bar_chart->drawing_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->drawing_intrusion_proportion[l][domain] = 0.0;
            naming_bar_chart->copying_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->copying_intrusion_proportion[l][domain] = 0.0;

            bar_height[l][domain] = 0.0;
            bar_height[l][domain] = 0.0;

            naming_bar_chart->se_drawing_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->se_drawing_intrusion_proportion[l][domain] = 0.0;
            naming_bar_chart->se_copying_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->se_copying_intrusion_proportion[l][domain] = 0.0;


        }

        naming_bar_chart->se_drawing_overall[l] = 0.0;
        naming_bar_chart->se_copying_overall[l] = 0.0;
    }


    naming_bar_chart->type = 0;

    if (EXCLUDE_FRUIT) {

        n_pattern[0] = get_domain_n_pattern(0) - get_n_fruit();// animals
        n_pattern[1] = get_domain_n_pattern(1); // artifacts

    }
    else
    {

        n_pattern[0] = get_domain_n_pattern(0);
        n_pattern[1] = get_domain_n_pattern(1);
    }

    //   printf("%i %i \n", n_pattern[0], n_pattern[1]);


}

void gtk_naming_bar_chart_set_type(GtkNaming_Bar_chart *naming_bar_chart, int type)
{
    naming_bar_chart->type = type;

}

void gtk_naming_bar_chart_set_data_type(GtkNaming_Bar_chart *naming_bar_chart, int type)
{

    naming_bar_chart->data_type = type;
    gtk_naming_bar_chart_update(naming_bar_chart);
    gtk_naming_bar_chart_redraw(GTK_WIDGET(naming_bar_chart));

}


static void gtk_naming_bar_chart_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_NAMING_BAR_CHART(widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

//     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_naming_bar_chart_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_NAMING_BAR_CHART(widget));
    g_return_if_fail (allocation != NULL);

    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

//     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void gtk_naming_bar_chart_realize(GtkWidget *widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(GTK_IS_NAMING_BAR_CHART(widget));

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

static gboolean gtk_naming_bar_chart_expose(GtkWidget *widget, GdkEventExpose *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_NAMING_BAR_CHART(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    gtk_naming_bar_chart_paint(widget);

    return FALSE;
}


extern double activation_threshold;




static void draw_naming_subbar_chart(cairo_t *cr, GtkNaming_Bar_chart *naming_bar_chart, double width, double height, PangoLayout *layout, CairoxTextParameters p, int subbar_chart)
{
    int i;
    double max = 0.0, max_proportion = 0.0;





    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);

//     int i;

    // Transparent background
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width(cr, 2.0);




    if (naming_bar_chart->type == 0) {


//         for (i = 0; i < naming_lesion_level; i++) {
//             max = fmax(max, ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1])));
//             max = fmax(max, ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1])));
//         }
//
//         for (i = 0; i < n_patients; i++) {
//             max = fmax(max, mean_delayed_copy_errors[i]);
//             max = fmax(max, mean_drawing_errors[i]);
//         }
//         max = round(max+1);
        max = 11.0;
//     printf("beep: %i\n", naming_bar_chart->data_type == PATIENTS);

        if ((naming_bar_chart->data_type == PATIENTS) || (naming_bar_chart->data_type == ROGERS)) {

// 	for (i = 0; i < n_patients; i++) {
// 	  max = fmax(max, mean_delayed_copy_errors[i]);
// 	  max = fmax(max, mean_drawing_errors[i]);
// 	}
// 	max = round(max+1);
            x_width = width - (x_offset * 2);
            y_height = height - (y_offset * 2);
            x_segments = (double) n_patients;
            y_segments = max;
            x_space = x_width / x_segments;
            y_space = y_height / y_segments;
            x_axis = width - x_offset;
            y_axis = height - y_offset;

            for (i = 0; i < n_patients; i++) {
// 	      166.0/255.0,205.0/255.0,200.0/255.0
                cairo_set_source_rgb(cr, 166.0/255.0, 205.0/255.0, 200.0/255.0);

                bar_x[i][0] = x_axis - x_width + x_space*(i+0.05);
                bar_value[i][0] = mean_drawing_errors[i];
                bar_height[i][0] = -y_space * mean_drawing_errors[i];
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.05), y_axis, x_space*0.45, -y_space * mean_drawing_errors[i]);

                cairo_fill(cr);

                if (naming_bar_chart->se_drawing_overall[i] > se_threshold) {
                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_space * (mean_drawing_errors[i] + naming_bar_chart->se_drawing_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_space * (mean_drawing_errors[i] - naming_bar_chart->se_drawing_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) +4.0, y_axis -y_space * (mean_drawing_errors[i] + naming_bar_chart->se_drawing_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275)-4.0, y_axis -y_space * (mean_drawing_errors[i] + naming_bar_chart->se_drawing_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) +4.0, y_axis -y_space * (mean_drawing_errors[i] - naming_bar_chart->se_drawing_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275)-4.0, y_axis -y_space * (mean_drawing_errors[i] - naming_bar_chart->se_drawing_overall[i]));
                    cairo_stroke(cr);
                }


                cairo_set_source_rgb(cr, 73.0/255.0,40.0/255.0,68.0/255.0);
                bar_x[i][1] = x_axis - x_width + x_space*(i+0.5);
                bar_value[i][1] = mean_delayed_copy_errors[i];
                bar_height[i][1]= -y_space * mean_delayed_copy_errors[i];


                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.5), y_axis, x_space*0.45, -y_space * mean_delayed_copy_errors[i]);

                cairo_fill(cr);


                if (naming_bar_chart->se_copying_overall[i] > se_threshold) {
                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_space * (mean_delayed_copy_errors[i] + naming_bar_chart->se_copying_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_space * (mean_delayed_copy_errors[i] - naming_bar_chart->se_copying_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) +4, y_axis -y_space * (mean_delayed_copy_errors[i] + naming_bar_chart->se_copying_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725)-4, y_axis -y_space * (mean_delayed_copy_errors[i] + naming_bar_chart->se_copying_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) +4, y_axis -y_space * (mean_delayed_copy_errors[i] - naming_bar_chart->se_copying_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725)-4, y_axis -y_space * (mean_delayed_copy_errors[i] - naming_bar_chart->se_copying_overall[i]));
                    cairo_stroke(cr);
                }


            }
        }
        else {


            x_width = width - (x_offset * 2);
            y_height = height - (y_offset * 2);
            x_segments = (double) naming_lesion_level;
            y_segments = max;
            x_space = x_width / x_segments;
            y_space = y_height / y_segments;
            x_axis = width - x_offset;
            y_axis = height - y_offset;
            for (i = 0; i < naming_lesion_level; i++) {
                cairo_set_source_rgb(cr, 166.0/255.0, 205.0/255.0, 200.0/255.0);

                bar_x[i][0] = x_axis - x_width + x_space*(i+0.05);
                bar_value[i][0] = ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]));
                bar_height[i][0] = -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]));
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.05), y_axis, x_space*0.45, -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1])));

                cairo_fill(cr);

                if (naming_bar_chart->se_drawing_overall[i] > se_threshold) {
                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) + naming_bar_chart->se_drawing_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) - naming_bar_chart->se_drawing_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) +4.0, y_axis -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) + naming_bar_chart->se_drawing_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275)-4.0, y_axis -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) + naming_bar_chart->se_drawing_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) +4.0, y_axis -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) - naming_bar_chart->se_drawing_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275)-4.0, y_axis -y_space * ((naming_bar_chart->drawing_omission[i][0] + naming_bar_chart->drawing_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->drawing_omission[i][1]+naming_bar_chart->drawing_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) - naming_bar_chart->se_drawing_overall[i]));
                    cairo_stroke(cr);
                }


                cairo_set_source_rgb(cr, 73.0/255.0,40.0/255.0,68.0/255.0);
                bar_x[i][1] = x_axis - x_width + x_space*(i+0.5);
                bar_value[i][1] = ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]));
                bar_height[i][1]= -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]));
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.5), y_axis, x_space*0.45, -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1])));

                cairo_fill(cr);


                if (naming_bar_chart->se_copying_overall[i] > se_threshold) {
                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) + naming_bar_chart->se_copying_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) - naming_bar_chart->se_copying_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) +4, y_axis -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) + naming_bar_chart->se_copying_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725)-4, y_axis -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) + naming_bar_chart->se_copying_overall[i]));

                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) +4, y_axis -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) - naming_bar_chart->se_copying_overall[i]));

                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725)-4, y_axis -y_space * ((naming_bar_chart->copying_omission[i][0] + naming_bar_chart->copying_intrusion[i][0]) * n_pattern[0] + (naming_bar_chart->copying_omission[i][1]+naming_bar_chart->copying_intrusion[i][1]) * n_pattern[1] / (double) (n_pattern[0] + n_pattern[1]) - naming_bar_chart->se_copying_overall[i]));
                    cairo_stroke(cr);
                }


            }
        }
    }
    else if (naming_bar_chart->type == 1) {


        for (i = 0; i < naming_lesion_level; i++) {
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_omission_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_omission_proportion[i][1]);
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_intrusion_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_intrusion_proportion[i][1]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_omission_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_omission_proportion[i][1]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_intrusion_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_intrusion_proportion[i][1]);
        }
        max_proportion = ceilf(max_proportion * 10) / 10 + 0.1;


        x_width = width - (x_offset * 2);
        y_height = height - (y_offset * 2);
        x_segments = (double) naming_lesion_level;
        y_segments = 3.0;//(y_height > 100) ? 10.0 : 5.0;
        x_space = x_width / x_segments;
        y_space = y_height / y_segments;
        x_axis = width - x_offset;
        y_axis = height - y_offset;

        if (subbar_chart == 1) {
            for (i = 0; i < naming_lesion_level; i++) {
                cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
                bar_x[i][0] = x_axis - x_width + x_space*(i+0.05);
                bar_value[i][0] = naming_bar_chart->drawing_omission_proportion[i][0];
                bar_height[i][0] = y_axis-y_height * naming_bar_chart->drawing_omission_proportion[i][0] / max_proportion;

                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.05), y_axis, x_space*0.45, -y_height* naming_bar_chart->drawing_omission_proportion[i][0] / max_proportion);
                cairo_fill(cr);

                if (naming_bar_chart->se_drawing_omission_proportion[i][0] > se_threshold) {
                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][0] + naming_bar_chart->se_drawing_omission_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][0] - naming_bar_chart->se_drawing_omission_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][0] + naming_bar_chart->se_drawing_omission_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][0] + naming_bar_chart->se_drawing_omission_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][0] - naming_bar_chart->se_drawing_omission_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][0] - naming_bar_chart->se_drawing_omission_proportion[i][0]) / max_proportion);
                    cairo_stroke(cr);
                }

                cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
                bar_x[i][1] = x_axis - x_width + x_space*(i+0.5);
                bar_value[i][1] = naming_bar_chart->drawing_omission_proportion[i][1];
                bar_height[i][1] = -y_space * 10.0 *naming_bar_chart->drawing_omission_proportion[i][1] / max_proportion;
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.5), y_axis, x_space*0.45, -y_height *naming_bar_chart->drawing_omission_proportion[i][1] / max_proportion);
                cairo_fill(cr);

                if (naming_bar_chart->se_drawing_omission_proportion[i][1] > se_threshold) {

                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][1] + naming_bar_chart->se_drawing_omission_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][1] - naming_bar_chart->se_drawing_omission_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][1] + naming_bar_chart->se_drawing_omission_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][1] + naming_bar_chart->se_drawing_omission_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][1] - naming_bar_chart->se_drawing_omission_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_omission_proportion[i][1] - naming_bar_chart->se_drawing_omission_proportion[i][1]) / max_proportion);

                    cairo_stroke(cr);
                }
            }
        }
        else {
            for (i = 0; i < naming_lesion_level; i++) {
                cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
                bar_x[i][0] = x_axis - x_width + x_space*(i+0.05);
                bar_value[i][0] = naming_bar_chart->copying_omission_proportion[i][0];
                bar_height[i][0] = -y_space * 10.0 * naming_bar_chart->copying_omission_proportion[i][0] / max_proportion;
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.05), y_axis, x_space*0.45, -y_height* naming_bar_chart->copying_omission_proportion[i][0] / max_proportion);
                cairo_fill(cr);

                if (naming_bar_chart->se_drawing_omission_proportion[i][0] > se_threshold) {
                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][0] + naming_bar_chart->se_copying_omission_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][0] - naming_bar_chart->se_copying_omission_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][0] + naming_bar_chart->se_copying_omission_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][0] + naming_bar_chart->se_copying_omission_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][0] - naming_bar_chart->se_copying_omission_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][0] - naming_bar_chart->se_copying_omission_proportion[i][0]) / max_proportion);
                    cairo_stroke(cr);
                }


                cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
                bar_x[i][1] = x_axis - x_width + x_space*(i+0.5);
                bar_value[i][1] = naming_bar_chart->copying_omission_proportion[i][1];
                bar_height[i][1] = -y_space * 10.0 *naming_bar_chart->copying_omission_proportion[i][1] / max_proportion;
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.5), y_axis, x_space*0.45, -y_height *naming_bar_chart->copying_omission_proportion[i][1] / max_proportion);
                cairo_fill(cr);


                if (naming_bar_chart->se_copying_omission_proportion[i][1] > se_threshold) {
                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][1] + naming_bar_chart->se_copying_omission_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][1] - naming_bar_chart->se_copying_omission_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][1] + naming_bar_chart->se_copying_omission_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][1] + naming_bar_chart->se_copying_omission_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][1] - naming_bar_chart->se_copying_omission_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->copying_omission_proportion[i][1] - naming_bar_chart->se_copying_omission_proportion[i][1]) / max_proportion);
                    cairo_stroke(cr);
                }

            }
        }
    }
    else if (naming_bar_chart->type == 2) {



        for (i = 0; i < naming_lesion_level; i++) {
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_omission_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_omission_proportion[i][1]);
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_intrusion_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->drawing_intrusion_proportion[i][1]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_omission_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_omission_proportion[i][1]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_intrusion_proportion[i][0]);
            max_proportion = fmax(max_proportion, naming_bar_chart->copying_intrusion_proportion[i][1]);
        }
        max_proportion = ceilf(max_proportion * 10) / 10 + 0.1;


        x_width = width - (x_offset * 2);
        y_height = height - (y_offset * 2);
        x_segments = (double) naming_lesion_level;
        y_segments = 3.0;//(y_height > 100) ? 10.0 : 5.0;
        x_space = x_width / x_segments;
        y_space = y_height / y_segments;
        x_axis = width - x_offset;
        y_axis = height - y_offset;


        if (subbar_chart == 1) {
            for (i = 0; i < naming_lesion_level; i++) {
                bar_x[i][0] = x_axis - x_width + x_space*(i+0.05);
                bar_value[i][0] = naming_bar_chart->drawing_intrusion_proportion[i][0];
                bar_height[i][0] = -y_height * 2 * naming_bar_chart->drawing_intrusion_proportion[i][0] / max_proportion;
                cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.05), y_axis, x_space*0.45, -y_height * naming_bar_chart->drawing_intrusion_proportion[i][0] / max_proportion);
                cairo_fill(cr);

                if (naming_bar_chart->se_drawing_intrusion_proportion[i][0] > se_threshold) {

                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][0] + naming_bar_chart->se_drawing_intrusion_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][0] - naming_bar_chart->se_drawing_intrusion_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][0] + naming_bar_chart->se_drawing_intrusion_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][0] + naming_bar_chart->se_drawing_intrusion_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][0] - naming_bar_chart->se_drawing_intrusion_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][0] - naming_bar_chart->se_drawing_intrusion_proportion[i][0]) / max_proportion);
                    cairo_stroke(cr);
                }


                cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
                bar_x[i][1] = x_axis - x_width + x_space*(i+0.5);
                bar_value[i][1] = naming_bar_chart->drawing_intrusion_proportion[i][1];
                bar_height[i][1] = -y_space * 10.0 * naming_bar_chart->drawing_intrusion_proportion[i][1] / max_proportion;
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.5), y_axis, x_space*0.45, -y_height * naming_bar_chart->drawing_intrusion_proportion[i][1] / max_proportion);
                cairo_fill(cr);

                if (naming_bar_chart->se_drawing_intrusion_proportion[i][1] > se_threshold) {

                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][1] + naming_bar_chart->se_drawing_intrusion_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][1] - naming_bar_chart->se_drawing_intrusion_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][1] + naming_bar_chart->se_drawing_intrusion_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][1] + naming_bar_chart->se_drawing_intrusion_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][1] - naming_bar_chart->se_drawing_intrusion_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->drawing_intrusion_proportion[i][1] - naming_bar_chart->se_drawing_intrusion_proportion[i][1]) / max_proportion);
                    cairo_stroke(cr);
                }
            }
        }
        else {

            for (i = 0; i < naming_lesion_level; i++) {
                bar_x[i][0] = x_axis - x_width + x_space*(i+0.05);
                bar_value[i][0] = naming_bar_chart->copying_intrusion_proportion[i][0];
                bar_height[i][0] = -y_height * 2 * naming_bar_chart->copying_intrusion_proportion[i][0] / max_proportion;
                cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.05), y_axis, x_space*0.45, -y_height * naming_bar_chart->copying_intrusion_proportion[i][0] / max_proportion);
                cairo_fill(cr);

                if (naming_bar_chart->se_copying_intrusion_proportion[i][0] > se_threshold) {

                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][0] + naming_bar_chart->se_copying_intrusion_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275), y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][0] - naming_bar_chart->se_copying_intrusion_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][0] + naming_bar_chart->se_copying_intrusion_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][0] + naming_bar_chart->se_copying_intrusion_proportion[i][0]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.275) + 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][0] - naming_bar_chart->se_copying_intrusion_proportion[i][0]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.275) - 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][0] - naming_bar_chart->se_copying_intrusion_proportion[i][0]) / max_proportion);
                    cairo_stroke(cr);
                }


                cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
                bar_x[i][1] = x_axis - x_width + x_space*(i+0.5);
                bar_value[i][1] = naming_bar_chart->copying_intrusion_proportion[i][1];
                bar_height[i][1] = -y_space * 10.0 * naming_bar_chart->copying_intrusion_proportion[i][1] / max_proportion;
                cairo_rectangle(cr, x_axis - x_width + x_space*(i+0.5), y_axis, x_space*0.45, -y_height * naming_bar_chart->copying_intrusion_proportion[i][1] / max_proportion);
                cairo_fill(cr);

                if (naming_bar_chart->se_copying_intrusion_proportion[i][1] > se_threshold) {

                    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][1] + naming_bar_chart->se_copying_intrusion_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725), y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][1] - naming_bar_chart->se_copying_intrusion_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][1] + naming_bar_chart->se_copying_intrusion_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][1] + naming_bar_chart->se_copying_intrusion_proportion[i][1]) / max_proportion);
                    cairo_move_to(cr, x_axis - x_width + x_space*(i+0.725) + 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][1] - naming_bar_chart->se_copying_intrusion_proportion[i][1]) / max_proportion);
                    cairo_line_to(cr, x_axis - x_width + x_space*(i+0.725) - 4.0, y_axis -y_height* (naming_bar_chart->copying_intrusion_proportion[i][1] - naming_bar_chart->se_copying_intrusion_proportion[i][1]) / max_proportion);
                    cairo_stroke(cr);
                }
            }
        }
    }
//     cairo_set_dash (cr, omission_dash, 0, 0);

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
    // y axis

    if (naming_bar_chart->type == 0) {

        for (i = 0; i < ((int)y_segments + 1); i++) {
            cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
            cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
            g_snprintf(buffer, 64, "%1.1f", (double) y_segments- i);// 1.0 - i);// / (double) y_segments);
            cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }
    }
    else {

        for (i = 0; i < ((int)y_segments + 1); i++) {
            cairo_move_to(cr, x_offset + 1.0, y_offset + i*y_space + 0.5);
            cairo_line_to(cr, x_offset - 4.0, y_offset + i*y_space + 0.5);
            g_snprintf(buffer, 64, "%1.1f", (y_segments - i) * max_proportion / (double) y_segments);// 1.0 - i);// / (double) y_segments);
            cairox_text_parameters_set(&p, x_offset - 6.0, y_offset + i*y_space + 0.5, PANGOX_XALIGN_RIGHT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }

    }
    //
    cairo_stroke(cr);
    // x axis

    if (naming_bar_chart->data_type == PATIENTS) {

        for (i = 1; i < ((int)x_segments + 1); i++) { //bar chart - hence diff labels!
            cairo_move_to(cr, x_offset + i*x_space - x_space*0.5, y_axis - 1.0);
            cairo_line_to(cr, x_offset + i*x_space - x_space*0.5, y_axis + 4.0);
            g_snprintf(buffer, 64, "%s", patient_name[i-1]);
            cairox_text_parameters_set(&p, x_offset + i*x_space - x_space*0.5, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }
    }
    else if(naming_bar_chart->data_type == ROGERS) {

        for (i = 1; i < ((int)x_segments + 1); i++) { //bar chart - hence diff labels!
            cairo_move_to(cr, x_offset + i*x_space - x_space*0.5, y_axis - 1.0);
            cairo_line_to(cr, x_offset + i*x_space - x_space*0.5, y_axis + 4.0);
            g_snprintf(buffer, 64, "%1.1f", rogers_lesion_level[i-1]);
            cairox_text_parameters_set(&p, x_offset + i*x_space - x_space*0.5, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }
    }
    else {
        for (i = 1; i < ((int)x_segments + 1); i++) { //bar chart - hence diff labels!
            cairo_move_to(cr, x_offset + i*x_space - x_space*0.5, y_axis - 1.0);
            cairo_line_to(cr, x_offset + i*x_space - x_space*0.5, y_axis + 4.0);
            g_snprintf(buffer, 64, "%1.1f", (i - 1)* 100.0 / (double) naming_lesion_level);
            cairox_text_parameters_set(&p, x_offset + i*x_space - x_space*0.5, y_axis + 5.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
        }

    }



    cairo_stroke(cr);

}

static void draw_naming_legend(cairo_t *cr, GtkNaming_Bar_chart *naming_bar_chart, PangoLayout *layout, CairoxTextParameters p)
{
    // 	//

    double text_width = 0.0;
    pangox_layout_set_font_size(layout, 13);

    pangox_layout_select_font_from_string(layout, "Times");
    if (naming_bar_chart->data_type == MODEL) {

        //draw legend
        if (naming_bar_chart->type == 0) {

            g_snprintf(buffer, 64, "%s", "Drawing:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 2*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);   //use x_offset not x_offset, for legend
            cairox_paint_pango_text(cr, &p, layout, buffer);

            g_snprintf(buffer, 64, "%s", "Delayed Copy:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));
            cairox_text_parameters_set(&p, 2*x_offset, 2.5*y_offset, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);

            cairo_set_source_rgb(cr, 166.0/255.0, 205.0/255.0, 200.0/255.0);
            cairo_move_to(cr, 2.5*x_offset + text_width, 2*y_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 2*y_offset+1);
            cairo_stroke(cr);

            cairo_set_source_rgb(cr, 73.0/255.0,40.0/255.0,68.0/255.0);
            cairo_move_to(cr, 2.5*x_offset + text_width, 2.5*y_offset+1);
            cairo_line_to(cr, 6*x_offset-5, 2.5*y_offset+1);
            cairo_stroke(cr);
        }
        else {// if (naming_bar_chart->type == 1) {

            double y_offset_legend = 40;
            g_snprintf(buffer, 64, "%s", "Animals:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));

            cairox_text_parameters_set(&p, 2*x_offset, 2*y_offset - y_offset_legend, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);


            g_snprintf(buffer, 64, "%s", "Artifacts:");
            text_width = fmax(text_width, pangox_layout_get_string_width(layout, buffer));

            cairox_text_parameters_set(&p, 2*x_offset, 2.5*y_offset - y_offset_legend, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_CENTER, 0.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);

            cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
            cairo_move_to(cr, 2.5*x_offset + text_width, 2*y_offset+1 - y_offset_legend);
            cairo_line_to(cr, 5*x_offset-5, 2*y_offset+1 - y_offset_legend);
            cairo_stroke(cr);

            cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
            cairo_move_to(cr, 2.5*x_offset + text_width, 2.5*y_offset+1 -y_offset_legend);
            cairo_line_to(cr, 5*x_offset-5, 2.5*y_offset+1 - y_offset_legend);
            cairo_stroke(cr);
        }

    }
//       cairo_set_dash (cr, omission_dash, 0, 0);

}

static void draw_naming_axes_labels(cairo_t *cr, GtkNaming_Bar_chart *naming_bar_chart, double width, double height, PangoLayout *layout, CairoxTextParameters p)
{

    if (naming_bar_chart->type == 0)
    {
        g_snprintf(buffer, 64, "Mean Errors Per Drawing");
        cairox_text_parameters_set(&p, x_offset - 33.0, (height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
        cairox_paint_pango_text(cr, &p, layout, buffer);
        cairo_stroke(cr);


    }
    else if (naming_bar_chart->type == 1)
    {
        int i;
        double half_height = height / 2.0;
        for (i = 0; i < 2; i++) {

            g_snprintf(buffer, 64, "%s", y_label[i]);
            cairox_text_parameters_set(&p, x_offset - 33.0, (half_height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_translate(cr, 0, half_height);
        }
        cairo_stroke(cr);

        cairo_translate(cr, 0, -height- y_offset/2.0);
        cairo_stroke(cr);


    }
    else if (naming_bar_chart->type == 2)
    {
        int i;
        double half_height = height / 2.0;
        for (i = 0; i < 2; i++) {

            g_snprintf(buffer, 64, "%s", y_label[2+i]);
            cairox_text_parameters_set(&p, x_offset - 33.0, (half_height - y_offset)/2.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 90.0);
            cairox_paint_pango_text(cr, &p, layout, buffer);
            cairo_translate(cr, 0, half_height);
        }
        cairo_stroke(cr);

        cairo_translate(cr, 0, -height- y_offset/2.0);
        cairo_stroke(cr);


    }


    if (naming_bar_chart->data_type == PATIENTS) {
        g_snprintf(buffer, 64, "Patient");

    }
    else {
        g_snprintf(buffer, 64, "Percent Connections Lesioned");
    }
    cairox_text_parameters_set(&p, (width - x_offset)/2.0, (height - y_offset) + 27.0, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
    cairox_paint_pango_text(cr, &p, layout, buffer);
    cairo_stroke(cr);


    //
}


void gtk_naming_bar_chart_update(GtkNaming_Bar_chart *naming_bar_chart)
{


//     gtk_naming_bar_chart_init(naming_bar_chart);




    // if (naming_bar_chart->type == 0) {
    int s, p, l, d, domain, n_visual_units = pattern_get_n_visual(), n_visual_features;// p is target, q is network's response
    for (l = 0; l < naming_lesion_level; l++) {

        for (domain = 0; domain < N_DOMAIN; domain++) {
            naming_bar_chart->drawing_omission[l][domain] = 0.0;
            naming_bar_chart->drawing_intrusion[l][domain] = 0.0;
            naming_bar_chart->copying_omission[l][domain] = 0.0;
            naming_bar_chart->copying_intrusion[l][domain] = 0.0;

            naming_bar_chart->drawing_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->drawing_intrusion_proportion[l][domain] = 0.0;
            naming_bar_chart->copying_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->copying_intrusion_proportion[l][domain] = 0.0;

            naming_bar_chart->se_drawing_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->se_drawing_intrusion_proportion[l][domain] = 0.0;
            naming_bar_chart->se_copying_omission_proportion[l][domain] = 0.0;
            naming_bar_chart->se_copying_intrusion_proportion[l][domain] = 0.0;

        }
        naming_bar_chart->se_drawing_overall[l] = 0.0;
        naming_bar_chart->se_copying_overall[l] = 0.0;


    }

    if (naming_bar_chart->data_type == MODEL) {


//       d = 0, l = 2, s = 0, p = 18;
//       printf("loaded: drawing_omission[%i][%i][%i][%i] = %i, drawing_intrusion[%i][%i][%i][%i] = %i\n", d, l, s, p, drawing_omission[d][l][s][p], d, l, s, p, drawing_intrusion[d][l][s][p]);
        // 	d = 0, l = 1, s = 0, p = 5;



        for (d = 0; d < naming_lesion_distribution; d++) {
            for (l = 0; l < naming_lesion_level; l++) {
                for (p = 0; p < P; p++) {

                    if (!excluded_pattern(p)) {

                        n_visual_features = get_n_visual_features(p);

                        for (s = 0; s < naming_sample; s++) {


                            domain = get_domain(p);
//                             if (domain == -1)
//                             {
//                                 printf("domain units: %f %f\n", get_pattern(p, i0_of(ENCYCLOPEDIC)+0), get_pattern(p, i0_of(ENCYCLOPEDIC)+1));
//                                 printf("naming_bar_chart->copying_intrusion[%d][%d] = %f\n", l, domain, naming_bar_chart->copying_intrusion[l][domain]);
//
//
//                             }
                            naming_bar_chart->drawing_omission[l][domain] += drawing_omission[d][l][s][p];
                            naming_bar_chart->drawing_intrusion[l][domain] += drawing_intrusion[d][l][s][p];
                            naming_bar_chart->copying_omission[l][domain] += copying_omission[d][l][s][p];
                            naming_bar_chart->copying_intrusion[l][domain] += copying_intrusion[d][l][s][p];




// 			    printf("copying_intrusion[%i][%i][%i][%i] = %f\n", d, l, s, p, copying_intrusion[d][l][s][p]);


                            naming_bar_chart->drawing_omission_proportion[l][domain] += drawing_omission[d][l][s][p] / (double) n_visual_features;
                            naming_bar_chart->drawing_intrusion_proportion[l][domain] += drawing_intrusion[d][l][s][p] / (double) n_visual_features;
                            naming_bar_chart->copying_omission_proportion[l][domain] += copying_omission[d][l][s][p] / (double) n_visual_features;
                            naming_bar_chart->copying_intrusion_proportion[l][domain] += copying_intrusion[d][l][s][p] / (double) n_visual_features;


                        }
                    }
                    // 	  else
                    // 	  {
                    // 	      printf("pattern = %s\n", get_name_string(p));
                    // 	  }
                }
            }
        }


        // for (l = 0; l < naming_lesion_level; l++) {
        //   for (domain = 0; domain < N_DOMAIN; domain++) {
        //     if (l % 2) {
        //       naming_bar_chart->drawing_correct[l][domain] = 10;
        //       naming_bar_chart->drawing_omission[l][domain] = 13;
        //       naming_bar_chart->drawing_intrusion[l][domain] = 40;
        //       naming_bar_chart->copying_correct[l][domain] = 0;
        //       naming_bar_chart->copying_omission[l][domain] = 20;
        //       naming_bar_chart->copying_intrusion[l][domain] = 21;
        //     }
        //     else {
        //       naming_bar_chart->drawing_correct[l][domain] = 10;
        //       naming_bar_chart->drawing_omission[l][domain] = 22;
        //       naming_bar_chart->drawing_intrusion[l][domain] = 15;
        //       naming_bar_chart->copying_correct[l][domain] = 30;
        //       naming_bar_chart->copying_omission[l][domain] = 11;
        //       naming_bar_chart->copying_intrusion[l][domain] = 10;
        //     }
        //   }
        // }


        for (l = 0; l < naming_lesion_level; l++) {
            for (domain = 0; domain < N_DOMAIN; domain++) {
                naming_bar_chart->drawing_omission[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain] * n_visual_units);
                naming_bar_chart->drawing_intrusion[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain] * n_visual_units);
                naming_bar_chart->copying_omission[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain] * n_visual_units);
                naming_bar_chart->copying_intrusion[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain] * n_visual_units);

                naming_bar_chart->drawing_omission_proportion[l][domain]/= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_bar_chart->drawing_intrusion_proportion[l][domain]/= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_bar_chart->copying_omission_proportion[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_bar_chart->copying_intrusion_proportion[l][domain] /= (double) (naming_lesion_distribution * naming_sample * n_pattern[domain]);

// 		if (domain == -1)
// 		{
// 		  printf("domain units: %f %f\n", get_pattern(p, i0_of(ENCYCLOPEDIC)+0), get_pattern(p, i0_of(ENCYCLOPEDIC)+1));
// 		  printf("naming_bar_chart->copying_intrusion[%d][%d] = %f\n", l, domain, naming_bar_chart->copying_intrusion[l][domain]);
//
// 		}
            }


        }


        for (d = 0; d < naming_lesion_distribution; d++) {

            for (l = 0; l < naming_lesion_level; l++) {



                for (p = 0; p < P; p++) {
                    if (!excluded_pattern(p)) {


                        domain = get_domain(p);
                        n_visual_features = get_n_visual_features(p);

                        for (s = 0; s < naming_sample; s++) {


                            naming_bar_chart->se_drawing_omission_proportion[l][domain] += sqr(naming_bar_chart->drawing_omission_proportion[l][domain] - (drawing_omission[d][l][s][p] / (double) n_visual_features));
                            naming_bar_chart->se_drawing_intrusion_proportion[l][domain] += sqr(naming_bar_chart->drawing_intrusion_proportion[l][domain] - (drawing_intrusion[d][l][s][p] / (double) n_visual_features));
                            naming_bar_chart->se_copying_omission_proportion[l][domain] += sqr(naming_bar_chart->copying_omission_proportion[l][domain] - (copying_omission[d][l][s][p] / (double) n_visual_features));
                            naming_bar_chart->se_copying_intrusion_proportion[l][domain] += sqr(naming_bar_chart->copying_intrusion_proportion[l][domain] - (copying_intrusion[d][l][s][p] / (double) n_visual_features));


                            naming_bar_chart->se_drawing_overall[l] += sqr(((naming_bar_chart->drawing_omission[l][0] + naming_bar_chart->drawing_omission[l][1]) / 2.0) - drawing_omission[d][l][s][p]);
                            naming_bar_chart->se_drawing_overall[l] += sqr(((naming_bar_chart->drawing_intrusion[l][0] + naming_bar_chart->drawing_intrusion[l][1]) / 2.0) - drawing_intrusion[d][l][s][p]);
                            naming_bar_chart->se_copying_overall[l] += sqr(((naming_bar_chart->copying_omission[l][0] + naming_bar_chart->copying_omission[l][1]) / 2.0) - copying_omission[d][l][s][p]);
                            naming_bar_chart->se_copying_overall[l] += sqr(((naming_bar_chart->copying_intrusion[l][0] + naming_bar_chart->copying_intrusion[l][1]) / 2.0) - copying_intrusion[d][l][s][p]);
                        }
                    }
                }

            }
        }

        for (l = 0; l < naming_lesion_level; l++) {



            for (domain = 0; domain < N_DOMAIN; domain++) {
                naming_bar_chart->se_drawing_omission_proportion[l][domain] /= (double) ((naming_lesion_distribution * naming_sample * n_pattern[domain]) -1);
                naming_bar_chart->se_drawing_intrusion_proportion[l][domain] /= (double) ((naming_lesion_distribution * naming_sample * n_pattern[domain]) -1);
                naming_bar_chart->se_copying_omission_proportion[l][domain] /= (double) ((naming_lesion_distribution * naming_sample * n_pattern[domain]) -1);
                naming_bar_chart->se_copying_intrusion_proportion[l][domain] /= (double) ((naming_lesion_distribution * naming_sample * n_pattern[domain]) -1);

                naming_bar_chart->se_drawing_omission_proportion[l][domain] = sqrt(naming_bar_chart->se_drawing_omission_proportion[l][domain]) / (double) sqrt(naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_bar_chart->se_drawing_intrusion_proportion[l][domain] = sqrt(naming_bar_chart->se_drawing_intrusion_proportion[l][domain]) / (double) sqrt(naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_bar_chart->se_copying_omission_proportion[l][domain] = sqrt(naming_bar_chart->se_copying_omission_proportion[l][domain]) / (double) sqrt(naming_lesion_distribution * naming_sample * n_pattern[domain]);
                naming_bar_chart->se_copying_intrusion_proportion[l][domain] = sqrt(naming_bar_chart->se_copying_intrusion_proportion[l][domain]) / (double) sqrt(naming_lesion_distribution * naming_sample * n_pattern[domain]);

            }


            naming_bar_chart->se_drawing_overall[l] /= (double) ((naming_lesion_distribution * naming_sample * (n_pattern[0]+n_pattern[1]) * n_visual_units) -1);
            naming_bar_chart->se_copying_overall[l] /= (double) ((naming_lesion_distribution * naming_sample * (n_pattern[0]+n_pattern[1]) * n_visual_units) -1);



            naming_bar_chart->se_drawing_overall[l] = sqrt(naming_bar_chart->se_drawing_overall[l]) / (double) sqrt(naming_lesion_distribution * naming_sample * (n_pattern[0]+n_pattern[1]) * n_visual_units);
            naming_bar_chart->se_copying_overall[l] =  sqrt(naming_bar_chart->se_copying_overall[l]) / (double) sqrt(naming_lesion_distribution * naming_sample * (n_pattern[0]+n_pattern[1]) * n_visual_units);


        }

    }
    else if (naming_bar_chart->data_type == PATIENTS) {
        mean_delayed_copy_errors[0] = 1.9982;
        mean_delayed_copy_errors[1] = 4.6419;
        mean_delayed_copy_errors[2] = 6.7071;
        naming_bar_chart->se_copying_overall[0] = fabs(2.0342 - 2.4628);
        naming_bar_chart->se_copying_overall[1] = fabs(4.6596 - 5.1637);
        naming_bar_chart->se_copying_overall[2] = fabs(6.7303 - 7.2093);


        mean_drawing_errors[0] = 3.9693;
        mean_drawing_errors[1] = 6.0847;
        mean_drawing_errors[2] = 9.6592;
        naming_bar_chart->se_drawing_overall[0] = fabs(3.9388 - 4.3927);
        naming_bar_chart->se_drawing_overall[1] = fabs(6.0851 - 6.7406);
        naming_bar_chart->se_drawing_overall[2] = fabs(9.6936 - 10.1474);





    }
    else if (naming_bar_chart->data_type == ROGERS) {
        mean_delayed_copy_errors[0] = 3.7895;
        mean_delayed_copy_errors[1] = 5.1789;
        mean_delayed_copy_errors[2] = 6.2653;
        naming_bar_chart->se_copying_overall[0] = 0;
        naming_bar_chart->se_copying_overall[1] = 0;
        naming_bar_chart->se_copying_overall[2] = 0;
        naming_bar_chart->se_drawing_overall[0] = 0;
        naming_bar_chart->se_drawing_overall[1] = 0;
        naming_bar_chart->se_drawing_overall[2] = 0;





        mean_drawing_errors[0] = 5.1537;
        mean_drawing_errors[1] = 6.3411;
        mean_drawing_errors[2] = 7.9074;



    }




}
static void draw_naming_bar_chart(cairo_t *cr, GtkWidget *widget, double width, double height)
{


//     double width = (double) widget->allocation.width, height = (double) widget->allocation.height;
    GtkNaming_Bar_chart *naming_bar_chart =  GTK_NAMING_BAR_CHART(widget);
    PangoLayout *layout = pango_cairo_create_layout(cr);
    CairoxTextParameters p;


    x_offset = (naming_bar_chart->type) ? 40.0 : 40.0;
    y_offset = (naming_bar_chart->type) ? 40.0 : 40.0;


    draw_naming_legend(cr, GTK_NAMING_BAR_CHART(widget), layout, p);

    draw_naming_axes_labels(cr, naming_bar_chart, width, height, layout, p);


    if (naming_bar_chart->type == 0) {
        draw_naming_subbar_chart(cr, naming_bar_chart, width, height, layout, p, -1);
    }
    else
    {
        int i;
        double half_height = height / 2.0;
        for (i = 0; i < 2; i++) {

            draw_naming_subbar_chart(cr, naming_bar_chart, width, half_height, layout, p, i);
            cairo_translate(cr, 0, half_height);
        }

    }
    g_object_unref(layout);
    cairo_destroy(cr);

}
void gtk_naming_bar_chart_pdf(GtkWidget *widget, char *filename)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = 500;//widget->allocation.width;
    int height = 500;//widget->allocation.height;
    char name[100];
    int i = 0;

    GtkNaming_Bar_chart *naming_bar_chart = GTK_NAMING_BAR_CHART(widget);


    if (naming_bar_chart->data_type == MODEL) {
        while (filename[i] != '\0') {
            name[i] = filename[i];
            i++;
        }
        name[i] = '\0';
        printf("%s\n", name);
        sprintf(name, "%s_naming_%d.pdf", name, naming_bar_chart->type);
    }
    else if (naming_bar_chart->data_type == ROGERS) {
        sprintf(name, "rogers_model_naming_%d.pdf", naming_bar_chart->type);

    }
    else if (naming_bar_chart->data_type == PATIENTS) {
        sprintf(name, "rogers_patients_naming_%d.pdf", naming_bar_chart->type);

    }



    surface = cairo_pdf_surface_create(name, width, height);
    cr = cairo_create(surface);

    // draw naming_bar_chart
    draw_naming_bar_chart(cr, widget, width, height);


    cairo_surface_destroy(surface);

}


static void gtk_naming_bar_chart_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    draw_naming_bar_chart(cr, widget, (double) widget->allocation.width, (double) widget->allocation.height);
}

void gtk_naming_bar_chart_reset(GtkNaming_Bar_chart *naming_bar_chart)
{
    gtk_naming_bar_chart_init(naming_bar_chart);
    gtk_naming_bar_chart_redraw(GTK_WIDGET(naming_bar_chart));
}

static void gtk_naming_bar_chart_destroy(GtkObject *object)
{
//     GtkNaming_Bar_chart *naming_bar_chart;
    GtkNaming_Bar_chartClass *klass;

    g_return_if_fail(object != NULL);
    g_return_if_fail(GTK_IS_NAMING_BAR_CHART(object));

//     naming_bar_chart = GTK_NAMING_BAR_CHART(object);

    klass = gtk_type_class(gtk_widget_get_type());

    if (GTK_OBJECT_CLASS(klass)->destroy)
    {
        (*GTK_OBJECT_CLASS(klass)->destroy) (object);
    }
}

void gtk_naming_bar_chart_redraw(GtkWidget *widget)
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


void gtk_naming_bar_chart_clear(GtkWidget *widget)
{
    init_naming_response(0, 0, 0);
    gtk_naming_bar_chart_reset(GTK_NAMING_BAR_CHART(widget));

}
