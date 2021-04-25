#pragma once

#include <libwidget/elements/StatefullElement.h>

namespace Widget
{

class ToggleElement : public StatefullElement<bool>
{
public:
    static constexpr auto WIDTH = 36;
    static constexpr auto HEIGHT = 18;

    ToggleElement();

    virtual void paint(Graphic::Painter &painter, const Math::Recti &);

    virtual void event(Event *event);
};

static inline RefPtr<ToggleElement> toggle(bool state)
{
    auto toggle = make<ToggleElement>();
    toggle->state(state);
    return toggle;
}

} // namespace Widget
