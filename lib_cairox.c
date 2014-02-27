
typedef enum boolean {FALSE, TRUE} Boolean;

#include "lib_cairox.h"
//#include "lib_print.h"
#include "lib_string.h"
//#include "lib_error.h"
#include <math.h>
#include <string.h>
#include <ctype.h>

/******************************************************************************/
/* SECTION 1: Line utility functions: *****************************************/
/******************************************************************************/

static double line_length(CairoxPoint *p1, CairoxPoint *p2)
{
    return(sqrt((p1->x-p2->x)*(p1->x-p2->x) + (p1->y-p2->y)*(p1->y-p2->y)));
}

double line_calculate_slope(CairoxPoint *p1, CairoxPoint *p2)
{
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;

    if (dx > 0.0) {
        return(M_PI + atan(dy/dx));
    }
    else if (dx < 0.0) {
        return(atan(dy/dx));
    }
    else if (dy > 0.0) {
        return(-M_PI / 2.0);
    }
    else if (dy < 0.0) {
        return(M_PI / 2.0);
    }
    else {
        return(0.0);
    }
}

/*----------------------------------------------------------------------------*/

void line_displace(CairoxPoint *p1, CairoxPoint *p2, double offset)
{
    if (offset != 0) {
        double theta = line_calculate_slope(p1, p2);
        double dx = (short) 5 * sin(theta);
        double dy = (short) 5 * cos(theta);

        p1->x = p1->x - offset * dx;
        p1->y = p1->y + offset * dy;
        p2->x = p2->x - offset * dx;
        p2->y = p2->y + offset * dy;
    }
}

/******************************************************************************/
/* SECTION 2: Paint cairo elements: *******************************************/
/******************************************************************************/

void cairox_text_parameters_set_foreground(CairoxTextParameters *p, double r, double g, double b)
{
    p->foreground.r = r;
    p->foreground.g = g;
    p->foreground.b = b;
}

/*----------------------------------------------------------------------------*/

void cairox_text_parameters_set(CairoxTextParameters *p, double x, double y, PangoXAlignment x_align, PangoYAlignment y_align, double angle)
{
    cairox_text_parameters_set_foreground(p, 0.0, 0.0, 0.0);
    p->x = x;
    p->y = y;
    p->x_align = x_align;
    p->y_align = y_align;
    p->angle = angle;
}

/*----------------------------------------------------------------------------*/

double cairox_show_text_in_layout(cairo_t *cr, int x, int y, PangoLayout *layout, char *text)
{
    cairo_move_to(cr, x, y);
    pango_layout_set_text(layout, text, strlen(text));
    pango_cairo_show_layout(cr, layout);
    return(pangox_layout_get_string_width(layout, text));
}

/*----------------------------------------------------------------------------*/

void cairox_paint_pango_text(cairo_t *cr, CairoxTextParameters *p, PangoLayout *layout, const char *buffer)
{
    int x, y, w, h;

    if (strlen(buffer) == 0) {
        return;
    }

    cairo_save(cr);
    cairo_translate(cr, p->x, p->y);
    cairo_rotate(cr, -p->angle * M_PI / 180.0);

    pango_layout_set_text(layout, buffer, strlen(buffer));

    if (p->x_align == PANGOX_XALIGN_CENTER) {
        pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
        if ((w = pango_layout_get_width(layout)) <= 0) {
            pango_layout_get_size(layout, &w, &h);
        }
        x = - w/((double) PANGO_SCALE*2.0);
    }
    else if (p->x_align == PANGOX_XALIGN_RIGHT) {
        pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
        if ((w = pango_layout_get_width(layout)) <= 0) {
            pango_layout_get_size(layout, &w, &h);
        }
        x = - w/((double) PANGO_SCALE);
    }
    else {
        pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
        x = 0;
    }

    if (p->y_align == PANGOX_YALIGN_BOTTOM) {
        pango_layout_get_size(layout, &w, &h);
        y = - h/((double) PANGO_SCALE);
    }
    else if (p->y_align == PANGOX_YALIGN_CENTER) {
        pango_layout_get_size(layout, &w, &h);
        y = - h/((double) PANGO_SCALE*2.0);
    }
    else {
        y = 0;
    }

#if FALSE
    double x0, y0;

    /* Calculate the background rectangle coordinates: */
    pango_layout_get_size(layout, &w, &h);
    if (p->x_align == PANGOX_XALIGN_LEFT) {
        x0 = -1.0;
        y0 = y - 1.0;
    }
    else if (p->x_align == PANGOX_XALIGN_RIGHT) {
        x0 = -(w / (double) PANGO_SCALE)-1;
        y0 = y - 1.0;
    }
    else {
        x0 = -(w / (PANGO_SCALE*2.0))-1;
        y0 = y - 1.0;
    }

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.5);
    cairo_rectangle(cr, x0, y0, (w / (double) PANGO_SCALE)+2, (h / (double) PANGO_SCALE)+2);
    cairo_fill(cr);
