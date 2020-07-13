#pragma once

enum NSVGpaintType
{
    NSVG_PAINT_NONE = 0,
    NSVG_PAINT_COLOR = 1,
    NSVG_PAINT_LINEAR_GRADIENT = 2,
    NSVG_PAINT_RADIAL_GRADIENT = 3
};

enum NSVGspreadType
{
    NSVG_SPREAD_PAD = 0,
    NSVG_SPREAD_REFLECT = 1,
    NSVG_SPREAD_REPEAT = 2
};

enum NSVGlineJoin
{
    NSVG_JOIN_MITER = 0,
    NSVG_JOIN_ROUND = 1,
    NSVG_JOIN_BEVEL = 2
};

enum NSVGlineCap
{
    NSVG_CAP_BUTT = 0,
    NSVG_CAP_ROUND = 1,
    NSVG_CAP_SQUARE = 2
};

enum NSVGfillRule
{
    NSVG_FILLRULE_NONZERO = 0,
    NSVG_FILLRULE_EVENODD = 1
};

enum NSVGflags
{
    NSVG_FLAGS_VISIBLE = 0x01
};

typedef struct NSVGgradientStop
{
    unsigned int color;
    float offset;
} NSVGgradientStop;

typedef struct NSVGgradient
{
    float xform[6];
    char spread;
    float fx, fy;
    int nstops;
    NSVGgradientStop stops[1];
} NSVGgradient;

typedef struct NSVGpaint
{
    char type;
    union
    {
        unsigned int color;
        NSVGgradient *gradient;
    };
} NSVGpaint;

typedef struct NSVGpath
{
    float *pts;            // Cubic bezier points: x0,y0, [cpx1,cpx1,cpx2,cpy2,x1,y1], ...
    int npts;              // Total number of bezier points.
    char closed;           // Flag indicating if shapes should be treated as closed.
    float bounds[4];       // Tight bounding box of the shape [minx,miny,maxx,maxy].
    struct NSVGpath *next; // Pointer to next path, or NULL if last element.
} NSVGpath;

typedef struct NSVGshape
{
    char id[64];              // Optional 'id' attr of the shape or its group
    NSVGpaint fill;           // Fill paint
    NSVGpaint stroke;         // Stroke paint
    float opacity;            // Opacity of the shape.
    float strokeWidth;        // Stroke width (scaled).
    float strokeDashOffset;   // Stroke dash offset (scaled).
    float strokeDashArray[8]; // Stroke dash array (scaled).
    char strokeDashCount;     // Number of dash values in dash array.
    char strokeLineJoin;      // Stroke join type.
    char strokeLineCap;       // Stroke cap type.
    float miterLimit;         // Miter limit
    char fillRule;            // Fill rule, see NSVGfillRule.
    unsigned char flags;      // Logical or of NSVG_FLAGS_* flags
    float bounds[4];          // Tight bounding box of the shape [minx,miny,maxx,maxy].
    NSVGpath *paths;          // Linked list of paths in the image.
    struct NSVGshape *next;   // Pointer to next shape, or NULL if last element.
} NSVGshape;

typedef struct NSVGimage
{
    float width;       // Width of the image.
    float height;      // Height of the image.
    NSVGshape *shapes; // Linked list of shapes in the image.
} NSVGimage;

typedef struct NSVGrasterizer NSVGrasterizer;

NSVGrasterizer *nsvgCreateRasterizer(void);

void nsvgDeleteRasterizer(NSVGrasterizer *r);
