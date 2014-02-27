#ifndef _lib_cairox_h_
#define _lib_cairox_h_

// #include <stdio.h>
#include <gdk/gdk.h>
#include <cairo-ps.h>
#include <cairo-pdf.h>
#ifdef WIN32
#include <cairo-win32.h>
#endif

typedef enum {PS_FONT_FIXED, PS_FONT_SANS, PS_FONT_SERIF} OutputFontFace;

typedef struct cairox_point {
    double x, y;
} CairoxPoint;

typedef struct cairox_colour {
    double r, g, b;
} CairoxColour;

typedef enum pango_x_alignment {
    PANGOX_XALIGN_LEFT, PANGOX_XALIGN_CENTER, PANGOX_XALIGN_RIGHT
} PangoXAlignment;

typedef enum pango_y_alignment {
    PANGOX_YALIGN_TOP, PANGOX_YALIGN_CENTER, PANGOX_YALIGN_BOTTOM
} PangoYAlignment;

typedef struct cairox_text_parameters {
    double x;
    double y;
    PangoXAlignment x_align;
    PangoYAlignment y_align;
    double angle;
    CairoxColour foreground;
} CairoxTextParameters;

typedef enum {AH_NONE, AH_SHARP, AH_BLUNT, AH_CIRCLE, AH_WIDE} ArrowHeadType;
typedef enum {LS_STRAIGHT, LS_CURVED} LineStyle;

extern double line_calculate_slope(CairoxPoint *p1, CairoxPoint *p2);
extern void line_displace(CairoxPoint *p1, CairoxPoint *p2, double offset);

extern void cairox_text_parameters_set(CairoxTextParameters *p, double x, double y, PangoXAlignment x_align, PangoYAlignment y_align, double angle);
extern void cairox_text_parameters_set_foreground(CairoxTextParameters *p, double r, double g, double b);

extern double cairox_show_text_in_layout(cairo_t *cr, int x, int y, PangoLayout *layout, char *text);
extern void cairox_paint_pango_text(cairo_t *cr, CairoxTextParameters *p, PangoLayout *layout, const char *buffer);
extern void cairox_paint_pango_text2(cairo_t *cr, CairoxTextParameters *p, PangoLayout *layout, const char *buffer);
extern void cairox_paint_pixbuf(cairo_t *cr, GdkPixbuf *pb, int x0, int y0);
extern void cairox_paint_line(cairo_t *cr, double w, double x1, double y1, double x2, double y2);
extern void cairox_paint_lines(cairo_t *cr, CairoxPoint *p, int n);
extern void cairox_paint_polygon(cairo_t *cr, CairoxPoint *p, int n);
extern void cairox_paint_circle(cairo_t *cr, double x1, double y1, double r);
extern void cairox_paint_straight_arrow(cairo_t *cr, ArrowHeadType head, CairoxPoint *p, int n, double width, double zoom);
extern void cairox_paint_curved_arrow(cairo_t *cr, ArrowHeadType head, CairoxPoint *p, int n, double width, double zoom);
extern void cairox_paint_grid(cairo_t *cr, int w, int h, double gs);
extern void cairox_draw_symbol(cairo_t *cr, double x, double y, int s);
extern void cairox_pointlist_reverse(CairoxPoint *p, int length);

extern void pangox_layout_select_font_from_string(PangoLayout *layout, char *string);
extern void pangox_layout_set_font_face(PangoLayout *layout, OutputFontFace face);
extern void pangox_layout_set_font_weight(PangoLayout *layout, PangoWeight weight);
extern void pangox_layout_set_font_style(PangoLayout *layout, PangoStyle style);
extern void pangox_layout_set_font_size(PangoLayout *layout, double size);

extern int  pangox_layout_get_font_height(PangoLayout *layout);
extern double pangox_layout_get_string_width(PangoLayout *layout, char *text);

extern void cairox_select_colour(cairo_t *cr, int i, double alpha);
extern void cairox_select_colour_scale(cairo_t *cr, double d);

extern void pangox_layout_list_fonts(FILE *fp, PangoLayout *layout);

/* These functions are only available in CAIRO 1.6 and later: ----------------*/
#if CAIRO_VERSION_MAJOR == 1
#if CAIRO_VERSION_MINOR < 6
void cairo_ps_surface_set_eps(cairo_surface_t *surface, Boolean value);
void cairo_surface_show_page(cairo_surface_t *surface);
#endif
#endif

#endif
