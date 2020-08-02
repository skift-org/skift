#pragma once

#include <libwidget/Widget.h>

struct Label
{
    Widget widget;
    char *text;
};

Widget *label_create(Widget *parent, const char *text);

void label_set_text(Widget *label, const char *text);
