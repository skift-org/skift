#pragma once

#include <libgraphic/Painter.h>
#include <libutils/Callback.h>
#include <libwidget/Component.h>
#include <libwidget/Window.h>

namespace Widget
{

class Slider : public Component
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

    Slider(Component *parent);

    void event(Event *event) override;

    void paint(Graphic::Painter &painter, const Recti &) override;
};

} // namespace Widget
