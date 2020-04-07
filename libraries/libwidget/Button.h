#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    char *text;
} Button;

Widget *button_create(Widget *parent, const char *text);