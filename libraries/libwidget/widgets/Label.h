#pragma once

#include <libwidget/Widget.h>

struct Label : public Widget
{
    char *text;
    Position text_position;
};

Widget *label_create(Widget *parent, const char *text);

void label_set_text(Widget *label, const char *text);

void label_set_text_position(Widget *label, Position text_position);
