#pragma once

#include "node.h"

namespace Karm::Ui {

static inline void shouldRepaint(Node &n) {
    Events::PaintEvent e;
    e.bound = n.bound();
    n.bubble(e);
}

static inline void shouldLayout(Node &n) {
    Events::LayoutEvent e;
    n.bubble(e);
}

static inline void shouldAnimate(Node &n) {
    Events::AnimateEvent e;
    n.bubble(e);
}

} // namespace Karm::Ui
