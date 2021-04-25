#include <libgraphic/Painter.h>
#include <libwidget/Label.h>
#include <libwidget/Window.h>

namespace Widget
{

Label::Label(String text)
    : Label(text, Anchor::LEFT)
{
}

Label::Label(String text, Anchor anchor)

{
    _text = text;
    _anchor = anchor;
}

void Label::paint(Graphic::Painter &painter, const Math::Recti &)
{
    painter.draw_string_within(
        *font(),
        _text.cstring(),
        content(),
        _anchor,
        color(THEME_FOREGROUND));
}

Math::Vec2i Label::size()
{
    return {font()->mesure_with_fulllineheight(_text.cstring()).size()};
}

} // namespace Widget
