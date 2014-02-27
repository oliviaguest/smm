#include "unrolled_network_viewer.h"

extern void cairox_select_colour_scale (cairo_t * cr, double s);

static void gtk_unrolled_network_viewer_class_init (GtkUnrolled_network_viewerClass *klass);
GtkType gtk_unrolled_network_viewer_get_type (void);
static void gtk_unrolled_network_viewer_init (GtkUnrolled_network_viewer *unrolled_network_viewer);
static void gtk_unrolled_network_viewer_size_request (GtkWidget * widget, GtkRequisition *requisition);
static void gtk_unrolled_network_viewer_size_allocate (GtkWidget * widget, GtkAllocation *allocation);
static void gtk_unrolled_network_viewer_realize (GtkWidget * widget);
static gboolean gtk_unrolled_network_viewer_expose (GtkWidget * widget, GdkEventExpose * event);
static void gtk_unrolled_network_viewer_paint (GtkWidget * widget);
static void gtk_unrolled_network_viewer_destroy (GtkObject * object);
void cairox_select_colour_scale (cairo_t * cr, double d);
void draw_unrolled_network_viewer(cairo_t *cr, GtkWidget *widget, double width, double height);
// static int W = 1020;
// static int H = 650;

int unit_width = 5;
int unit_height = 10;
int unit_x[PDPTOOL_UNITS];
int unit_y[PDPTOOL_UNITS];

#define N network_get_N(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type))
#define UNITS network_get_UNITS(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type))

GtkType gtk_unrolled_network_viewer_get_type (void)
{
    static GtkType gtk_unrolled_network_viewer_type = 0;
    if (!gtk_unrolled_network_viewer_type)
    {
        static const GtkTypeInfo gtk_unrolled_network_viewer_info =
        {   "GtkUnrolled_network_viewer", sizeof (GtkUnrolled_network_viewer),
            sizeof (GtkUnrolled_network_viewerClass), (GtkClassInitFunc) gtk_unrolled_network_viewer_class_init,
            (GtkObjectInitFunc) gtk_unrolled_network_viewer_init, NULL, NULL, (GtkClassInitFunc) NULL
        };
        gtk_unrolled_network_viewer_type =
            gtk_type_unique (GTK_TYPE_WIDGET, &gtk_unrolled_network_viewer_info);
    }
    return gtk_unrolled_network_viewer_type;
}

GtkWidget *gtk_unrolled_network_viewer_new(void)
{
    return GTK_WIDGET(gtk_type_new (gtk_unrolled_network_viewer_get_type ()));
}


static void gtk_unrolled_network_viewer_class_init (GtkUnrolled_network_viewerClass *
        klass)
{
    GtkWidgetClass *widget_class;
    GtkObjectClass *object_class;

    widget_class = (GtkWidgetClass *) klass;
    object_class = (GtkObjectClass *) klass;

    widget_class->realize = gtk_unrolled_network_viewer_realize;
    widget_class->size_request = gtk_unrolled_network_viewer_size_request;
    widget_class->size_allocate = gtk_unrolled_network_viewer_size_allocate;
    widget_class->expose_event = gtk_unrolled_network_viewer_expose;
    object_class->destroy = gtk_unrolled_network_viewer_destroy;



}

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    double coordx = 0, coordy = 0;

    gtk_unrolled_network_viewer_redraw(widget);



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



    cr = gdk_cairo_create (widget->window);

    /* Transparent background */
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint (cr);
    layout = pango_cairo_create_layout (cr);

    for (i = 0; i < UNITS; i++) {
        if (((int)coordx >= unit_x[i]) && ((int)coordx < (unit_x[i]+ unit_width)) &&
                ((int)coordy >= unit_y[i]) && ((int)coordy < (unit_y[i]+ unit_height))) {
            // then we are in a unit, so must print it's state
            printf("unit[%d] = %f\n", i, network_state_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));
            printf("x = %f, y = %f\n", coordx, coordy);
            g_snprintf (buffer, 64, "%1.1f", network_state_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));
            cairox_text_parameters_set (&text_p, unit_x[i] + unit_width*0.5 , unit_y[i] + unit_height*1.25, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_TOP, 0.0);
            pangox_layout_set_font_size(layout, 10.0);

            cairox_paint_pango_text (cr, &text_p, layout, buffer);
            break;

        }
    }



    g_object_unref (layout);
    cairo_destroy (cr);

    //     time_t t1,t2;
    //
    //   time(&t1);
    //
    //
    //
    //    time(&t2);
    //
    //     while ((t2-t1) != (time_t)1) {
    //          time(&t2);
    //
    // //       printf("beep");
    //       gtkx_flush_events();
    //     }
    return TRUE;
}

