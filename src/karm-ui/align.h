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

static inline Child hcenter(Child child) {
    return align(Layout::Align::HCENTER | Layout::Align::TOP, child);
}

static inline Child vcenter(Child child) {
    return align(Layout::Align::VCENTER | Layout::Align::START, child);
}

static inline Child hcenterFill(Child child) {
    return align(Layout::Align::HCENTER | Layout::Align::VFILL, child);
}

static inline Child vcenterFill(Child child) {
    return align(Layout::Align::VCENTER | Layout::Align::HFILL, child);
}

} // namespace Karm::Ui
