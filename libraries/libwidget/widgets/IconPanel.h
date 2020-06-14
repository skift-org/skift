#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Bitmap *bitmap;
} IconPanel;

Widget *icon_panel_create(Widget *parent, const char *name);
