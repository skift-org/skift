#include <libgraphic/Painter.h>
#include <libwidget/widgets/TextField.h>

TextField::TextField(Widget *parent)
    : Widget(parent)
{
    _scrollbar = new ScrollBar(this);
}

TextField::~TextField()
{
}

void TextField::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.fill_rectangle(this->content_bound().take_left(32), color(THEME_BORDER));
    painter.fill_rectangle(this->content_bound().take_left(32).take_right(1), color(THEME_BORDER));
}

void TextField::event(Event *event)
{
    __unused(event);
}

void TextField::do_layout()
{
    _scrollbar->bound(this->content_bound().take_right(16));
}
