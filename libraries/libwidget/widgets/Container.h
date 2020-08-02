#pragma once

#include <libwidget/Widget.h>

struct Container : public Widget
{
};

Widget *container_create(Widget *parent);
