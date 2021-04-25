#pragma once

#include <libwidget/Element.h>
#include <libwidget/layouts/Flags.h>
#include <libwidget/layouts/Stack.h>

namespace Widget
{

static inline RefPtr<Element> spacing(Insetsi outsets, RefPtr<Element> child)
{
    child->outsets(outsets);
    return child;
}

} // namespace Widget