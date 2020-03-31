#pragma once

#include <libgraphic/Painter.h>
#include <libmath/Face.h>

void painter3D_draw_face(Painter *painter, Face face, Color color);

void painter3D_fill_face(Painter *painter, Face face, Color color);