#endif

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_move_to(cr, x-1, y-1);
    pango_cairo_show_layout(cr, layout);
    cairo_move_to(cr, x, y-1);
    pango_cairo_show_layout(cr, layout);
    cairo_move_to(cr, x+1, y-1);
    pango_cairo_show_layout(cr, layout);

    cairo_move_to(cr, x-1, y);
    pango_cairo_show_layout(cr, layout);
    cairo_move_to(cr, x+1, y);
    pango_cairo_show_layout(cr, layout);

    cairo_move_to(cr, x-1, y+1);
    pango_cairo_show_layout(cr, layout);
    cairo_move_to(cr, x, y+1);
    pango_cairo_show_layout(cr, layout);
    cairo_move_to(cr, x+1, y+1);
    pango_cairo_show_layout(cr, layout);

    cairo_set_source_rgba(cr, p->foreground.r, p->foreground.g, p->foreground.b, 1.0);
    cairo_move_to(cr, x, y);
    pango_cairo_show_layout(cr, layout);
    cairo_stroke(cr);
    cairo_restore(cr);
}

/*----------------------------------------------------------------------------*/

void cairox_paint_pango_text2(cairo_t *cr, CairoxTextParameters *p, PangoLayout *layout, const char *buffer)
{
    int x, y, w, h;
    double x0, y0;

    /* This version puts the text in a white box - over-painting the background */

    if (strlen(buffer) == 0) {
        return;
    }

    cairo_save(cr);
    cairo_translate(cr, p->x, p->y);
    cairo_rotate(cr, -p->angle * M_PI / 180.0);

    pango_layout_set_text(layout, buffer, strlen(buffer));

    if (p->x_align == PANGOX_XALIGN_CENTER) {
        pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
        if ((w = pango_layout_get_width(layout)) <= 0) {
            pango_layout_get_size(layout, &w, &h);
        }
        x = - w/((double) PANGO_SCALE*2.0);
    }
    else if (p->x_align == PANGOX_XALIGN_RIGHT) {
        pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
        if ((w = pango_layout_get_width(layout)) <= 0) {
            pango_layout_get_size(layout, &w, &h);
        }
        x = - w/((double) PANGO_SCALE);
    }
    else {
        pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
        x = 0;
    }

    if (p->y_align == PANGOX_YALIGN_BOTTOM) {
        pango_layout_get_size(layout, &w, &h);
        y = - h/((double) PANGO_SCALE);
    }
    else if (p->y_align == PANGOX_YALIGN_CENTER) {
        pango_layout_get_size(layout, &w, &h);
        y = - h/((double) PANGO_SCALE*2.0);
    }
    else {
        y = 0;
    }

    /* Calculate the background rectangle coordinates: */
    pango_layout_get_size(layout, &w, &h);
    if (p->x_align == PANGOX_XALIGN_LEFT) {
        x0 = -1.0;
        y0 = y - 1.0;
    }
    else if (p->x_align == PANGOX_XALIGN_RIGHT) {
        x0 = -(w / (double) PANGO_SCALE)-1;
        y0 = y - 1.0;
    }
    else {
        x0 = -(w / (PANGO_SCALE*2.0))-1;
        y0 = y - 1.0;
    }

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, x0, y0, (w / (double) PANGO_SCALE)+2, (h / (double) PANGO_SCALE)+2);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, p->foreground.r, p->foreground.g, p->foreground.b, 1.0);
    cairo_move_to(cr, x, y);
    pango_cairo_show_layout(cr, layout);
    cairo_stroke(cr);
    cairo_restore(cr);
}

/*----------------------------------------------------------------------------*/

