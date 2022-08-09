#pragma once

#include <karm-layout/flow.h>

#include "empty.h"
#include "group.h"
#include "proxy.h"

namespace Karm::Ui {

struct Grow : public Proxy<Grow> {
    int _grow;

    Grow(Child child) : Proxy(child), _grow(1) {}
    Grow(int grow, Child child) : Proxy(child), _grow(grow) {}

    int grow() const {
        return _grow;
    }
};

static inline Child grow(Child child) {
    return makeStrong<Grow>(child);
}

static inline Child grow(int grow, Child child) {
    return makeStrong<Grow>(grow, child);
}

static inline Child spacer(int g = 1) {
    return grow(g, empty());
}

struct FlowLayout : public Group<FlowLayout> {
    using Group::Group;

    Layout::Flow _flow = Layout::Flow::LEFT_TO_RIGHT;
    int _gaps{};

    FlowLayout(Children children)
        : Group(children) {}

    FlowLayout(Layout::Flow flow, Children children)
        : Group(children), _flow(flow) {}

    FlowLayout(int gaps, Children children)
        : Group(children), _gaps(gaps) {}

    FlowLayout(Layout::Flow flow, int gaps, Children children)
        : Group(children), _flow(flow), _gaps(gaps) {}

    void layout(Math::Recti r) override {
        int total = 0;
        int grows = 0;

        for (auto &child : children()) {
            if (child.is<Grow>()) {
                grows += child.unwrap<Grow>().grow();
            } else {
                total += _flow.getX(child->size(r.size()));
            }
        }

        int all = _flow.getWidth(r) - _gaps * (max(1uz, children().len()) - 1);
        int growTotal = max(0, all - total);
        int growUnit = (growTotal) / max(1, grows);
        int start = _flow.getStart(r);

        for (auto &child : children()) {
            Math::Recti inner = {};

            inner = _flow.setStart(inner, start);
            if (child.is<Grow>()) {
                inner = _flow.setWidth(inner, growUnit * child.unwrap<Grow>().grow());
            } else {
                inner = _flow.setWidth(inner, _flow.getX(child->size(r.size())));
            }

            inner = _flow.setTop(inner, _flow.getTop(r));
            inner = _flow.setBottom(inner, _flow.getBottom(r));

            child->layout(inner);
            start += _flow.getWidth(inner) + _gaps;
        }
    }

    Math::Vec2i size(Math::Vec2i s) override {
        int w{};
        int h{};
        bool grow = false;

        for (auto &child : children()) {
            if (child.is<Grow>())
                grow = true;
            w += _flow.getX(child->size(s));
            h = max(h, _flow.getY(child->size(s)));
        }

        w += _gaps * (max(1uz, children().len()) - 1);
        if (grow)
            w = _flow.getX(s);

        return _flow.horizontal() ? Math::Vec2i{w, h} : Math::Vec2i{h, w};
    }
};

static inline Child flow(Children children) {
    return makeStrong<FlowLayout>(children);
}

static inline Child flow(int gaps, Children children) {
    return makeStrong<FlowLayout>(gaps, children);
}

static inline Child flow(Layout::Flow flow, Children children) {
    return makeStrong<FlowLayout>(flow, children);
}

static inline Child flow(Layout::Flow flow, int gaps, Children children) {
    return makeStrong<FlowLayout>(flow, gaps, children);
}

static inline Child flow(Meta::Same<Child> auto... children) {
    return flow({children...});
}

static inline Child flow(Layout::Flow f, Meta::Same<Child> auto... children) {
    return flow(f, {children...});
}

static inline Child flow(int gaps, Meta::Same<Child> auto... children) {
    return flow(gaps, {children...});
}

static inline Child flow(Layout::Flow f, int gaps, Meta::Same<Child> auto... children) {
    return flow(f, gaps, {children...});
}

} // namespace Karm::Ui