// gboolean hover(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
// {
//     if (event->button == 1) {
//         coordx = (int) event->x;
//         coordy = (int) event->y;
//     }
//     printf("x = %d y = %d\n", coordx, coordy);

//     if (event->button == 3) {
//         gtk_widget_queue_draw(widget);
//     }

//     PangoLayout *layout;
//     CairoxTextParameters text_p;
//     char buffer[64];
//
//     cairo_t *cr;
//     int i;
//
//
//
//     cr = gdk_cairo_create (widget->window);
//
//     /* Transparent background */
//     cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0);
//     cairo_paint (cr);
//     layout = pango_cairo_create_layout (cr);
//
//     for (i = 0; i < UNITS; i++) {
//       if ((coordx >= unit_x[i]) && (coordx < (unit_x[i]+ unit_width)) &&
// 	  (coordy >= unit_y[i]) && (coordy < (unit_y[i]+ unit_height))) {
// 	    // then we are in a unit, so must print it's state
// 		printf("unit[%d] = %f\n", i, network_state_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));
// 		printf("x = %f, y = %f\n", coordx, coordy);
//
// 		g_snprintf (buffer, 64, "State: %1.1f", network_state_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));
// 		cairox_text_parameters_set (&text_p, unit_x[i] + unit_width*0.5 , unit_y[i] - unit_height*2, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
// 		cairox_paint_pango_text (cr, &text_p, layout, buffer);
// 		break;
//
// 	  }
//     }
//
//   time_t t1,t2;
//
//   time(&t1);
//
//
//
//    time(&t2);
//
//     while ((t2-t1) != (time_t)10) {
//          time(&t2);
//
//       printf("beep");
//     }
//
//
//    g_object_unref (layout);
//     cairo_destroy (cr);
//     return TRUE;
// }
static void gtk_unrolled_network_viewer_init (GtkUnrolled_network_viewer *unrolled_network_viewer)
{
//     GtkWidget *fixed =    gtk_fixed_new();
    gtk_widget_add_events(GTK_WIDGET(unrolled_network_viewer), GDK_BUTTON_PRESS_MASK);
    //     gtk_widget_add_events(GTK_WIDGET(unrolled_network_viewer), GDK_ENTER_NOTIFY_MASK);
    g_signal_connect(G_OBJECT(unrolled_network_viewer), "button-press-event", G_CALLBACK(clicked), NULL);
    //         g_signal_connect(G_OBJECT(unrolled_network_viewer), "enter-notify-event", G_CALLBACK(hover), NULL);

    unrolled_network_viewer->type_of_view = ROLLED_STATES;
    unrolled_network_viewer->network_type = NULL;
    unrolled_network_viewer->error_type = 0;
//     int p;
//     for (p = 0; p < P_MAX; p++) {
//         unrolled_network_viewer->check_button[p] = NULL;
//     }
//     unrolled_network_viewer->fixed = gtk_fixed_new();
//     gtk_fixed_put(GTK_FIXED(unrolled_network_viewer->fixed), GTK_WIDGET(unrolled_network_viewer), 0, 0);
//     gtk_widget_show(unrolled_network_viewer->fixed);

}

static void gtk_unrolled_network_viewer_size_request(GtkWidget *widget, GtkRequisition *requisition)
/// "Obtains the preferred size of a widget."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_UNROLLED_NETWORK_VIEWER (widget));
    g_return_if_fail (requisition != NULL);
    requisition->width = 400;
    requisition->height = 600;

    //     printf("%s, %d %d\n", __FUNCTION__, requisition->width, requisition->height);

}

static void gtk_unrolled_network_viewer_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
/// "[...] assign a size and position to their child widgets."
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_UNROLLED_NETWORK_VIEWER (widget));
    g_return_if_fail (allocation != NULL);
    //     allocation->width = 400;
    //     allocation->height = 400;
    widget->allocation = *allocation;


    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize(widget->window, allocation->x, allocation->y,
                               allocation->width, allocation->height);
    }

    //     printf("%s, %d %d\n", __FUNCTION__, allocation->width, allocation->height);
}

