#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct Spacer : public Element
{
    Spacer()
    {
        flags(Element::FILL | Element::NO_MOUSE_HIT);
    }
};

static inline RefPtr<Spacer> spacer() { return make<Spacer>(); }

} // namespace Widget