void cairox_paint_pixbuf(cairo_t *cr, GdkPixbuf *pb, int x0, int y0)
{
    gdk_cairo_set_source_pixbuf(cr, pb, x0, y0);
    cairo_paint(cr);
}

/*----------------------------------------------------------------------------*/

void cairox_paint_line(cairo_t *cr, double w, double x1, double y1, double x2, double y2)
{
    cairo_set_line_width(cr, w);
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_stroke(cr);
}

/*----------------------------------------------------------------------------*/

void cairox_paint_lines(cairo_t *cr, CairoxPoint *p, int n)
{
    int i;

    cairo_new_path(cr);
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, p[0].x, p[0].y);
    for (i = 1; i < n; i++) {
        cairo_line_to(cr, p[i].x, p[i].y);
    }
    cairo_stroke(cr);
}

/*----------------------------------------------------------------------------*/

void cairox_paint_polygon(cairo_t *cr, CairoxPoint *p, int n)
{
    int i;

    cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
    cairo_new_path(cr);
    cairo_move_to(cr, p[0].x, p[0].y);
    for (i = 1; i < n; i++) {
        cairo_line_to(cr, p[i].x, p[i].y);
    }
    cairo_close_path(cr);
    cairo_fill(cr);
}

/*----------------------------------------------------------------------------*/

void cairox_paint_circle(cairo_t *cr, double x1, double y1, double r)
{
    cairo_new_path(cr);
    cairo_arc(cr, x1, y1, r, 0.0, 2.0 * M_PI);
    cairo_close_path(cr);
    cairo_fill(cr);
}

/*----------------------------------------------------------------------------*/

static void cairox_paint_arrow_head(cairo_t *cr, ArrowHeadType head, CairoxPoint p[2], double width, double zoom)
{
    CairoxPoint q[4];
    double theta = line_calculate_slope(&p[0], &p[1]);
    double cos_theta = cos(theta);
    double sin_theta = sin(theta);

    if (head == AH_SHARP) {
        q[0].x = zoom * p[1].x;
        q[0].y = zoom * p[1].y;
        q[1].x = zoom * (p[1].x + (width * 14.0 * cos(theta-0.3)));
        q[1].y = zoom * (p[1].y + (width * 14.0 * sin(theta-0.3)));
        q[2].x = zoom * (p[1].x + (width * 10.0 * cos_theta));
        q[2].y = zoom * (p[1].y + (width * 10.0 * sin_theta));
        q[3].x = zoom * (p[1].x + (width * 14.0 * cos(theta+0.3)));
        q[3].y = zoom * (p[1].y + (width * 14.0 * sin(theta+0.3)));
        cairox_paint_polygon(cr, q, 4);
    }
    else if (head == AH_WIDE) {
        q[0].x = p[1].x;
        q[0].y = p[1].y;
        q[1].x = (p[1].x + (width * 12.0 * cos(theta-0.5)));
        q[1].y = (p[1].y + (width * 12.0 * sin(theta-0.5)));
        q[2].x = (p[1].x + (width *  9.0 * cos_theta));
        q[2].y = (p[1].y + (width *  9.0 * sin_theta));
        q[3].x = (p[1].x + (width * 12.0 * cos(theta+0.5)));
        q[3].y = (p[1].y + (width * 12.0 * sin(theta+0.5)));
        cairox_paint_polygon(cr, q, 4);
    }
    else if (head == AH_BLUNT) {
        q[0].x = zoom * (p[1].x + (width * 10.0 * cos_theta));
        q[0].y = zoom * (p[1].y + (width * 10.0 * sin_theta));
        q[1].x = zoom * (p[1].x + (width * 5.0 * sin_theta));
        q[1].y = zoom * (p[1].y - (width * 5.0 * cos_theta));
        q[2].x = zoom * (p[1].x - (width * 5.0 * sin_theta));
        q[2].y = zoom * (p[1].y + (width * 5.0 * cos_theta));
        cairox_paint_polygon(cr, q, 3);
    }
    else if (head == AH_CIRCLE) {
        double d = width * zoom * 4;

        q[0].x = zoom * p[1].x;
        q[0].y = zoom * p[1].y;

        cairox_paint_circle(cr, q[0].x, q[0].y, d);
    }
}

/*----------------------------------------------------------------------------*/

