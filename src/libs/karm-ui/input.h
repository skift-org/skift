#pragma once

#include "view.h"

namespace Karm::Ui {

struct Input : public View<Input> {
};

static inline Child input() {
    return makeStrong<Input>();
}

} // namespace Karm::Ui
