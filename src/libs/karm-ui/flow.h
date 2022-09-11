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

static inline Child grow(int g = 1) {
    return grow(g, empty());
}

struct FlowStyle {
    Layout::Flow flow = Layout::Flow::LEFT_TO_RIGHT;
    Layout::Align align = Layout::Align::FILL;
    int gaps{};
};

struct FlowLayout : public Group<FlowLayout> {
    using Group::Group;

    FlowStyle _style;

    FlowLayout(FlowStyle style, Children children)
        : Group(children), _style(style) {}

    int computeGrowUnit(Math::Recti r) {
        int total = 0;
        int grows = 0;

        for (auto &child : children()) {
            if (child.is<Grow>()) {
                grows += child.unwrap<Grow>().grow();
            } else {
                total += _style.flow.getX(child->size(r.size(), Layout::Hint::MIN));
            }
        }

        int all = _style.flow.getWidth(r) - _style.gaps * (max(1uz, children().len()) - 1);
        int growTotal = max(0, all - total);
        return (growTotal) / max(1, grows);
    }

    void layout(Math::Recti r) override {
        _bound = r;

        int growUnit = computeGrowUnit(r);
        int start = _style.flow.getStart(r);

        for (auto &child : children()) {
            Math::Recti inner = {};
            auto childSize = child->size(r.size(), Layout::Hint::MIN);

            inner = _style.flow.setStart(inner, start);
            if (child.is<Grow>()) {
                inner = _style.flow.setWidth(inner, growUnit * child.unwrap<Grow>().grow());
            } else {
                inner = _style.flow.setWidth(inner, _style.flow.getX(childSize));
            }

            inner = _style.flow.setTop(inner, _style.flow.getTop(r));
            inner = _style.flow.setBottom(inner, _style.flow.getBottom(r));

            child->layout(_style.align.apply(_style.flow, Math::Recti{childSize}, inner));
            start += _style.flow.getWidth(inner) + _style.gaps;
        }
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        int w{};
        int h{hint == Layout::Hint::MAX ? _style.flow.getY(s) : 0};
        bool grow = false;

        for (auto &child : children()) {
            if (child.is<Grow>())
                grow = true;

            auto childSize = child->size(s, Layout::Hint::MIN);
            w += _style.flow.getX(childSize);
            h = max(h, _style.flow.getY(childSize));
        }

        w += _style.gaps * (max(1uz, children().len()) - 1);
        if (grow && hint == Layout::Hint::MAX) {
            w = max(_style.flow.getX(s), w);
        }

        return _style.flow.orien() == Layout::Orien::HORIZONTAL
                   ? Math::Vec2i{w, h}
                   : Math::Vec2i{h, w};
    }
};

static inline Child flow(FlowStyle style, Children children) {
    return makeStrong<FlowLayout>(style, children);
}

static inline Child flow(FlowStyle style, Meta::Same<Child> auto... children) {
    return flow(style, {children...});
}

static inline Child hflow(Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT}, {children...});
}

static inline Child hflow(int gaps, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .gaps = gaps}, {children...});
}

static inline Child hflow(int gaps, Layout::Align align, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, {children...});
}

static inline Child hflow(Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT}, children);
}

static inline Child hflow(int gaps, Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .gaps = gaps}, children);
}

static inline Child hflow(int gaps, Layout::Align align, Children children) {
    return flow({.flow = Layout::Flow::LEFT_TO_RIGHT, .align = align, .gaps = gaps}, children);
}

static inline Child vflow(Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM}, {children...});
}

static inline Child vflow(int gaps, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .gaps = gaps}, {children...});
}

static inline Child vflow(int gaps, Layout::Align align, Meta::Same<Child> auto... children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .align = align, .gaps = gaps}, {children...});
}

static inline Child vflow(Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM}, children);
}

static inline Child vflow(int gaps, Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .gaps = gaps}, children);
}

static inline Child vflow(int gaps, Layout::Align align, Children children) {
    return flow({.flow = Layout::Flow::TOP_TO_BOTTOM, .align = align, .gaps = gaps}, children);
}

} // namespace Karm::Ui
