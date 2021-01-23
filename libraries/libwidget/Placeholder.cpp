#include <libgraphic/Painter.h>
#include <libsystem/core/CString.h>
#include <libwidget/Placeholder.h>
#include <libwidget/Window.h>

Placeholder::Placeholder(Widget *parent, String text)
    : Widget(parent), _alert_icon(Icon::get("alert"))
{
    char buffer[256];
    snprintf(buffer, 256, "Cannot create an instance of \"%s\".", text.cstring());
    _text = buffer;
}

void Placeholder::paint(Painter &painter, const Recti &)
{
    painter.draw_rectangle(bound(), Colors::RED);

    painter.blit(
        *_alert_icon,
        ICON_18PX,
        _alert_icon->bound(ICON_18PX).moved(Vec2i(8, 8)),
        Colors::RED);

    painter.draw_string(*font(), _text.cstring(), {32, 20}, color(THEME_FOREGROUND));
}
