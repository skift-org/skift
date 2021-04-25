#pragma once

#include <libutils/String.h>
#include <libwidget/Element.h>

namespace Widget
{

class Label : public Element
{
private:
    String _text = "Label";
    Anchor _anchor = Anchor::LEFT;

public:
    void text(String text)
    {
        _text = text;
        should_relayout();
        should_repaint();
    }

    Label(String text);

    Label(String text, Anchor anchor);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

static inline RefPtr<Label> label(String text, Anchor anchor = Anchor::LEFT) { return make<Label>(text, anchor); }

} // namespace Widget
