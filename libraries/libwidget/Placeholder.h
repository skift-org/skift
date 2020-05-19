#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    char *text;
} Placeholder;

Widget *placeholder_create(Widget *parent, const char *name);
