#include <libgraphic/Painter.h>
#include <libwidget/Window.h>
#include <libwidget/elements/LabelElement.h>

namespace Widget
{

LabelElement::LabelElement(String text, Math::Anchor anchor)
{
    state({
        text,
        anchor,
    });
}

void LabelElement::paint(Graphic::Painter &painter, const Math::Recti &)
{
    auto s = state();

    painter.draw_string_within(
        *font(),
        s.text.cstring(),
        bound(),
        s.anchor,
        color(THEME_FOREGROUND));
}

Math::Vec2i LabelElement::size()
{
    auto s = state();

    return {
        font()->mesure_with_fulllineheight(s.text.cstring()).size(),
    };
}

} // namespace Widget
