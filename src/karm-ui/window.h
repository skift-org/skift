#pragma once

#include "node.h"
#include "ui.h"

namespace Karm::Ui {

struct _Window : public Node {
};

void window(Children children = {}) {
    ui().group([&] {
        children();
    });
}

} // namespace Karm::Ui
