#include <libgraphic/Painter.h>
#include <libsystem/core/CString.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Placeholder.h>

Placeholder::Placeholder(Widget *parent, const char *text)
    : Widget(parent), _alert_icon(Icon::get("alert"))
{
    char buffer[256];
    snprintf(buffer, 256, "Cannot create an instance of \"%s\".", text);
    _text = strdup(buffer);
}

Placeholder::~Placeholder()
{
    free(_text);
}

void Placeholder::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.draw_rectangle(bound(), Colors::RED);

    painter.blit_icon(
        *_alert_icon,
        ICON_18PX,
        _alert_icon->bound(ICON_18PX).moved(bound().position() + Vec2i(8, 8)),
        Colors::RED);

    painter.draw_string(
        *font(),
        _text,
        bound().position() + Vec2i(32, 10),
        color(THEME_FOREGROUND));
}
