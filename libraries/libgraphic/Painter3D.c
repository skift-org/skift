#include <libgraphic/Painter3D.h>

void painter3D_draw_face(Painter *painter, Face face, Color color)
{
    painter_draw_triangle(
        painter,
        (Point){face.a.X, face.a.Y},
        (Point){face.b.X, face.b.Y},
        (Point){face.c.X, face.c.Y},
        color);
}

void painter3D_fill_face(Painter *painter, Face face, Color color)
{
    painter_fill_triangle(
        painter,
        (Point){face.a.X, face.a.Y},
        (Point){face.b.X, face.b.Y},
        (Point){face.c.X, face.c.Y},
        color);
}