#pragma once

#include <karm-layout/align.h>

#include "proxy.h"

namespace Karm::Ui {

struct Align : public Proxy<Align> {
    Layout::Align _align;

    Align(Layout::Align align, Child child) : Proxy(child), _align(align) {}

    void layout(Math::Recti bound) override {
        child().layout(_align.apply<int>(
            Layout::Flow::LEFT_TO_RIGHT,
            child().size(bound.size()),
            bound));
    };

    Math::Vec2i size(Math::Vec2i s) override {
        return _align.size(child().size(s), s);
    }
};

static inline Child align(Layout::Align align, Child child) {
    return makeStrong<Align>(align, child);
}

static inline Child center(Child child) {
    return align(Layout::Align::CENTER, child);
}

} // namespace Karm::Ui
