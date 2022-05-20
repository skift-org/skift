#pragma once

#include "node.h"
#include "ui.h"

namespace Karm::Ui {

struct _Widget : public Node {
};

static inline void widget(Children children = {}) {
    ctx().group([&] {
        children();
    });
}

} // namespace Karm::Ui
