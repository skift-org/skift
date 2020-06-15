#pragma once

#include <libsystem/Path.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Path *path;

    Icon *icon_computer;
    Icon *icon_expand;
} Breadcrumb;

Widget *breadcrumb_create(Widget *parent, const char *current_path);

void breadcrumb_navigate(Widget *widget, Path *path);
