#pragma once

#include <libwidget/Widget.h>

#include "paint/PaintDocument.h"

struct Canvas
{
    Widget widget;
    PaintDocument *document;
};

Widget *canvas_create(Widget *parent, PaintDocument *document);
