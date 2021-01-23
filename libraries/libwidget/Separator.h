#pragma once

#include <libwidget/Widget.h>

struct Separator : public Widget
{
    Separator(Widget *parent);

    void paint(Painter &, const Recti &) override;

    Vec2i size() override;
};
