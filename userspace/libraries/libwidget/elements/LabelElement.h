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
    void text(String text)
    {
        auto s = state();
        s.text = text;
        state(s);
    }

    void anchor(Math::Anchor anchor)
    {
        auto s = state();
        s.anchor = anchor;
        state(s);
    }

    LabelElement(String text, Math::Anchor anchor = Math::Anchor::LEFT);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(LabelElement, label);

} // namespace Widget
