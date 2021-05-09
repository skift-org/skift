#pragma once

#include <libwidget2/Element.h>
#include <libwidget2/Transient.h>

namespace Widget2
{

RefPtr<BaseElement> diff(RefPtr<BaseElement> element, OwnPtr<BaseTransient> transient)
{
    if (transient == nullptr)
    {
        return element;
    }

    if (element == nullptr)
    {
        return transient->create();
    }

    if (!transient->match(*element))
    {
        return transient->create();
    }

    transient->inject(*element);

    return element;
}

} // namespace Widget2
