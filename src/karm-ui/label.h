#pragma once

#include <karm-text/str.h>

#include "widget.h"

namespace Karm::Ui {

struct _Label : public _Widget {
};

template <typename... Ts>
void label(Karm::Text::Str, Ts &...) {
    widget([&] {
    });
}

} // namespace Karm::Ui
