#pragma once

#include <libwidget/Component.h>

namespace Widget
{

struct Separator : public Component
{
    Separator();

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

} // namespace Widget
