#pragma once

#include "view.h"

namespace Karm::Ui {

struct Toggle : public View<Toggle> {
};

template <typename... Args>
Child toggle(Args &&...args) {
    return makeStrong<Toggle>(std::forward<Args>(args)...);
}

} // namespace Karm::Ui
