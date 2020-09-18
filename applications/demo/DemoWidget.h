#pragma once

#include <libwidget/Widget.h>

#include "demo/Demos.h"

class DemoWidget : public Widget
{
private:
    double _time{};
    Demo *_demo;

    RefPtr<Bitmap> _bitmap;
    OwnPtr<Painter> _painter;
    OwnPtr<Timer> _timer;

public:
    Demo *demo() { return _demo; }

    void demo(Demo *demo)
    {
        if (_painter)
        {
            _painter->clear(Colors::BLACK);
        }
        _demo = demo;
    }

    DemoWidget(Widget *parent);

    ~DemoWidget();

    void tick() { _time += 1.0 / 60; }

    void paint(Painter &painter, Rectangle rectangle);
};
