#include <libgraphic/Painter.h>
#include <libio/Format.h>
#include <libwidget/Placeholder.h>

namespace Widget
{

Placeholder::Placeholder(Component *parent, String text)
    : Component(parent),
      _alert_icon(Graphic::Icon::get("alert"))
{
    _text = IO::format("Cannot create an instance of \"{}\".", text);
}

void Placeholder::paint(Graphic::Painter &painter, const Math::Recti &)
{
    painter.draw_rectangle(bound(), Graphic::Colors::RED);

    painter.blit(
        *_alert_icon,
        Graphic::ICON_18PX,
        _alert_icon->bound(Graphic::ICON_18PX).moved(Math::Vec2i(8, 8)),
        Graphic::Colors::RED);

    painter.draw_string(*font(), _text.cstring(), {32, 20}, color(THEME_FOREGROUND));
}

} // namespace Widget
