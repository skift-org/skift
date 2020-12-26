#include <libgraphic/Painter.h>
#include <libwidget/widgets/IconPanel.h>

IconPanel::IconPanel(Widget *parent, RefPtr<Icon> icon)
    : Widget(parent), _icon(icon)
{
}

void IconPanel::paint(Painter &painter, Recti rectangle)
{
    __unused(rectangle);

    if (!_icon)
    {
        return;
    }

    Recti destination = _icon->bound(_icon_size).centered_within(content_bound());

    painter.blit_icon(
        *_icon,
        _icon_size,
        destination,
        color(THEME_FOREGROUND));
}

Vec2i IconPanel::size()
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
