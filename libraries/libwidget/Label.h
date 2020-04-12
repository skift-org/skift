#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    char *text;
} Label;

Widget *label_create(Widget *parent, const char *text);

void label_set_text(Widget *label, const char *text);