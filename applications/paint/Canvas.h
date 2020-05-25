#pragma once

#include <libwidget/Widget.h>

#include "paint/PaintDocument.h"

typedef struct
{
    Widget widget;
    PaintDocument *document;
} Canvas;

Widget *canvas_create(Widget *parent, PaintDocument *document);
