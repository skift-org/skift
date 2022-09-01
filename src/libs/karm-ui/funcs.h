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

template <typename T>
static inline T &queryParent(Node &n) {
    void *ptr = n.query(Meta::makeId<T>());
    if (!ptr) {
        panic("not found");
    }
    return *static_cast<T *>(ptr);
}

} // namespace Karm::Ui
