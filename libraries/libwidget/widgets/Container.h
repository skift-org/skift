#pragma once

#include <libwidget/Widget.h>

struct Container : public Widget
{
};

Container *container_create(Widget *parent);
