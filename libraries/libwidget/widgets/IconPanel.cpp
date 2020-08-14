#include <libgraphic/Painter.h>
#include <libwidget/widgets/IconPanel.h>

IconPanel::IconPanel(Widget *parent, RefPtr<Icon> icon)
    : Widget(parent), _icon(icon)
{
}

void IconPanel::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (!_icon)
    {
        return;
    }

    Rectangle destination = _icon->bound(ICON_18PX).centered_within(content_bound());

    painter.blit_icon(
        *_icon,
        ICON_18PX,
        destination,
        color(THEME_FOREGROUND));
}

Vec2i IconPanel::size()
{
    if (_icon)
    {
        return _icon->bound(ICON_18PX).size();
    }
    else
    {
        return bound().size();
    }
}
