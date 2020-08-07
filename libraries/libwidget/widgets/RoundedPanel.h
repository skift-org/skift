#pragma once

#include <libwidget/Widget.h>

struct RoundedPanel : public Widget
{
    int radius;
};

RoundedPanel *rounded_panel_create(Widget *parent, int radius);
