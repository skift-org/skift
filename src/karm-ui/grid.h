#pragma once

#include "widget.h"

namespace Karm::Ui {

struct _Grid : public _Widget {
};

void grid(Children children = {}) {
    widget([&] {
        children();
    });
}

} // namespace Karm::Ui
