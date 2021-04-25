#pragma once

#include <libgraphic/Painter.h>
#include <libwidget/Element.h>

namespace Widget
{

class SeparatorElement : public Element
{
public:
    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override
    {
        painter.fill_rectangle(dirty, color(THEME_BORDER));
    }

    Math::Vec2i size() override
    {
        return Math::Vec2i(1, 1);
    }
};

static inline RefPtr<SeparatorElement> separator() { return make<SeparatorElement>(); }

} // namespace Widget
