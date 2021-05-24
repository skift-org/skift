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

    LabelElement(String text);

    LabelElement(String text, Math::Anchor anchor);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

static inline RefPtr<LabelElement> label(String text, Math::Anchor anchor = Math::Anchor::LEFT)
{
    return make<LabelElement>(text, anchor);
}

} // namespace Widget
