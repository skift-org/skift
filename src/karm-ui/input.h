#pragma once

#include "widget.h"

namespace Karm::Ui {

struct _Input : public _Widget {
};

void input(Children children = {}) {
    widget([&] {
        children();
    });
}

} // namespace Karm::Ui
