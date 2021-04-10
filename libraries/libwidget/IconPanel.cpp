#include <libgraphic/Painter.h>
#include <libwidget/IconPanel.h>

namespace Widget
{

IconPanel::IconPanel(Component *parent, RefPtr<Graphic::Icon> icon)
    : Component(parent), _icon(icon)
{
}

void IconPanel::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (!_icon)
    {
        return;
    }

    Math::Recti destination = _icon->bound(_icon_size).centered_within(content());

    painter.blit(
        *_icon,
        _icon_size,
        destination,
        color(THEME_FOREGROUND));
}

Math::Vec2i IconPanel::size()
{
    if (_icon)
    {
        return _icon->bound(_icon_size).size();
    }
    else
    {
        return bound().size();
    }
}

} // namespace Widget
