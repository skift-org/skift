#pragma once

#include <libsystem/path.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Path *path;

    Bitmap *icon_computer;
    Bitmap *icon_expand;
} Breadcrumb;

Widget *breadcrumb_create(Widget *parent, const char *current_path);