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
static inline Vector<RefPtr<TElement>> fill(Vector<RefPtr<TElement>> childs)
{
    for (auto &child : childs)
    {
        child->flags(child->flags() | Element::FILL);
    }

    return childs;
}

template <typename TElement>
static inline RefPtr<TElement> square(RefPtr<TElement> child)
{
    child->flags(child->flags() | Element::SQUARE);
    return child;
}

template <typename TElement>
static inline RefPtr<TElement> min_width(int value, RefPtr<TElement> child)
{
    child->min_width(value);
    return child;
}

template <typename TElement>
static inline RefPtr<TElement> min_height(int value, RefPtr<TElement> child)
{
    child->min_height(value);
    return child;
}

template <typename TElement>
static inline RefPtr<TElement> max_width(int value, RefPtr<TElement> child)
{
    child->max_width(value);
    return child;
}

template <typename TElement>
static inline RefPtr<TElement> max_height(int value, RefPtr<TElement> child)
{
    child->max_height(value);
    return child;
}

} // namespace Widget
