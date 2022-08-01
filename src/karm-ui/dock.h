#pragma once

#include <karm-layout/dock.h>

#include "group.h"
#include "proxy.h"

namespace Karm::Ui {

struct DockItem : public Proxy<DockItem> {
    Layout::Dock _dock;

    DockItem(Layout::Dock dock, Child child) : Proxy(child), _dock(dock) {}

    Layout::Dock dock() const { return _dock; }
};

template <typename... T>
Child docked(T &&...args) {
    return makeStrong<DockItem>(std::forward<T>(args)...);
}

struct DockLayout : public Group<DockLayout> {
    using Group::Group;

    void layout(Math::Recti bound) override {
        _bound = bound;
        auto outer = bound;

        auto getDock = [&](auto &child) -> Layout::Dock {
            if (child.template is<DockItem>()) {
                return child.template unwrap<DockItem>().dock();
            }

            return Layout::Dock::NONE;
        };

        for (auto &child : children()) {
            Math::Recti inner = child->size(outer.size());
            child->layout(getDock(child).apply(inner, outer));
        }
    }
};

static inline Child dock(Children children) {
    return makeStrong<DockLayout>(children);
}

template <typename... Args>
static inline Child dock(Args... args) {
    return dock({std::forward<Args>(args)...});
}

} // namespace Karm::Ui
