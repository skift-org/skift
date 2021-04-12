#pragma once

#include <libwidget/Component.h>

#include "demo/demos/Demos.h"

class DemoWidget : public Widget::Component
{
private:
    double _time{};
    Demo *_demo;
    OwnPtr<Async::Timer> _timer;

public:
    Demo *demo() { return _demo; }

    void demo(Demo *demo)
    {
        _demo = demo;
        should_repaint();
    }

    DemoWidget(Component *parent);

    void tick() { _time += 1.0 / 60; }

    void paint(Graphic::Painter &, const Math::Recti &) override;
};
