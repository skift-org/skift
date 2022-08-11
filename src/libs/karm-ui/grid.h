#pragma once

#include "group.h"
#include "proxy.h"

namespace Karm::Ui {

struct GridLayout : public Group<GridLayout> {
    using Group::Group;
};

static inline Child grid(Children children) {
    return makeStrong<GridLayout>(children);
}

static inline Child grid(auto... children) {
    return grid(Children{children...});
}

} // namespace Karm::Ui
