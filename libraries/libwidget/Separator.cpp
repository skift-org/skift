#include <libgraphic/Painter.h>
#include <libwidget/Separator.h>

Separator::Separator(Widget *parent) : Widget(parent)
{
}

void Separator::paint(Painter &painter, const Recti &dirty)
{
    painter.fill_rectangle(dirty, color(THEME_BORDER));
}

Vec2i Separator::size()
{
    return Vec2i(1, 1);
}
