#pragma once

#include <libwidget/Component.h>

struct Separator : public Component
{
    Separator(Component *parent);

    void paint(Graphic::Painter &, const Recti &) override;

    Vec2i size() override;
};
