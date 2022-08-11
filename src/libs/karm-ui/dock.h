#pragma once

#include <karm-debug/logger.h>
#include <karm-layout/dock.h>

#include "group.h"
#include "proxy.h"

namespace Karm::Ui {

struct DockItem : public Proxy<DockItem> {
    Layout::Dock _dock;

    DockItem(Layout::Dock dock, Child child) : Proxy(child), _dock(dock) {}

    Layout::Dock dock() const { return _dock; }
};

static inline Child docked(Layout::Dock dock, Child child) {
    return makeStrong<DockItem>(dock, child);
}

static inline Child dockTop(Child child) { return docked(Layout::Dock::TOP, child); }
static inline Child dockBottom(Child child) { return docked(Layout::Dock::BOTTOM, child); }
static inline Child dockStart(Child child) { return docked(Layout::Dock::START, child); }
static inline Child dockEnd(Child child) { return docked(Layout::Dock::END, child); }

struct DockLayout : public Group<DockLayout> {
    using Group::Group;

    static auto getDock(auto &child) -> Layout::Dock {
        if (child.template is<DockItem>()) {
            return child.template unwrap<DockItem>().dock();
        }

        return Layout::Dock::NONE;
    };

    void layout(Math::Recti bound) override {
        _bound = bound;
        auto outer = bound;

        for (auto &child : children()) {
            Math::Recti inner = child->size(outer.size());
            child->layout(getDock(child).apply(inner, outer));
        }
    }

    static Math::Vec2i apply(Layout::Orien o, Math::Vec2i current, Math::Vec2i inner) {
        switch (o) {
        case Layout::Orien::NONE:
            current = current.max(inner);

        case Layout::Orien::HORIZONTAL:
            current.x += inner.x;
            current.y = max(current.y, inner.y);
            break;

        case Layout::Orien::VERTICAL:
            current.x = max(current.x, inner.x);
            current.y += inner.y;
            break;
        }

        return current;
    }

    Math::Vec2i size(Math::Vec2i) override {
        Math::Vec2i currentSize{};
        for (auto &child : mutIterRev(children())) {
            currentSize = apply(getDock(child).orien(), child->size(currentSize), currentSize);
        }
        return currentSize;
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
