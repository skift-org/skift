#pragma once

#include <libwidget/Widget.h>

struct Placeholder : public Widget
{
    char *text;
};

Placeholder *placeholder_create(Widget *parent, const char *name);
