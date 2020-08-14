#include <libgraphic/Painter.h>
#include <libwidget/widgets/Separator.h>

Separator::Separator(Widget *parent) : Widget(parent)
{
}

void Separator::paint(Painter &painter, Rectangle rectangle)
{
    painter.fill_rectangle(rectangle, color(THEME_BORDER));
}

Vec2i Separator::size()
{
    return Vec2i(1, 1);
}