static void
gtk_unrolled_network_viewer_realize (GtkWidget * widget)
{
    GdkWindowAttr attributes;
    guint attributes_mask;


    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_UNROLLED_NETWORK_VIEWER (widget));

    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;	//W;
    attributes.height = widget->allocation.height;	//H;


    //     printf("%s, %d %d\n", __FUNCTION__, attributes.width, attributes.height);



    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
    attributes_mask = GDK_WA_X | GDK_WA_Y;

    widget->window =
        gdk_window_new (gtk_widget_get_parent_window (widget), &attributes,
                        attributes_mask);

    gdk_window_set_user_data (widget->window, widget);
    widget->style = gtk_style_attach (widget->style, widget->window);
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

}

static gboolean
gtk_unrolled_network_viewer_expose (GtkWidget * widget,
                                    GdkEventExpose * event)
{
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_UNROLLED_NETWORK_VIEWER (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    gtk_unrolled_network_viewer_paint (widget);

    return FALSE;
}

void
cairox_select_colour_unit (cairo_t * cr, int free)
{
    double r = 0.0, g = 0.0, b = 0.0;

    if (!free)
    {
        r = 1.0, g = 1.0, b = 1.0;
    }
    cairo_set_source_rgb (cr, r, g, b);
}

void calculate_delta_min_max (GtkUnrolled_network_viewer *widget, double *min, double *max)
{
    int i = 0, t = 0;
    double temp = bptt_get_delta (i, t);
    *min = bptt_get_delta (i, t);
    *max = *min;
    for (t = 0; t < BPTT_T; t++)
    {
        for (i = 0; i < UNITS; i++)
        {
            temp = bptt_get_delta (i, t);
            if (temp > *max)
            {
                *max = temp;
            }
            else if (temp < *min)
            {
                *min = temp;

            }
        }
    }
}

void calculate_error_min_max (GtkUnrolled_network_viewer *widget, double *min, double *max)
{
    int i = 0, t = 0;
    double temp = bptt_get_epsilon (i, t);
    *min = bptt_get_epsilon (i, t);
    *max = *min;
    for (t = 0; t < BPTT_T; t++)
    {
        for (i = 0; i < N; i++)
        {
            temp = bptt_get_epsilon (i, t);
            if (temp > *max)
            {
                *max = temp;
            }
            else if (temp < *min)
            {
                *min = temp;

            }
        }
    }
}


static void draw_unit(GtkUnrolled_network_viewer * widget, cairo_t * cr, double x, double y, int i, int t, int type)
{
    double value = 0.0, range = 0.0;

    cairo_rectangle (cr, x, y, unit_width, unit_height);
    unit_x[i] = x;
    unit_y[i] = y;

    if (type == DELTAS)
    {
        if (deltas_updated)
        {
            calculate_delta_min_max (widget, &(widget->delta_min),
                                     &(widget->delta_max));
            deltas_updated = 0;
        }
        if (range)
        {
            value = (bptt_get_delta (i, t) + fabs (widget->delta_min)) / range;
        }
        else
        {
            value = (bptt_get_delta (i, t) + 1) * 0.5;
        }
    }
    else if (type == ERRORS)
    {
        if (in_range (i, N))
        {
            if (errors_updated)
            {
                calculate_error_min_max (widget, &(widget->error_min),
                                         &(widget->error_max));
                errors_updated = 0;
            }
            range = widget->error_max - widget->error_min;
            if (range)
            {
                value = (bptt_get_epsilon (i, t) + fabs (widget->error_min)) / range;
            }
            else
            {
                value = bptt_get_epsilon (i, t);
            }
        }
    }
    //     else if (type == CLAMPED_PATTERN)
    //     {
    //         if (in_range (i, N))
    //         {
    //             if (get_clamped_pattern() > -1)
    //             {
    //                 value = get_pattern (get_clamped_pattern(), i);
    //             }
    //         }
    //     }
    else if (type == PATTERNS)
    {

//       printf("N = %d, P = %d\n", N, P);
        if (in_range(i, network_input_units_n_get(*(widget->network_type))) && in_range(t, P)) {
            value = pattern_get(t, i);
            // 	  value = pdptool_get_pattern(t, pdptool_stage_to_type(pdptool_get_clamped_stage()), i);
            //           printf("value  = %f\n", value);

        }
    }
    //     else if
    //     {
    // //         if (in_range (i, PDPTOOL_N))
    //         if (in_range (i, N))
    //
    //         {
    //             value = pdptool_get_pattern (t, i);
    // // 	  printf("get_pattern(%i, %i) = %f\n", t, i, value);
    // // 	  if (isnan(value)) {exit(0);}
    //
    //         }
    //     }

    else if (type == UNROLLED_STATES) {
        value = network_state_timestep_get(*(widget->network_type), i, t);
    }
    else if (type == ROLLED_STATES) {
        value = network_state_get(*(widget->network_type), i);
//         printf("state(%i) = %f\n", i, value);

    }
    //     else if (type == PDPTOOL_STATES)
    //     {
    //       value = network_state_get(*(widget->network_type), i);
    //
    //     }
    //     else
    //     {
    //       value = network_state_get(*(widget->network_type), i);
    //
    //     }

    cairox_select_colour_scale (cr, 1.0 - value);
    cairo_fill_preserve (cr);

    // Colouring the outline. Black means free, white means clamped.
    if (type == PATTERNS)
    {

//         printf("N = %d, P = %d\n", N, P);
        if (in_range(i, N) && in_range(t, P)) {
            cairox_select_colour_unit (cr, !((network_clamped_pattern_get(*(widget->network_type)) == t) && (network_is_state_clamped(*(widget->network_type), i))));
        }
    }
    else
    {
//         printf("N = %d, P = %d\n", N, P);
        if (in_range(i, UNITS)) {

            cairox_select_colour_unit(cr, !network_is_state_clamped(*(widget->network_type), i));
        }

    }
    //     else if (type == BM_PATTERN)
    //     {
    //
    //         cairox_select_colour_unit (cr, ((bm_get_clamped_pattern() == t) && (bm_clamped_index(i))) ? (0) : (1));
    //
    //     }
    //     else if (type == PDPTOOL_PATTERN)
    //     {
    //         cairox_select_colour_unit (cr, ((pdptool_get_clamped_pattern() == t) && (pdptool_is_clamped(i))) ? (0) : (1));
    //
    //     }
    //     else if (type == PDPTOOL_STATES)
    //     {
    //         cairox_select_colour_unit (cr, !pdptool_is_clamped(i));
    //
    //     }
    //     else if (type == CLAMPED_PATTERN) {
    //         cairox_select_colour_unit (cr, !is_clamped (i, bptt_t1));
    // //         printf("%i\n", !is_clamped (i, bptt_t1));
    //     }
    //     else if (type == BM_STATES) {
    //         cairox_select_colour_unit (cr, !bm_clamped_index(i));
    //
    //     }
    //     else {
    //         cairox_select_colour_unit (cr, !is_clamped (i, t));
    //     }

    cairo_set_line_width (cr, 1.5);
    cairo_stroke (cr);
}


static void gtk_unrolled_network_viewer_paint(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);

    draw_unrolled_network_viewer(cr, widget, (double) widget->allocation.width, (double) widget->allocation.height);
}

