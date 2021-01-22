#pragma once

#include <libwidget/Widget.h>

struct Spacer : public Widget
{
    Spacer(Widget *parent) : Widget(parent)
    {
        flags(Widget::FILL | Widget::NO_MOUSE_HIT);
    }
};