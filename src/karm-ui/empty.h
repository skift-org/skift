#pragma once

#include "view.h"

namespace Karm::Ui {

struct Empty : public View<Empty> {
    Math::Vec2i _size;

    Empty(Math::Vec2i size = {}) : _size(size) {}

    void reconcile(Empty &o) override {
        _size = o._size;
    }

    Math::Vec2i size(Math::Vec2i) const override {
        return _size;
    }
};

template <typename... Args>
Child empty(Args &&...args) {
    return makeStrong<Empty>(std::forward<Args>(args)...);
}

} // namespace Karm::Ui