//static void calculate_arc_coordinates(CairoxPoint *p0, CairoxPoint *p1, double radius, CairoxPoint *p)
//{
//    double dx = (p1->x - p0->x);
//    double dy = (p1->y - p0->y);
//    double d = sqrt((dx*dx) + (dy*dy));

//    if (d < radius) {
//        /* If the boxes overlap, draw the arrow to the box centre:    */
//        p->x = p1->x;
//        p->y = p1->y;
//    }
//    else {
//        /* Otherwise offset the arrow by a proportion of the length:  */
//        p->x = p1->x - radius * dx / d;
//        p->y = p1->y - radius * dy / d;
//    }
//}

/*----------------------------------------------------------------------------*/

static void cairox_paint_final_arrow_segment(cairo_t *cr, ArrowHeadType head, CairoxPoint p[2], double width, double zoom)
{
    double theta = line_calculate_slope(&p[0], &p[1]);
    double cos_theta = cos(theta);
    double sin_theta = sin(theta);
    CairoxPoint q;

    width = (width + 7) / 6.0;

    if (head == AH_SHARP) {
        q.x = zoom * (p[1].x + (width * 10.0 * cos_theta));
        q.y = zoom * (p[1].y + (width * 10.0 * sin_theta));
    }
    else if (head == AH_WIDE) {
        q.x = zoom * (p[1].x + (width * 10.0 * cos_theta));
        q.y = zoom * (p[1].y + (width * 10.0 * sin_theta));
    }
    else if (head == AH_CIRCLE) {
        q.x = zoom * (p[1].x + (width * 4.0 * cos_theta));
        q.y = zoom * (p[1].y + (width * 4.0 * sin_theta));
    }
    else if (head == AH_BLUNT) {
        q.x = zoom * (p[1].x + (width * 10.0 * cos_theta));
        q.y = zoom * (p[1].y + (width * 10.0 * sin_theta));
    }
    else {
        q.x = zoom * p[1].x;
        q.y = zoom * p[1].y;
    }
    cairo_line_to(cr, q.x, q.y);
    cairo_stroke(cr);
    cairox_paint_arrow_head(cr, head, p, width, zoom);
}

void cairox_paint_straight_arrow(cairo_t *cr, ArrowHeadType head, CairoxPoint *coordinates, int points, double width, double zoom)
{
    int i = 0;

    cairo_set_line_width(cr, width * zoom);
    cairo_move_to(cr, zoom * coordinates[i].x, zoom * coordinates[i].y);
    while (++i < points-1) {
        /* Make line segments */
        cairo_line_to(cr, zoom * coordinates[i].x, zoom * coordinates[i].y);
    }
    cairox_paint_final_arrow_segment(cr, head, &coordinates[i-1], width, zoom);
}

void cairox_paint_curved_arrow(cairo_t *cr, ArrowHeadType head, CairoxPoint *coordinates, int points, double width, double zoom)
{
    int i = 0;

    cairo_set_line_width(cr, width * zoom);
    cairo_move_to(cr, zoom * coordinates[i].x, zoom * coordinates[i].y);

    if (points <= 2) {
        /* Not enough line segments for true curves: */
        cairox_paint_final_arrow_segment(cr, head, &coordinates[1], width, zoom);
    }
    else {
        while (++i < points-1) {
            double w1 = line_length(&coordinates[i-1], &coordinates[i]);
            double w2 = line_length(&coordinates[i+1], &coordinates[i]);
            double dw = MIN(w1, w2) / 2.0;
            CairoxPoint point_a, centre;//  point_b,
            double slope_a, slope_b, radius;
            double angle1, angle2;

            /* Where does the arc start: */
            point_a.x = coordinates[i-1].x + (coordinates[i].x-coordinates[i-1].x)*(w1-dw)/w1;
            point_a.y = coordinates[i-1].y + (coordinates[i].y-coordinates[i-1].y)*(w1-dw)/w1;

            /* Where does the arc end: */
//             point_b.x = coordinates[i+1].x + (coordinates[i].x-coordinates[i+1].x)*(w2-dw)/w2;
//             point_b.y = coordinates[i+1].y + (coordinates[i].y-coordinates[i+1].y)*(w2-dw)/w2;

            slope_a = line_calculate_slope(&coordinates[i-1], &coordinates[i]);
            slope_b = line_calculate_slope(&coordinates[i+1], &coordinates[i]);

            radius = dw * tan((slope_b-slope_a)/2.0);

            /* arc is of size radius and goes from point_a to point_b */
            centre.x = point_a.x - radius * sin(slope_a);
            centre.y = point_a.y + radius * cos(slope_a);

            /* Draw a line segment in the appropriate direction: */
            cairo_line_to(cr, point_a.x, point_a.y);
            /* Now draw the arc: */
            if (radius < 0) {
                angle1 = slope_a + M_PI / 2.0;
                angle2 = slope_b - M_PI / 2.0;
                cairo_arc(cr, centre.x, centre.y, -radius, angle1, angle2);
            }
            else {
                angle1 = slope_a - M_PI / 2.0;
                angle2 = slope_b + M_PI / 2.0;
                cairo_arc_negative(cr, centre.x, centre.y, radius, angle1, angle2);
            }
        }
        cairox_paint_final_arrow_segment(cr, head, &coordinates[i-1], width, zoom);
    }
}

