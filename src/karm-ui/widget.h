#pragma once

#include "node.h"
#include "ui.h"

namespace Karm::Ui {

struct _Widget : public Node {
};

void widget(Children children = {}) {
    group([&] {
        children();
    });
}

} // namespace Karm::Ui
