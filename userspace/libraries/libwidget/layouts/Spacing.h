#pragma once

#include <libwidget/Element.h>
#include <libwidget/layouts/Flags.h>
#include <libwidget/layouts/Stack.h>

namespace Widget
{

static inline RefPtr<Element> spacing(Insetsi insets, RefPtr<Element> child)
{
    auto layout = stack({fill(child)});
    layout->insets(insets);
    return layout;
}

} // namespace Widget