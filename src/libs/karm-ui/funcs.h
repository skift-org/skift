#pragma once

#include "node.h"

namespace Karm::Ui {

inline void shouldRepaint(Node &n) {
    Events::PaintEvent e;
    e.bound = n.bound();
    n.bubble(e);
}

inline void shouldRepaint(Node &n, Math::Recti bound) {
    Events::PaintEvent e;
    e.bound = bound;
    n.bubble(e);
}

inline void shouldLayout(Node &n) {
    Events::LayoutEvent e;
    n.bubble(e);
}

inline void shouldRebuild(Node &n) {
    Events::BuildEvent e;
    n.bubble(e);
}

inline void shouldAnimate(Node &n) {
    Events::AnimateEvent e;
    n.bubble(e);
}

template <typename T>
inline T &queryParent(Node &n) {
    void *ptr = n.query(Meta::makeId<T>());
    if (not ptr) {
        panic("not found");
    }
    return *static_cast<T *>(ptr);
}

} // namespace Karm::Ui
