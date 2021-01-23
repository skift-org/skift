#pragma once

#include <libwidget/Widget.h>

#include "demo/Demos.h"

class DemoWidget : public Widget
{
private:
    double _time{};
    Demo *_demo;
    OwnPtr<Timer> _timer;

public:
    Demo *demo() { return _demo; }

    void demo(Demo *demo)
    {
        _demo = demo;
        should_repaint();
    }

    DemoWidget(Widget *parent);

    void tick() { _time += 1.0 / 60; }

    void paint(Painter &, const Recti &) override;
};
