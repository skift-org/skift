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

struct Sizing : public Proxy<Align> {
    static constexpr auto UNCONSTRAINED = -1;

    Math::Vec2i _min;
    Math::Vec2i _max;
    Math::Recti _rect;

    Sizing(Math::Vec2i min, Math::Vec2i max, Child child) : Proxy(child), _min(min), _max(max) {}

    Math::Recti bound() override {
        return _rect;
    }

    void layout(Math::Recti bound) override {
        _rect = bound;
        child().layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s) override {
        auto result = child().size(s);

        if (_min.x != UNCONSTRAINED) {
            result.x = max(result.x, _min.x);
        }

        if (_min.y != UNCONSTRAINED) {
            result.y = max(result.y, _min.y);
        }

        if (_max.x != UNCONSTRAINED) {
            result.x = min(result.x, _max.x);
        }

        if (_max.y != UNCONSTRAINED) {
            result.y = min(result.y, _max.y);
        }

        return result;
    }
};

static inline Child minSize(Math::Vec2i size, Child child) {
    return makeStrong<Sizing>(size, Sizing::UNCONSTRAINED, child);
}

static inline Child minSize(int size, Child child) {
    return minSize(Math::Vec2i{size}, child);
}

static inline Child maxSize(Math::Vec2i size, Child child) {
    return makeStrong<Sizing>(Sizing::UNCONSTRAINED, size, child);
}

static inline Child maxSize(int size, Child child) {
    return maxSize(Math::Vec2i{size}, child);
}

static inline Child pinSize(Math::Vec2i size, Child child) {
    return makeStrong<Sizing>(size, size, child);
}

static inline Child pinSize(int size, Child child) {
    return minSize(Math::Vec2i{size}, child);
}

} // namespace Karm::Ui
