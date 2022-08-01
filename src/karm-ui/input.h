#pragma once

#include "view.h"

namespace Karm::Ui {

struct Input : public View<Input> {
};

template <typename... Args>
Child input(Args &&...args) {
    return makeStrong<Input>(std::forward<Args>(args)...);
}

} // namespace Karm::Ui
