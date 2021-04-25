#pragma once

#include <libwidget/Element.h>

#include "demo/demos/Demos.h"

class DemoWidget : public Widget::Element
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

    DemoWidget();

    void tick() { _time += 1.0 / 60; }

    void paint(Graphic::Painter &, const Math::Recti &) override;
};