/*----------------------------------------------------------------------------*/

void cairox_paint_grid(cairo_t *cr, int w, int h, double gs)
{
    double x, y;

    cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    cairo_set_line_width(cr, 1);

    for (x = 0; x < w; x = x + gs) {
        cairo_move_to(cr, x, 0);
        cairo_line_to(cr, x, h);
    }

    for (y = 0; y < h; y = y + gs) {
        cairo_move_to(cr, 0, y);
        cairo_line_to(cr, w, y);
    }
    cairo_stroke(cr);
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
}

/*----------------------------------------------------------------------------*/

void cairox_draw_symbol(cairo_t *cr, double x, double y, int s)
{
    if (s == 0) {
        double d = 5.0;

        cairo_new_path(cr);
        cairo_move_to(cr, x-d, y-d);
        cairo_line_to(cr, x+d, y-d);
        cairo_line_to(cr, x+d, y+d);
        cairo_line_to(cr, x-d, y+d);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
    else if (s == 1) {
        double d = 6.0;

        cairo_new_path(cr);
        cairo_move_to(cr, x-d, y+d*(sqrt(3)-1));
        cairo_line_to(cr, x+d, y+d*(sqrt(3)-1));
        cairo_line_to(cr, x, y-d);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
    else if (s == 2) {
        double d = 7.0;

        cairo_new_path(cr);
        cairo_move_to(cr, x-d, y);
        cairo_line_to(cr, x, y-d);
        cairo_line_to(cr, x+d, y);
        cairo_line_to(cr, x, y+d);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
    else if (s == 3) {
        double d = 5.0;

        cairo_new_path(cr);
        cairo_arc(cr, x, y, d, 0, 2*M_PI);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
    else if (s == 4) {
        double d = 4.0;

        cairo_new_path(cr);
        cairo_move_to(cr, x-d, y-d);
        cairo_line_to(cr, x+d, y-d);
        cairo_line_to(cr, x+d, y+d);
        cairo_line_to(cr, x-d, y+d);
        cairo_close_path(cr);
        cairo_stroke(cr);
    }
    else if (s == 5) {
        double d = 5.0;

        cairo_new_path(cr);
        cairo_move_to(cr, x-d, y+d*(sqrt(3)-1));
        cairo_line_to(cr, x+d, y+d*(sqrt(3)-1));
        cairo_line_to(cr, x, y-d);
        cairo_close_path(cr);
        cairo_stroke(cr);
    }
    else if (s == 6) {
        double d = 6.0;

        cairo_new_path(cr);
        cairo_move_to(cr, x-d, y);
        cairo_line_to(cr, x, y-d);
        cairo_line_to(cr, x+d, y);
        cairo_line_to(cr, x, y+d);
        cairo_close_path(cr);
        cairo_stroke(cr);
    }
    else if (s == 7) {
        double d = 4.0;

        cairo_new_path(cr);
        cairo_arc(cr, x, y, d, 0, 2*M_PI);
        cairo_close_path(cr);
        cairo_stroke(cr);
    }
}

/******************************************************************************/
/* SECTION 3: Miscellaneous cairo extensions: *********************************/
/******************************************************************************/

void cairox_pointlist_reverse(CairoxPoint *p, int length)
{
    double tmp;
    int i;

    for (i = 0; i < (length/2); i++) {
        tmp = p[i].x;
        p[i].x = p[length-i-1].x;
        p[length-i-1].x = tmp;

        tmp = p[i].y;
        p[i].y = p[length-i-1].y;
        p[length-i-1].y = tmp;
    }
}

/*----------------------------------------------------------------------------*/

void cairox_select_colour(cairo_t *cr, int i, double alpha)
{
    if (i < 0) {        /* White */
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, alpha);
    }
    else if (i == 0) {  /* Black */
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, alpha);
    }
    else if (i == 1) {  /* Gray */
        cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, alpha);
    }
    else if (i == 2) {  /* Red */
        cairo_set_source_rgba(cr, 0.8, 0.2, 0.2, alpha);
    }
    else if (i == 3) {  /* Royal Blue */
        cairo_set_source_rgba(cr, 0.2, 0.2, 0.8, alpha);
    }
    else if (i == 4) {  /* Green */
        cairo_set_source_rgba(cr, 0.2, 0.8, 0.2, alpha);
    }
    else if (i == 5) {  /* Brick */
        cairo_set_source_rgba(cr, 0.5, 0.2, 0.0, alpha);
    }
    else if (i == 6) {  /* Purple */
        cairo_set_source_rgba(cr, 0.5, 0.2, 0.8, alpha);
    }
    else if (i == 7) {  /* Orange */
        cairo_set_source_rgba(cr, 0.8, 0.5, 0.2, alpha);
    }
    else if (i == 8) {  /* Sky Blue */
        cairo_set_source_rgba(cr, 0.2, 0.8, 0.8, alpha);
    }
}

/*----------------------------------------------------------------------------*/

static double hsl_index_to_rgb_index(double tc, double p, double q)
{
    if (tc < 0.1667) {
        return(p + ((q - p) * 6.0 * tc));
    }
    else if (tc < 0.5) {
        return(q);
    }
    else if (tc < 0.6667) {
        return(p + ((q - p) * 6.0 * (0.6667 - tc)));
    }
    else {
        return(p);
    }
}

static void hsl_to_rgb(double h, double s, double l, double *r, double *g, double *b)
{
    double q, p, tr, tg, tb;

    q = (l < 0.5) ? l * (s + 1.0) : (l + s - (l * s));
    p = 2.0 * l - q;
    tr = (h > 0.6667) ? (h - 0.6667) : h + 0.3333;
    tg = h;
    tb = (h < 0.3333) ? (h + 0.6667) : h - 0.3333;
    *r = hsl_index_to_rgb_index(tr, p, q);
    *g = hsl_index_to_rgb_index(tg, p, q);
    *b = hsl_index_to_rgb_index(tb, p, q);
}

void cairox_select_colour_scale(cairo_t *cr, double d)
{
    double r, g, b;

    if (d > 1.0) {
        r = 1.0;
        g = 1.0;
        b = 1.0;
    }
    else if (d < 0.0) {
        r = 0.0;
        g = 0.0;
        b = 0.0;
    }
    else {
        hsl_to_rgb(d * 0.667, 1.0, 0.5, &r, &g, &b);
    }
    cairo_set_source_rgb(cr, r, g, b);
}

/******************************************************************************/
/* SECTION 4: Miscellaneous pango extensions: *********************************/
/******************************************************************************/

void pangox_layout_select_font_from_string(PangoLayout *layout, char *string)
{
    if (layout != NULL) {
        PangoFontDescription *font;

        font = pango_font_description_from_string(string);
        pango_layout_set_font_description(layout, font);
        pango_font_description_free(font);
    }
}

/*----------------------------------------------------------------------------*/

void pangox_layout_set_font_face(PangoLayout *layout, OutputFontFace face)
{
    if (layout != NULL) {
        const PangoFontDescription *old_font;
        PangoFontDescription *new_font;

        if ((old_font = pango_layout_get_font_description(layout)) == NULL) {
            PangoContext *context = pango_layout_get_context(layout);
            new_font = pango_font_description_copy(pango_context_get_font_description(context));
        }
        else {
            new_font = pango_font_description_copy(old_font);
        }
        switch (face) {
        case PS_FONT_SANS: {
            pango_font_description_set_family(new_font, "Sans");
            break;
        }
        case PS_FONT_SERIF: {
            pango_font_description_set_family(new_font, "Serif");
            break;
        }
        default: {
            pango_font_description_set_family(new_font, "Fixed");
            break;
        }
        }
        pango_layout_set_font_description(layout, new_font);
        pango_font_description_free(new_font);
    }
}

/*----------------------------------------------------------------------------*/

void pangox_layout_set_font_size(PangoLayout *layout, double size)
{
    if (layout != NULL) {
        const PangoFontDescription *old_font;
        PangoFontDescription *new_font;

        if ((old_font = pango_layout_get_font_description(layout)) == NULL) {
            PangoContext *context = pango_layout_get_context(layout);
            new_font = pango_font_description_copy(pango_context_get_font_description(context));
        }
        else {
            new_font = pango_font_description_copy(old_font);
        }
        pango_font_description_set_size(new_font, size * PANGO_SCALE * 0.71);
        pango_layout_set_font_description(layout, new_font);
        pango_font_description_free(new_font);
    }
}

/*----------------------------------------------------------------------------*/

void pangox_layout_set_font_weight(PangoLayout *layout, PangoWeight weight)
{
    if (layout != NULL) {
        const PangoFontDescription *old_font;
        PangoFontDescription *new_font;

        if ((old_font = pango_layout_get_font_description(layout)) == NULL) {
            PangoContext *context = pango_layout_get_context(layout);
            new_font = pango_font_description_copy(pango_context_get_font_description(context));
        }
        else {
            new_font = pango_font_description_copy(old_font);
        }
        pango_font_description_set_weight(new_font, weight);
        pango_layout_set_font_description(layout, new_font);
        pango_font_description_free(new_font);
    }
}

/*----------------------------------------------------------------------------*/

void pangox_layout_set_font_style(PangoLayout *layout, PangoStyle style)
{
    if (layout != NULL) {
        const PangoFontDescription *old_font;
        PangoFontDescription *new_font;

        if ((old_font = pango_layout_get_font_description(layout)) == NULL) {
            PangoContext *context = pango_layout_get_context(layout);
            new_font = pango_font_description_copy(pango_context_get_font_description(context));
        }
        else {
            new_font = pango_font_description_copy(old_font);
        }
        pango_font_description_set_style(new_font, style);
        pango_layout_set_font_description(layout, new_font);
        pango_font_description_free(new_font);
    }
}

/*----------------------------------------------------------------------------*/

int pangox_layout_get_font_height(PangoLayout *layout)
{
    if (layout != NULL) {
        PangoFontMetrics *metric;
        PangoContext *context;
        int h;

        context = pango_layout_get_context(layout);
        metric = pango_context_get_metrics(context, pango_context_get_font_description(context), NULL);
        h = pango_font_metrics_get_ascent(metric) + pango_font_metrics_get_descent(metric);
        pango_font_metrics_unref(metric);

        return(h / PANGO_SCALE);
    }
    else {
        return(0);
    }
}

/*----------------------------------------------------------------------------*/

double pangox_layout_get_string_width(PangoLayout *layout, char *text)
{
    int w, h;

    pango_layout_set_text(layout, text, strlen(text));
    pango_layout_get_size(layout, &w, &h);
    return(w / (double) PANGO_SCALE);
}

void pangox_layout_list_fonts(FILE *fp, PangoLayout *layout)
{
    PangoContext *context;
    PangoFontMap *fontmap;
    PangoFontFamily **families;
    int n, i;

    context = pango_layout_get_context(layout);
    fontmap = pango_context_get_font_map(context);
    pango_font_map_list_families(fontmap, &families, &n);
    fprintf(fp, "Found %d fonts\n", n);
    for (i = 0; i < n; i++) {
        fprintf(fp, "    %s\n", pango_font_family_get_name(families[i]));
    }
    g_free(families);
}

/******************************************************************************/
/* SECTION 5: Compatability definitions: **************************************/
/******************************************************************************/

/* These functions are only available in CAIRO 1.6 and later: ----------------*/

#if CAIRO_VERSION_MAJOR == 1
#if CAIRO_VERSION_MINOR < 6
void cairo_ps_surface_set_eps(cairo_surface_t *surface, Boolean value) { }
void cairo_surface_show_page(cairo_surface_t *surface) { }
#endif
#endif

/******************************************************************************/
