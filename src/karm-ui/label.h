#pragma once

#include <karm-base/string.h>

#include "widget.h"

namespace Karm::Ui {

struct _Label : public _Widget {
};

template <typename... Ts>
void label(Str, Ts &...) {
    widget([&] {
    });
}

} // namespace Karm::Ui
