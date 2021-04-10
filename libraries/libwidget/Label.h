#pragma once

#include <libutils/String.h>
#include <libwidget/Component.h>

namespace Widget
{

class Label : public Component
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

    Label(Component *parent, String text);

    Label(Component *parent, String text, Anchor anchor);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

} // namespace Widget
