#pragma once

#include <libwidget/Element.h>

namespace Widget
{

static inline RefPtr<Element> fill(RefPtr<Element> child)
{
    child->flags(child->flags() | Element::FILL);
    return child;
}

static inline  RefPtr<Element> square(RefPtr<Element> child)
{
    child->flags(child->flags() | Element::SQUARE);
    return child;
}

} // namespace Widget
