#pragma once

#include <libgraphic/Painter.h>
#include <libutils/Callback.h>
#include <libwidget/Element.h>
#include <libwidget/Window.h>

namespace Widget
{

class Slider : public Element
{
private:
    static constexpr auto TRACK_HEIGHT = 4;

    double _value = 0.5;

    int icon_size();

    Math::Recti track_bound();

    Math::Recti value_bound();

    void slide_to(Math::Vec2i position);

public:
    double value() { return _value; }

    void value(double value) { _value = value; }

    Slider();

    void event(Event *event) override;

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

static inline RefPtr<Slider> slider() { return make<Slider>(); }

} // namespace Widget
