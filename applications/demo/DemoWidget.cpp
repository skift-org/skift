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
    _timer = make<Timer>(1000 / 60, [this]() {
        tick();
        should_repaint();
    });

    _timer->start();
}

DemoWidget::~DemoWidget()
{
}

void DemoWidget::paint(Painter &painter, Rectangle)
{
    if (_bitmap == nullptr)
    {
        _bitmap = Bitmap::create_shared(bound().width(), bound().height()).take_value();
        _painter = own<Painter>(_bitmap);
        _painter->clear(COLOR_BLACK);
    }

    if (bound().width() != _bitmap->width() ||
        bound().height() != _bitmap->height())
    {
        _bitmap = Bitmap::create_shared(bound().width(), bound().height()).take_value();
        _painter = own<Painter>(_bitmap);
        _painter->clear(COLOR_BLACK);
    }

    if (_demo)
    {
        _demo->callback(*_painter, _bitmap->bound(), _time);
    }

    painter.blit_bitmap_no_alpha(*_bitmap, _bitmap->bound(), bound());

    painter.draw_string(*font(), _demo->name, bound().position() + Vec2i(9, 17), COLOR_BLACK);
    painter.draw_string(*font(), _demo->name, bound().position() + Vec2i(8, 16), COLOR_WHITE);
}