// static void set_pattern(GtkToggleButton *togglebutton, gpointer user_data)
// {
//   gtk_toggle_button_get_active ()

// }
void draw_unrolled_network_viewer(cairo_t *cr, GtkWidget *widget, double width, double height)
{


    if (GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type == NULL) return;

//     int width, height;
    CairoxTextParameters text_p;


//     GtkWidget *fixed;
    //     GtkWidget *check_button[P_MAX];
//     GtkWidget *check_button;

    char buffer[64];
    double x = 0.0, y = 0.0;
//     cairo_t *cr= gdk_cairo_create (widget->window);
    int i, t, p;

//     check_button = gtk_check_button_new();
//     gtk_widget_show(check_button);


//       for (p = 0; p < P; p++) {

//       check_button[p] = gtk_check_button_new();
//       gtk_widget_show(check_button[p]);



//       g_signal_connect(G_OBJECT(check_button[p]), "toggled", G_CALLBACK(set_pattern), NULL);

//     }
//     width = widget->allocation.width;
//     height = widget->allocation.height;


    int input_units = network_input_units_n_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type));

    PangoLayout *layout = pango_cairo_create_layout (cr);

    /* Transparent background */
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0);
    cairo_paint (cr);

//     printf("UNITS %d\n", UNITS);

    double x_space = (width - X_MARGIN * 2.0) / (double) (UNITS);
    unit_width = x_space;

//     fixed = gtk_fixed_new();

