#pragma once

#include "react.h"

namespace Karm::Ui {

struct Lazy : public React<Lazy> {
    Func<Child()> _build;
    Lazy(Func<Child()> build)
        : _build(std::move(build)) {}

    Child build() override {
        return _build();
    }
};

inline Child lazy(Func<Child()> build) {
    return makeStrong<Lazy>(std::move(build));
}

} // namespace Karm::Ui
