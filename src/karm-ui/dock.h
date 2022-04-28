#pragma once

#include "flow.h"

namespace Karm::Ui {

struct Dock {
    enum _D {
        NONE,
        FILL,
        START,
        TOP,
        END,
        BOTTOM,
    };

    _D _d;

    Flow flow() const {
        switch (_d) {
        case NONE:
        case FILL:
            return Flow::LTR;
        case START:
            return Flow::LTR;
        case TOP:
            return Flow::TTB;
        case END:
            return Flow::RTL;
        case BOTTOM:
            return Flow::BTT;
        }
    }

    template <typename T>
    Math::Rect<T> apply(Flow flow, Math::Rect<T> child, Math::Rect<T> &parent) {
        flow = flow.apply(this->flow());

        switch (_d) {
        case NONE:
            break;

        case FILL:
            child = *parent;
            break;

        default:
            child = flow.origin(child, flow.origin(*parent));
            child = flow.height(child, flow.height(*parent));
            *parent = flow.start(*parent, flow.end(child));
            break;
        }

        return child;
    }
};

} // namespace Karm::Ui