//     GtkWidget *box = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
//     if (box != NULL)
//     {
//         gtk_box_pack_start(GTK_BOX(box), fixed, 0, 0, 0);
//
//     }
//
//     gtk_widget_show(fixed);




    if (GTK_UNROLLED_NETWORK_VIEWER(widget)->type_of_view == ROLLED_STATES)
    {

//         printf("input %d patterns %d\n", inp/*u*/t_units, P);

        for (i = 0; i < UNITS; i++)
        {
//             printf("%d\n", i);

            p = network_clamped_pattern_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type));
            //             x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (UNITS - 1.0);
            x = X_MARGIN + x_space * i;
            y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * 0.45;

            if (in_range(i, input_units) && in_range(p, P)) {

                draw_unit(GTK_UNROLLED_NETWORK_VIEWER(widget), cr, x, y, i, p, PATTERNS);
            }

            if (in_range(i-input_units, input_units) && in_range(p, P)) {

                draw_unit(GTK_UNROLLED_NETWORK_VIEWER(widget), cr, x, y, i-input_units, p, PATTERNS);
            }

            //             x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (UNITS - 1.0);
            x = X_MARGIN + x_space * i;

            y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * 0.5;
            draw_unit(GTK_UNROLLED_NETWORK_VIEWER(widget), cr, x, y, i, bptt_t1, ROLLED_STATES);
        }
    } else if (GTK_UNROLLED_NETWORK_VIEWER(widget)->type_of_view == PRETTY_STATES)
    {
        double radius = 8.0;
        x_space = (width - X_MARGIN) / (double) (pattern_get_n_visual());
        for (i = 0; i < UNITS; i++)
        {

            p = network_clamped_pattern_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type));



            if (in_range(i, N) && in_range(p, P)) {
                //             x = X_MARGIN + (i - i0_of(type_of(i))) * radius + (width - X_MARGIN - X_MARGIN) * (i - i0_of(type_of(i))) /  (double) N_VISUAL;
                x = (i - i0_of(type_of(i)) + 1.0) * x_space;
                y = Y_MARGIN + (height - 4.0*Y_MARGIN) * ((type_of(i)+1)/6.0) - radius*2.0;
                cairox_select_colour_scale (cr, 1.0 - pattern_get(p, i));

                cairo_arc(cr, x, y, radius, 0.0, 2*M_PI);

                cairo_fill_preserve(cr);
                cairox_select_colour_unit (cr, !network_is_state_clamped(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));

                cairo_set_line_width (cr, 1.5);
                cairo_stroke (cr);

            }



            //             x = X_MARGIN + (i - i0_of(type_of(i))) * radius + (width - X_MARGIN - X_MARGIN) * (i - i0_of(type_of(i))) / (double) N_VISUAL;
            x = (i - i0_of(type_of(i)) + 1) * x_space;

            y = Y_MARGIN + (height - 4.0*Y_MARGIN) * ((type_of(i)+1)/6.0);
            cairox_select_colour_scale (cr, 1.0 - network_state_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));
            cairo_arc(cr, x, y, radius, 0.0, 2*M_PI);

            cairo_fill_preserve(cr);
            cairox_select_colour_unit (cr, !network_is_state_clamped(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));

            cairo_set_line_width (cr, 1.5);
            cairo_stroke (cr);


            //                 printf("unit[%d] = %f\n", i, network_state_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));
            //             printf("x = %f, y = %f\n", coordx, coordy);
            g_snprintf (buffer, 64, "%1.1f", network_state_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type), i));
            cairox_text_parameters_set (&text_p, x , y, PANGOX_XALIGN_CENTER, PANGOX_YALIGN_CENTER, 0.0);
            pangox_layout_set_font_size(layout, radius);
            cairox_paint_pango_text (cr, &text_p, layout, buffer);
            pangox_layout_set_font_size(layout, 15.0);


        }
    }
    //     else if (GTK_UNROLLED_NETWORK_VIEWER (widget)->type_of_view == PDPTOOL_STATES)
    //     {
    //
    //         for (i = 0; i < UNITS; i++)
    //         {
    //
    //             p = pdptool_get_clamped_pattern();
    //
    //             x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (UNITS - 1.0);
    //             y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * 0.45;
    //             if (p == -1) {
    //                 p = 0;
    //             }
    //             draw_unit (GTK_UNROLLED_NETWORK_VIEWER (widget), cr, x, y, i, p, PDPTOOL_PATTERN);
    //
    //             x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (UNITS - 1.0);
    //             y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * 0.5;
    //             draw_unit (GTK_UNROLLED_NETWORK_VIEWER (widget), cr, x, y, i, bptt_t1, PDPTOOL_STATES);
    //         }
    //     }
    //     else if (GTK_UNROLLED_NETWORK_VIEWER (widget)->type_of_view == BM_STATES)
    //     {
    //         // Draw target states and network states.
    //         for (i = 0; i < BM_UNITS; i++)
    //         {
    //             p = bm_get_clamped_pattern();
    //
    //             x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (BM_UNITS - 1.0);
    //             y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * 0.45;
    //             if (p == -1) {
    //                 p= 0;
    //             }
    //             draw_unit (GTK_UNROLLED_NETWORK_VIEWER (widget), cr, x, y, i, p, BM_PATTERN);
    //             x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (BM_UNITS - 1.0);
    //             y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * 0.5;
    //             draw_unit (GTK_UNROLLED_NETWORK_VIEWER (widget), cr, x, y, i, bptt_t1, BM_STATES);
    //         }
    //     }

    else if (GTK_UNROLLED_NETWORK_VIEWER(widget)->type_of_view == PATTERNS)
    {
        printf("input %d patterns %d\n", input_units, P);

        x_space = (width - X_MARGIN * 2.0 - unit_width) / (double) (input_units);

        x = 0;
// 	x = X_MARGIN + (x_space / 2.0);
// 	y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * (1/ (double) P);
// 	gtk_fixed_put(GTK_FIXED(fixed), check_button, x, y);
// 	gtk_widget_set_size_request(check_button, 20, 20);

        for (p = 0; p < P; p++)
        {
            y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * ((P - p) / (double) P);

// 	  gtk_fixed_put(GTK_FIXED(GTK_UNROLLED_NETWORK_VIEWER(widget)->fixed), GTK_UNROLLED_NETWORK_VIEWER(widget)->check_button[p], 600, 0);
// 	  gtk_widget_set_size_request(GTK_UNROLLED_NETWORK_VIEWER(widget)->check_button[p], 40, 20);

// 	  gtk_widget_show(GTK_UNROLLED_NETWORK_VIEWER(widget)->check_button[p]);



            for (i = 0; i < input_units; i++)
            {
                x = X_MARGIN + x_space * i;

                draw_unit(GTK_UNROLLED_NETWORK_VIEWER(widget), cr, x, y, i, (P - 1) - p, PATTERNS);
            }
        }
    }

    //     else if (GTK_UNROLLED_NETWORK_VIEWER (widget)->type_of_view == BM_PATTERN)
    //     {
    //         for (p = 0; p < P; p++)
    //         {
    //             for (i = 0; i < N; i++)
    //             {
    //                 x =
    //                     X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (N - 1.0);
    //                 y =
    //                     Y_MARGIN + (height - 25 - Y_MARGIN -
    //                                 Y_MARGIN) * ((P - p) / (double) P);
    //                 draw_unit (GTK_UNROLLED_NETWORK_VIEWER (widget), cr, x, y, i,
    //                            (P - 1) - p, GTK_UNROLLED_NETWORK_VIEWER (widget)->type_of_view);
    //             }
    //         }
    //     }
    //     else if (GTK_UNROLLED_NETWORK_VIEWER (widget)->type_of_view == PDPTOOL_PATTERN)
    //     {
    //         for (p = 0; p < P; p++)
    //         {
    // //             for (i = 0; i < PDPTOOL_N; i++)
    //             for (i = 0; i < N; i++)
    //
    //             {
    // //                 x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (PDPTOOL_N - 1.0);
    //                 x = X_MARGIN + (width - X_MARGIN - X_MARGIN) * i / (N - 1.0);
    //
    //                 y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * ((P - p) / (double) P);
    //                 draw_unit (GTK_UNROLLED_NETWORK_VIEWER (widget), cr, x, y, i, (P - 1) - p, GTK_UNROLLED_NETWORK_VIEWER (widget)->type_of_view);
    //             }
    //         }
    //     }
    //
    else if (GTK_UNROLLED_NETWORK_VIEWER(widget)->type_of_view == UNROLLED_STATES)
    {
        x_space = (width - X_MARGIN * 2.0 - unit_width) / (double) (UNITS);


        for (t = 0; t < network_n_timesteps_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type)); t++)
        {
            for (i = 0; i < UNITS; i++)
            {
                x = X_MARGIN + x_space * i;
                y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * (( network_n_timesteps_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type))- t) / (double) network_n_timesteps_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type)));
                draw_unit(GTK_UNROLLED_NETWORK_VIEWER(widget), cr, x, y, i, t, UNROLLED_STATES);
            }
        }
        t = network_n_timesteps_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type));
        p = network_clamped_pattern_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type));
        for (i = 0; i < UNITS; i++)
        {
            x = X_MARGIN + x_space * i;
            y = Y_MARGIN + (height - 25 - Y_MARGIN - Y_MARGIN) * ((network_n_timesteps_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type))- t) / (double)network_n_timesteps_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type)));
            if (in_range(i, N) && in_range(t, P)) {
                draw_unit(GTK_UNROLLED_NETWORK_VIEWER(widget), cr, x, y, i, p, PATTERNS);
            }
        }
    }


    // The following code draws the details printed under the network states, like error and so on.
    //      if (!training) {
    //         g_snprintf(buffer, 64, "Step: %d", step_number);
    //         cairox_text_parameters_set(&text_p, 20, height-20, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    //         cairox_paint_pango_text(cr, &text_p, layout, buffer);

    if ((GTK_UNROLLED_NETWORK_VIEWER(widget)->type_of_view == UNROLLED_STATES) ||
            (GTK_UNROLLED_NETWORK_VIEWER(widget)->type_of_view == ROLLED_STATES) ||
            (GTK_UNROLLED_NETWORK_VIEWER(widget)->type_of_view ==  PATTERNS)) {

        if (network_clamped_pattern_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type)) != -1)
        {
            g_snprintf (buffer, 64, "Pattern/Stage: %d/%d", network_clamped_pattern_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type)), network_clamped_stage_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type)));
            cairox_text_parameters_set (&text_p, 20, height - 20, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text (cr, &text_p, layout, buffer);

            g_snprintf (buffer, 64, "Name: %s", get_name_string(network_clamped_pattern_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type))));
            cairox_text_parameters_set (&text_p, 220, height - 20, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text (cr, &text_p, layout, buffer);
            g_snprintf(buffer, 64, "Error: %f%%", 100 *

                       network_error_measure(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type),
                                             *(GTK_UNROLLED_NETWORK_VIEWER(widget)->error_type),
                                             network_clamped_pattern_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type)),
                                             network_clamped_type_get(*(GTK_UNROLLED_NETWORK_VIEWER(widget)->network_type))
                                            )
                      );

            cairox_text_parameters_set (&text_p, 520, height - 20, PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
            cairox_paint_pango_text (cr, &text_p, layout, buffer);

        }
    }
    //     if (PDPTOOL_STATES || PDPTOOL_PATTERN) {
    //
    //         if (pdptool_get_clamped_pattern () != -1)
    //         {
    //             g_snprintf (buffer, 64, "Pattern/Stage: %d/%d", pdptool_get_clamped_pattern (),
    //                         pdptool_get_clamped_stage ());
    //             cairox_text_parameters_set (&text_p, 20, height - 20,
    //                                         PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    //             cairox_paint_pango_text (cr, &text_p, layout, buffer);
    //
    //             g_snprintf (buffer, 64, "Name: %s",
    //                         get_name_string (pdptool_get_clamped_pattern ()));
    //             cairox_text_parameters_set (&text_p, 220, height - 20,
    //                                         PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    //             cairox_paint_pango_text (cr, &text_p, layout, buffer);
    //             //      }
    //
    //             printf("pdptool\n");
    //
    // // if unrolled network viewer uses generic network functions instead there will not be an issue. So I needf to send a pointer to network_type to it.
    //             g_snprintf (buffer, 64, "Error: %f%%",
    //                         100 * pdptool_calculate_rms_error (pdptool_get_clamped_pattern ()));
    //             cairox_text_parameters_set (&text_p, 520, height - 20,
    //                                         PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    //             cairox_paint_pango_text (cr, &text_p, layout, buffer);
    //         }
    //     }
    //     else {
    //         if (get_clamped_pattern () != -1)
    //         {
    //             g_snprintf (buffer, 64, "Pattern/Stage: %d/%d", get_clamped_pattern (),
    //                         get_clamped_stage ());
    //             cairox_text_parameters_set (&text_p, 20, height - 20,
    //                                         PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    //             cairox_paint_pango_text (cr, &text_p, layout, buffer);
    //
    //             g_snprintf (buffer, 64, "Name: %s",
    //                         get_name_string (get_clamped_pattern ()));
    //             cairox_text_parameters_set (&text_p, 220, height - 20,
    //                                         PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    //             cairox_paint_pango_text (cr, &text_p, layout, buffer);
    //             //      }
    //
    //
    //             g_snprintf (buffer, 64, "Error: %f%%",
    //                         100 * calculate_rms_error (get_clamped_pattern ()));
    //             cairox_text_parameters_set (&text_p, 520, height - 20,
    //                                         PANGOX_XALIGN_LEFT, PANGOX_YALIGN_TOP, 0.0);
    //             cairox_paint_pango_text (cr, &text_p, layout, buffer);
    //         }
    //     }

    g_object_unref (layout);

    cairo_destroy (cr);
}

