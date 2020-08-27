#include <libgraphic/Painter.h>
#include <libsystem/core/CString.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Label.h>

Label::Label(Widget *parent, String text)
    : Label(parent, text, Position::LEFT)
{
}

Label::Label(Widget *parent, String text, Position position)
    : Widget(parent)
{
    _text = text;
    _position = position;
}

void Label::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.draw_string_within(
        *font(),
        _text.cstring(),
        bound(),
        _position,
        color(THEME_FOREGROUND));
}

Vec2i Label::size()
{
    return font()->mesure_string(_text.cstring()).size();
}
