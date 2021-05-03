#pragma once

#include <libasync/Timer.h>
#include <libwidget/Element.h>

#include "demo/Demo.h"

struct DemoHostElement : public Widget::Element
{
private:
    Demo _demo;
    double _time{};
    OwnPtr<Async::Timer> _timer;

public:
    DemoHostElement(Demo demo) : _demo{demo}
    {
        _timer = own<Async::Timer>(1000 / 60, [this]() {
            _time += 1.0 / 60;
            should_repaint();
        });

        _timer->start();
    }

    void paint(Graphic::Painter &painter, const Math::Recti &) override
    {
        _demo.callback(painter, bound(), _time);
    }

    Math::Vec2i size() override
    {
        return {700, 400};
    }
};

WIDGET_BUILDER(DemoHostElement, demo_host);