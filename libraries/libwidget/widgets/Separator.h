#pragma once

#include <libwidget/Widget.h>

struct Separator : public Widget
{
private:
public:
    Separator(Widget *parent);

    void paint(Painter &painter, Rectangle rectangle);

    Vec2i size();
};
