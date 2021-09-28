#pragma once

#include <libutils/String.h>
#include <libwidget/elements/StatefulElement.h>

namespace Widget
{

struct LabelState
{
    String text = "LabelElement";
    Math::Anchor anchor = Math::Anchor::LEFT;
};

struct LabelElement :
    public StatefulElement<LabelState>
{
    LabelElement(String text, Math::Anchor anchor = Math::Anchor::LEFT);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(LabelElement, label);

} // namespace Widget
