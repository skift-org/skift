#pragma once

#include <karm-layout/align.h>
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
        _bound = r;
        int total = 0;
        int grows = 0;

        for (auto &child : children()) {
            if (child.is<Grow>()) {
                grows += child.unwrap<Grow>().grow();
            } else {
                total += _flow.getX(child->size(r.size(), Layout::Hint::MIN));
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
                inner = _flow.setWidth(inner, _flow.getX(child->size(r.size(), Layout::Hint::MIN)));
            }

            inner = _flow.setTop(inner, _flow.getTop(r));
            inner = _flow.setBottom(inner, _flow.getBottom(r));

            child->layout(inner);
            start += _flow.getWidth(inner) + _gaps;
        }
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        int w{};
        int h{hint == Layout::Hint::MAX ? _flow.getY(s) : 0};
        bool grow = false;

        for (auto &child : children()) {
            if (child.is<Grow>())
                grow = true;

            auto childSize = child->size(s, Layout::Hint::MIN);
            w += _flow.getX(childSize);
            h = max(h, _flow.getY(childSize));
        }

        w += _gaps * (max(1uz, children().len()) - 1);
        if (grow && hint == Layout::Hint::MAX) {
            w = max(_flow.getX(s), w);
        }

        return _flow.orien() == Layout::Orien::HORIZONTAL
                   ? Math::Vec2i{w, h}
                   : Math::Vec2i{h, w};
    }
};

static inline Child flow(Layout::Flow f, int gaps, Children children) {
    return makeStrong<FlowLayout>(f, gaps, children);
}

static inline Child flow(Layout::Flow f, Children children) {
    return flow(f, 0, children);
}

static inline Child flow(Layout::Flow f, int gaps, Meta::Same<Child> auto... children) {
    return flow(f, gaps, {children...});
}

static inline Child flow(Layout::Flow f, Meta::Same<Child> auto... children) {
    return flow(f, {children...});
}

static inline Child hflow(Meta::Same<Child> auto... children) {
    return flow(Layout::Flow::LEFT_TO_RIGHT, {children...});
}

static inline Child hflow(int gaps, Meta::Same<Child> auto... children) {
    return flow(Layout::Flow::LEFT_TO_RIGHT, gaps, {children...});
}

static inline Child hflow(Children children) {
    return flow(Layout::Flow::LEFT_TO_RIGHT, children);
}

static inline Child hflow(int gaps, Children children) {
    return flow(Layout::Flow::LEFT_TO_RIGHT, gaps, children);
}

static inline Child vflow(Meta::Same<Child> auto... children) {
    return flow(Layout::Flow::TOP_TO_BOTTOM, {children...});
}

static inline Child vflow(int gaps, Meta::Same<Child> auto... children) {
    return flow(Layout::Flow::TOP_TO_BOTTOM, gaps, {children...});
}

static inline Child vflow(Children children) {
    return flow(Layout::Flow::TOP_TO_BOTTOM, children);
}

static inline Child vflow(int gaps, Children children) {
    return flow(Layout::Flow::TOP_TO_BOTTOM, gaps, children);
}

} // namespace Karm::Ui
