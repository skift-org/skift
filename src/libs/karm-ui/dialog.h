#pragma once

#include "node.h"

namespace Karm::Ui {

struct ShowDialogEvent {
    Child child;
};

Child dialogLayer(Child child);

inline auto dialogLayer() {
    return [](Child child) {
        return dialogLayer(child);
    };
}

void showDialog(Node& n, Child child);

void closeDialog(Node& n);

} // namespace Karm::Ui
