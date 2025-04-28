module;

#include <karm-app/inputs.h>
#include <karm-base/func.h>
#include <karm-math/rect.h>

export module Karm.Ui:funcs;

import :node;

namespace Karm::Ui {

// MARK: Utilities -------------------------------------------------------------

export template <typename E, typename... Args>
void event(Node& n, Args&&... args) {
    auto e = App::makeEvent<E>(std::forward<Args>(args)...);
    n.event(*e);
}

export template <typename E, typename... Args>
void bubble(Node& n, Args&&... args) {
    auto e = App::makeEvent<E>(std::forward<Args>(args)...);
    n.bubble(*e);
}

export template <typename E, typename... Args>
auto bindEvent(Args&&... args) {
    return [args...](Node& n) {
        event<E>(n, args...);
    };
}

export template <typename E, typename... Args>
auto bindBubble(Args&&... args) {
    return [args...](Node& n) {
        bubble<E>(n, args...);
    };
}

export template <typename E, typename... Args>
Opt<Func<void(Node&)>> bindEventIf(bool cond, Args&&... args) {
    if (not cond)
        return NONE;
    return bindEvent<E>(args...);
}

export template <typename E, typename... Args>
Opt<Func<void(Node&)>> bindBubbleIf(bool cond, Args&&... args) {
    if (not cond)
        return NONE;
    return bindBubble<E>(args...);
}

// MARK: Helpers ---------------------------------------------------------------

export void shouldRepaint(Node& n) {
    bubble<Node::PaintEvent>(n, n.bound());
}

export void shouldRepaint(Node& n, Math::Recti bound) {
    bubble<Node::PaintEvent>(n, bound);
}

export void shouldLayout(Node& n) {
    bubble<Node::LayoutEvent>(n);
}

export void shouldAnimate(Node& n) {
    bubble<Node::AnimateEvent>(n, .0);
}

export void mouseLeave(Node& n) {
    event<App::MouseLeaveEvent>(n);
}

export void mouseLeave(Children& children) {
    for (auto& c : children) {
        mouseLeave(*c);
    }
}

} // namespace Karm::Ui
