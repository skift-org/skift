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

    Math::Recti track_bound();

    Math::Recti value_bound();

    Math::Recti thumb_bound();

    void slide_to(Math::Vec2i position);

public:
    double value() { return _value; }

    void value(double value) { _value = value; }

    Slider(Component *parent);

    void event(Event *event) override;

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

} // namespace Widget
