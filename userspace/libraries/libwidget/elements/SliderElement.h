#pragma once

#include <libwidget/elements/StatefulElement.h>

namespace Widget
{

class SliderElement : public StatefulElement<double>
{
private:
    int icon_size();

    Math::Recti value_bound();

public:
    static const auto THUMB_SIZE = 16;

    SliderElement();

    void slide_to(Math::Vec2i position);

    void event(Event *event) override;

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

static inline RefPtr<SliderElement> slider()
{
    return make<SliderElement>();
}

} // namespace Widget
