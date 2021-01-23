#pragma once

#include <libgraphic/Painter.h>
#include <libutils/Callback.h>
#include <libwidget/Widget.h>
#include <libwidget/Window.h>

class Slider : public Widget
{
private:
    static constexpr auto THUMP_SIZE = 12;
    static constexpr auto TRACK_HEIGHT = 4;

    double _value = 0.5;

    Recti track_bound();

    Recti value_bound();

    Recti thumb_bound();

    void slide_to(Vec2i position);

public:
    double value() { return _value; }

    void value(double value) { _value = value; }

    Slider(Widget *parent);

    void event(Event *event) override;

    void paint(Painter &painter, const Recti &) override;
};