static void
gtk_unrolled_network_viewer_destroy (GtkObject * object)
{
    //     GtkUnrolled_network_viewer *unrolled_network_viewer;
    GtkUnrolled_network_viewerClass *klass;

    g_return_if_fail (object != NULL);
    g_return_if_fail (GTK_IS_UNROLLED_NETWORK_VIEWER (object));

    //     unrolled_network_viewer = GTK_UNROLLED_NETWORK_VIEWER (object);

    klass = gtk_type_class (gtk_widget_get_type ());

    if (GTK_OBJECT_CLASS (klass)->destroy)
    {
        (*GTK_OBJECT_CLASS (klass)->destroy) (object);
    }
}

void
gtk_unrolled_network_viewer_redraw (GtkWidget * widget)
{
    //      GtkWidget *widget;
    GdkRegion *region;

    //      widget = GTK_WIDGET(unrolled_network_viewer);

    if (!widget->window)
        return;


    //      gdk_window_get_size(widget->window, &W, &H);
    //      printf("H = %i, W = %i\n", H, W);
    region = gdk_drawable_get_clip_region (widget->window);
    /* redraw the cairo canvas completely by exposing it */
    gdk_window_invalidate_region (widget->window, region, TRUE);
    gdk_window_process_updates (widget->window, TRUE);

    gdk_region_destroy (region);
}

