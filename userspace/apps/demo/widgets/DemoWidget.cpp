#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>

#include "demo/widgets/DemoWidget.h"

void demo_widget_on_timer_tick(DemoWidget *widget)
{
    widget->tick();
    widget->should_repaint();
}

DemoWidget::DemoWidget(Component *parent)
    : Component(parent)
{
    _demo = nullptr;
    _timer = own<Timer>(1000 / 60, [this]() {
        tick();
        should_repaint();
    });

    _timer->start();
}

void DemoWidget::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (_demo)
    {
        _demo->callback(painter, bound(), _time);
    }

    painter.draw_string(*font(), _demo->name, Math::Vec2i(9, 17), Graphic::Colors::BLACK);
    painter.draw_string(*font(), _demo->name, Math::Vec2i(8, 16), Graphic::Colors::WHITE);
}
