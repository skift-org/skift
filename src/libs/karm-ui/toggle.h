#pragma once

#include "view.h"

namespace Karm::Ui {

struct Toggle : public View<Toggle> {
};

static inline Child toggle() {
    return makeStrong<Toggle>();
}

} // namespace Karm::Ui
