#pragma once

#include <libwidget/Widget.h>

struct Placeholder : public Widget
{
    char *text;
};

Widget *placeholder_create(Widget *parent, const char *name);
