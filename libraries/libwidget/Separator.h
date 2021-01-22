#pragma once

#include <libwidget/Widget.h>

class Separator : public Widget
{
private:
public:
    Separator(Widget *parent);

    void paint(Painter &painter, Recti rectangle) override;

    Vec2i size() override;
};
