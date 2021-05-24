#pragma once

#include <libwidget/elements/PanelElement.h>
#include <libwidget/elements/SeparatorElement.h>
#include <libwidget/layouts/Flags.h>
#include <libwidget/layouts/FlowLayout.h>

namespace Widget
{

static inline RefPtr<Element> toolbar(Vector<RefPtr<Element>> children)
{
    // clang-format off

    return panel(
        fill(
            vflow({
                spacing(4, hflow(4, children)),
                separator(),
            })
        )
    );

    // clang-format on
}

} // namespace Widget
