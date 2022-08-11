#pragma once

#include "view.h"

namespace Karm::Ui {

struct Image : public View<Image> {
};

static inline Child image() {
    return makeStrong<Image>();
}

} // namespace Karm::Ui
