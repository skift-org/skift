#pragma once

#include "view.h"

namespace Karm::Ui {

struct Icon : public View<Icon> {
};

static inline Child icon() {
    return makeStrong<Icon>();
}

} // namespace Karm::Ui
