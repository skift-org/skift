#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct SeparatorElement : public Element
{
    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(SeparatorElement, separator);

} // namespace Widget
