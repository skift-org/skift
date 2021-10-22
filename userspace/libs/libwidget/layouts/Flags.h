#pragma once

#include <libwidget/Element.h>
#include <libwidget/layouts/StackLayout.h>

namespace Widget
{

template <typename TElement>
static inline Ref<TElement> fill(Ref<TElement> child)
{
    child->flags(child->flags() | Element::FILL);
    return child;
}

static inline Vec<Ref<Element>> fill(Vec<Ref<Element>> children)
{
    for (auto &child : children)
    {
        child->flags(child->flags() | Element::FILL);
    }

    return children;
}

template <typename TElement>
static inline Ref<TElement> square(Ref<TElement> child)
{
    child->flags(child->flags() | Element::SQUARE);
    return child;
}

template <typename TElement>
static inline Ref<TElement> min_width(int value, Ref<TElement> child)
{
    child->min_width(value);
    return child;
}

template <typename TElement>
static inline Ref<TElement> min_height(int value, Ref<TElement> child)
{
    child->min_height(value);
    return child;
}

template <typename TElement>
static inline Ref<TElement> min_size(Math::Vec2i size, Ref<TElement> child)
{
    child->min_width(size.x());
    child->min_height(size.y());
    return child;
}

template <typename TElement>
static inline Ref<TElement> max_width(int value, Ref<TElement> child)
{
    child->max_width(value);
    return child;
}

template <typename TElement>
static inline Ref<TElement> max_height(int value, Ref<TElement> child)
{
    child->max_height(value);
    return child;
}

template <typename TElement>
static inline Ref<TElement> max_size(Math::Vec2i size, Ref<TElement> child)
{
    child->max_width(size.x());
    child->max_height(size.y());
    return child;
}

static inline Ref<Element> enable_if(bool value, Ref<Element> element)
{
    element->enable_if(value);
    return element;
}

} // namespace Widget
