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

    Rectangle destination = _icon->bound(ICON_18PX).centered_within(widget_get_content_bound(this));

    painter.blit_icon(
        *_icon,
        ICON_18PX,
        destination,
        widget_get_color(this, THEME_FOREGROUND));
}

Vec2i IconPanel::size()
{
    if (_icon)
    {
        return _icon->bound(ICON_18PX).size();
    }
    else
    {
        return widget_get_bound(this).size();
    }
}
