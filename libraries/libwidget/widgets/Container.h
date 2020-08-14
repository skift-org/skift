#pragma once

#include <libwidget/Widget.h>

struct Container : public Widget
{
    Container(Widget *parent);
};
