#pragma once

#include <karm-math/align.h>

#include "node.h"

namespace Karm::Ui {

Child dialogLayer(Child child);

inline auto dialogLayer() {
    return [](Child child) {
        return dialogLayer(child);
    };
}

void showDialog(Node &n, Child child);

void closeDialog(Node &n);

void showPopover(Node &n, Math::Vec2i at, Child child);

} // namespace Karm::Ui
