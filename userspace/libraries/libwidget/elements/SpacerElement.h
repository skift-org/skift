#pragma once

#include <libwidget/Element.h>

namespace Widget
{

class SpacerElement : public Element
{
public:
    SpacerElement()
    {
        flags(Element::FILL | Element::NO_MOUSE_HIT);
    }
};

static inline RefPtr<SpacerElement> spacer()
{
    return make<SpacerElement>();
}

} // namespace Widget
