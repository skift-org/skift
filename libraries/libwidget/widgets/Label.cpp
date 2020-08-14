#include <libgraphic/Painter.h>
#include <libsystem/core/CString.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Label.h>

void Label::update_text(const char *text)
{
    if (text)
    {
        if (_text)
            free(_text);

        _text = strdup(text);
    }
}

Label::Label(Widget *parent, const char *text)
    : Label(parent, text, Position::LEFT)
{
}

Label::Label(Widget *parent, const char *text, Position position)
    : Widget(parent)
{
    update_text(text);
    _position = position;
}

Label::~Label()
{
    if (_text)
        free(_text);
}

void Label::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.draw_string_within(
        *widget_font(),
        _text,
        widget_get_bound(this),
        _position,
        widget_get_color(this, THEME_FOREGROUND));
}

Vec2i Label::size()
{
    return widget_font()->mesure_string(_text).size();
}
