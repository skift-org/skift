#pragma once

#include "node.h"
#include "ui.h"

namespace Karm::Ui {

struct _Widget : public Node {
};

static inline void widget(Children children = {}) {
    ui().group([&] {
        children();
    });
}

} // namespace Karm::Ui
