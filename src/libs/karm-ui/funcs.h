#pragma once

#include "node.h"

namespace Karm::Ui {

/* --- Utilities ------------------------------------------------------------ */

template <typename E, typename... Args>
inline void event(Node &n, Args &&...args) {
    auto e = Async::makeEvent<E>(Async::Propagation::DOWN, std::forward<Args>(args)...);
    n.event(*e);
}

template <typename E, typename... Args>
inline void bubble(Node &n, Args &&...args) {
    auto e = Async::makeEvent<E>(Async::Propagation::UP, std::forward<Args>(args)...);
    n.bubble(*e);
}

template <typename E, typename... Args>
inline auto bindEvent(Args &&...args) {
    return [args...](Node &n) {
        event<E>(n, args...);
    };
}

template <typename E, typename... Args>
inline auto bindBubble(Args &&...args) {
    return [args...](Node &n) {
        bubble<E>(n, args...);
    };
}

template <typename E, typename... Args>
inline Opt<Func<void(Node &)>> bindEventIf(bool cond, Args &&...args) {
    if (not cond)
        return NONE;
    return bindEvent<E>(args...);
}

template <typename E, typename... Args>
inline Opt<Func<void(Node &)>> bindBubbleIf(bool cond, Args &&...args) {
    if (not cond)
        return NONE;
    return bindBubble<E>(args...);
}

/* --- Helpers -------------------------------------------------------------- */

inline void shouldRepaint(Node &n) {
    bubble<Node::PaintEvent>(n, n.bound());
}

inline void shouldRepaint(Node &n, Math::Recti bound) {
    bubble<Node::PaintEvent>(n, bound);
}

inline void shouldLayout(Node &n) {
    bubble<Node::LayoutEvent>(n);
}

inline void shouldAnimate(Node &n) {
    bubble<Node::AnimateEvent>(n, .0);
}

inline void mouseLeave(Node &n) {
    event<Events::MouseLeaveEvent>(n);
}

inline void mouseLeave(Children &children) {
    for (auto &c : children) {
        mouseLeave(*c);
    }
}

} // namespace Karm::Ui
