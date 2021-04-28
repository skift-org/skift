#pragma once

#include <libwidget/Element.h>
#include <libwidget/layouts/StackLayout.h>

namespace Widget
{

static inline RefPtr<Element> fill(RefPtr<Element> child)
{
    child->flags(child->flags() | Element::FILL);
    return child;
}

static inline RefPtr<Element> square(RefPtr<Element> child)
{
    child->flags(child->flags() | Element::SQUARE);
    return child;
}

static inline RefPtr<Element> spacing(Insetsi outsets, RefPtr<Element> child)
{
    child->outsets(outsets);
    return child;
}

} // namespace Widget
