#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct SpacerElement : public Element
{
    SpacerElement()
    {
        flags(Element::FILL | Element::NO_MOUSE_HIT);
    }
};

WIDGET_BUILDER(SpacerElement, spacer);

} // namespace Widget
