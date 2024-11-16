#pragma once

#include <karm-kira/resizable.h>
#include <karm-ui/layout.h>
#include <mdi/view-compact.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_RESIZABLE{
    Mdi::VIEW_COMPACT,
    "Resizable",
    "A control that allows the user to resize an element.",
    [] {
        return Ui::hflow(
            Ui::labelMedium("One") | Ui::center() | Kr::resizable(Kr::ResizeHandle::END, 200, NONE),
            Ui::vflow(
                Ui::labelMedium("Two") | Ui::center() | Kr::resizable(Kr::ResizeHandle::BOTTOM, 200, NONE),
                Ui::labelMedium("Three") | Ui::center() | Ui::grow()
            ) | Ui::grow()
        );
    },
};

} // namespace Hideo::Zoo
