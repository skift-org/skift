#pragma once

#include <libwidget/elements/LabelElement.h>

namespace Widget
{

template <typename TElement>
static inline RefPtr<TElement> placeholder(RefPtr<TElement> element)
{
    if (element == nullptr)
    {
        return label("[nullptr]");
    }

    return element;
}

} // namespace Widget
