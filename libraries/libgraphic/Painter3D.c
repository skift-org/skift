#include <libgraphic/Painter3D.h>

void painter3D_draw_face(Painter *painter, Face face, Color color)
{
    painter_draw_triangle(
        painter,
        (Point){face.a.x, face.a.y},
        (Point){face.b.x, face.b.y},
        (Point){face.c.x, face.c.y},
        color);
}

void painter3D_fill_face(Painter *painter, Face face, Color color)
{
    painter_fill_triangle(
        painter,
        (Point){face.a.x, face.a.y},
        (Point){face.b.x, face.b.y},
        (Point){face.c.x, face.c.y},
        color);
}