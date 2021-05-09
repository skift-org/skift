#pragma once

#include <libwidget/Element.h>
#include <libwidget/layouts/StackLayout.h>

namespace Widget
{

template <typename TElement>
static inline RefPtr<TElement> fill(RefPtr<TElement> child)
{
    child->flags(child->flags() | Element::FILL);
    return child;
}

template <typename TElement>
static inline RefPtr<TElement> square(RefPtr<TElement> child)
{
    child->flags(child->flags() | Element::SQUARE);
    return child;
}

} // namespace Widget
