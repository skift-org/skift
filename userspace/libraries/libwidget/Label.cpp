#include <libgraphic/Painter.h>
#include <libwidget/Label.h>
#include <libwidget/Window.h>

namespace Widget
{

Label::Label(Component *parent, String text)
    : Label(parent, text, Anchor::LEFT)
{
}

Label::Label(Component *parent, String text, Anchor anchor)
    : Component(parent)
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
