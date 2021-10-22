#pragma once

#include <libwidget/elements/LabelElement.h>

namespace Widget
{

template <typename TElement>
static inline Ref<TElement> placeholder(Ref<TElement> element)
{
    if (element == nullptr)
    {
        return label("[nullptr]");
    }

    return element;
}

} // namespace Widget
