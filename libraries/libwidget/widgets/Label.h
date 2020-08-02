#pragma once

#include <libwidget/Widget.h>

struct Label : public Widget
{
    char *text;
};

Widget *label_create(Widget *parent, const char *text);

void label_set_text(Widget *label, const char *text);
