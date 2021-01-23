#pragma once

#include <libwidget/Widget.h>

class Panel : public Widget
{
private:
    int _border_radius = 0;

public:
    void border_radius(int value) { _border_radius = value; }

    Panel(Widget *parent);

    void paint(Painter &painter, const Recti &) override;
};
