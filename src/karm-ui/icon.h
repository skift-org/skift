#pragma once

#include "view.h"

namespace Karm::Ui {

struct Icon : public View<Icon> {
};

template <typename... Args>
Child icon(Args &&...args) {
    return makeStrong<Icon>(std::forward<Args>(args)...);
}

} // namespace Karm::Ui
