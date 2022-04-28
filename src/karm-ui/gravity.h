#pragma once

#include <karm-math/rect.h>

#include "flow.h"

namespace Karm::Ui {

struct Gravity {
    enum _G {
        NONE = (0 << 0),

        START = (1 << 0),
        END = (1 << 1),
        TOP = (1 << 2),
        BOTTOM = (1 << 3),
        VSTRETCH = (1 << 4),
        HSTRETCH = (1 << 5),
        VCENTER = (1 << 6),
        HCENTER = (1 << 7),

        COVER = (1 << 8),
        FIT = (1 << 9),

        CENTER = (VCENTER | HCENTER),
        VFILL = (VSTRETCH | START),
        HFILL = (HSTRETCH | TOP),
        FILL = (VFILL | HFILL),
    };

    uint32_t _g;

    constexpr Gravity() : _g(NONE) {}

    constexpr Gravity(uint32_t flags) : _g(flags) {}

    template <typename T>
    Math::Rect<T> apply(Flow flow, Math::Rect<T> child, Math::Rect<T> parent) {
        if (_g == NONE) {
            return child;
        }

        if (_g & COVER) {
            double scale = Base::max(parent.size() / child.size());
            child.size(child.size() * scale);
        }

        if (_g & FIT) {
            double scale = Base::min(parent.size() / child.size());
            child.size(child.size() * scale);
        }

        if (_g & START) {
            flow.start(child, flow.start(parent));
        }

        if (_g & END) {
            flow.end(child, flow.end(parent));
        }

        if (_g & TOP) {
            flow.top(child, flow.top(parent));
        }

        if (_g & BOTTOM) {
            flow.bottom(child, flow.bottom(parent));
        }

        if (_g & VSTRETCH) {
            flow.width(child, flow.width(parent));
        }

        if (_g & HSTRETCH) {
            flow.height(child, flow.height(parent));
        }

        if (_g & VCENTER) {
            flow.top(child, flow.top(parent) + flow.height(parent) / 2 - flow.height(child) / 2);
        }

        if (_g & HCENTER) {
            flow.start(child, flow.start(parent) + flow.width(parent) / 2 - flow.width(child) / 2);
        }

        return child;
    }
};

} // namespace Karm::Ui
