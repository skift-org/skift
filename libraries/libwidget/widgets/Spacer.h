#pragma once

#include <libwidget/Widget.h>

class Spacer : public Widget
{
private:
public:
    Spacer(Widget *parent) : Widget(parent)
    {
        flags(Widget::FILL | Widget::NO_MOUSE_HIT);
    }

    ~Spacer() {}
};