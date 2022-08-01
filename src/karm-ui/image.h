#pragma once

#include "view.h"

namespace Karm::Ui {

struct Image : public View<Image> {
};

template <typename... Args>
Child image(Args &&...args) {
    return makeStrong<Image>(std::forward<Args>(args)...);
}

} // namespace Karm::Ui
