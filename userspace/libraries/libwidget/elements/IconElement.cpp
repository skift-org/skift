#include <libgraphic/Painter.h>
#include <libwidget/elements/IconElement.h>

namespace Widget
{

IconElement::IconElement(RefPtr<Graphic::Icon> icon, Graphic::IconSize size)
    : _icon{icon}, _icon_size{size}
{
}

void IconElement::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (!_icon)
    {
        return;
    }

    Math::Recti destination = _icon->bound(_icon_size).centered_within(bound());

    painter.blit(
        *_icon,
        _icon_size,
        destination,
        color(THEME_FOREGROUND));
}

Math::Vec2i IconElement::size()
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
