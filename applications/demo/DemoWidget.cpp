#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>

#include "demo/DemoWidget.h"

void demo_widget_on_timer_tick(DemoWidget *widget)
{
    widget->tick();
    widget->should_repaint();
}

DemoWidget::DemoWidget(Widget *parent)
    : Widget(parent)
{
    _demo = nullptr;
    _timer = timer_create(this, 1000 / 60, (TimerCallback)demo_widget_on_timer_tick);
    timer_start(_timer);
}

DemoWidget::~DemoWidget()
{
    timer_destroy(_timer);
}

void DemoWidget::paint(Painter &painter, Rectangle)
{
    if (_bitmap == nullptr)
    {
        _bitmap = Bitmap::create_shared(widget_get_bound(this).width(), widget_get_bound(this).height()).take_value();
        _painter = own<Painter>(_bitmap);
        _painter->clear(COLOR_BLACK);
    }

    if (widget_get_bound(this).width() != _bitmap->width() ||
        widget_get_bound(this).height() != _bitmap->height())
    {
        _bitmap = Bitmap::create_shared(widget_get_bound(this).width(), widget_get_bound(this).height()).take_value();
        _painter = own<Painter>(_bitmap);
        _painter->clear(COLOR_BLACK);
    }

    if (_demo)
    {
        _demo->callback(*_painter, _bitmap->bound(), _time);
    }

    painter.blit_bitmap_no_alpha(*_bitmap, _bitmap->bound(), widget_get_bound(this));

    painter.draw_string(*widget_font(), _demo->name, widget_get_bound(this).position() + Vec2i(9, 17), COLOR_BLACK);
    painter.draw_string(*widget_font(), _demo->name, widget_get_bound(this).position() + Vec2i(8, 16), COLOR_WHITE);
}
