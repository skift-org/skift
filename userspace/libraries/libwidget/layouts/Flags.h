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

static inline Vector<RefPtr<Element>> fill(Vector<RefPtr<Element>> children)
{
    for (auto &child : children)
    {
        child->flags(child->flags() | Element::FILL);
    }

    return children;
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
static inline RefPtr<TElement> min_size(Math::Vec2i size, RefPtr<TElement> child)
{
    child->min_width(size.x());
    child->min_height(size.y());
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

template <typename TElement>
static inline RefPtr<TElement> max_size(Math::Vec2i size, RefPtr<TElement> child)
{
    child->max_width(size.x());
    child->max_height(size.y());
    return child;
}

static inline RefPtr<Element> enable_if(bool value, RefPtr<Element> element)
{
    element->enable_if(value);
    return element;
}

} // namespace Widget
