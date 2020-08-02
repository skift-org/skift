#pragma once

#include <libwidget/Widget.h>

#include "paint/PaintDocument.h"

struct Canvas : public Widget
{
    PaintDocument *document;
};

Widget *canvas_create(Widget *parent, PaintDocument *document);