void
gtk_unrolled_network_viewer_type_of_view (GtkUnrolled_network_viewer *
        unrolled_network_viewer, int value)
{
    unrolled_network_viewer->type_of_view = value;
//     int p;
//     if  (value == PATTERNS) {
//         for (p = 0; p < P_MAX; p++) {
// 	  unrolled_network_viewer->check_button[p] = gtk_check_button_new();
//         }
//
//     }
}

void unrolled_network_viewer_network_type_set(GtkUnrolled_network_viewer *unrolled_network_viewer, int *network_type)
{
    unrolled_network_viewer->network_type = network_type;


}

void unrolled_network_viewer_error_type_set(GtkUnrolled_network_viewer *unrolled_network_viewer, int *error_type)
{
    unrolled_network_viewer->error_type = error_type;

}

int gtk_unrolled_network_viewer_get_type_of_view(GtkUnrolled_network_viewer *unrolled_network_viewer)
{
    return  unrolled_network_viewer->type_of_view;
}

void gtk_unrolled_network_viewer_pdf(GtkWidget *widget, char filename[])
{
    cairo_surface_t *surface;
    cairo_t *cr;

    int width = 2000;//widget->allocation.width;
    int height = 400;//widget->allocation.height;
    char name[100];
    int i = 0;
//     GtkUnrolled_network_viewer *viewer = GTK_UNROLLED_NETWORK_VIEWER(widget);


//   if (viewer->data_type == MODEL) {

    while (filename[i] != '\0') {
        name[i] = filename[i];
        i++;
    }
    name[i] = '\0';
    printf("%s\n", name);
    sprintf(name, "%s_viewer.pdf", name);
// }
// else if (naming_graph->data_type == ROGERS) {
//   sprintf(name, "rogers_model_naming.pdf");
// }
// else if (naming_graph->data_type == PATIENTS) {
//   sprintf(name, "rogers_patients_naming.pdf");
// }
//

    surface = cairo_pdf_surface_create(name, width, height);
    cr = cairo_create(surface);

// draw naming_graph
    draw_unrolled_network_viewer(cr, widget, width, height);



    cairo_surface_destroy(surface);

}




