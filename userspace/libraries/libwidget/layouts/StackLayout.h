#pragma once

#include <libwidget/Element.h>

namespace Widget
{

static inline RefPtr<Element> stack(Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(STACK());
    layout->add(childs);
    return layout;
}

} // namespace Widget
