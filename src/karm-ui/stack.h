#pragma once

#include "group.h"

namespace Karm::Ui {

struct StackLayout : public Group<StackLayout> {
    using Group::Group;

    void layout(Math::Recti r) override {
        for (auto &child : children()) {
            child->layout(r);
        }
    }
};

static inline Child stack(Children children) {
    return makeStrong<StackLayout>(children);
}

static inline Child stack(auto... children) {
    return stack(Children{children...});
}

} // namespace Karm::Ui
