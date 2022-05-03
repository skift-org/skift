#pragma once

#include "node.h"
#include "ui.h"

namespace Karm::Ui {

struct _Window : public Node {
};

void window(Children children = {}) {
    group([&] {
        children();
    });
}

} // namespace Karm::Ui
