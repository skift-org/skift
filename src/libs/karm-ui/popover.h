#pragma once

#include "node.h"

namespace Karm::Ui {

Child popoverLayer(Child child);

inline auto popoverLayer() {
    return [](Child child) {
        return popoverLayer(child);
    };
}

void showPopover(Node &n, Math::Vec2i at, Child child);

void closePopover(Node &n);

} // namespace Karm::Ui
