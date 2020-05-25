#include <libgraphic/Painter3D.h>

void painter3D_draw_face(Painter *painter, Face face, Color color)
{
    painter_draw_triangle(
        painter,
        vec2i(face.a.x, face.a.y),
        vec2i(face.b.x, face.b.y),
        vec2i(face.c.x, face.c.y),
        color);
}

void painter3D_fill_face(Painter *painter, Face face, Color color)
{
    painter_fill_triangle(
        painter,
        vec2i(face.a.x, face.a.y),
        vec2i(face.b.x, face.b.y),
        vec2i(face.c.x, face.c.y),
        color);
}
