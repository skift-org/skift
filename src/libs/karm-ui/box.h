#pragma once

#include <karm-layout/spacing.h>

#include "proxy.h"

namespace Karm::Ui {
struct BoxStyle {
    Layout::Spacingi spacing{};
    double borderRadius{};
    double borderWidth{};
    Gfx::Color borderColor{Gfx::TRANSPARENT};
    Gfx::Color backgroundColor{Gfx::TRANSPARENT};
    Gfx::Color foregroundColor{Gfx::WHITE};
};

template <typename Crtp>
struct _Box : public Proxy<Crtp> {
    _Box(Child child)
        : Proxy<Crtp>(child) {}

    virtual BoxStyle &boxStyle() = 0;

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();

        if (boxStyle().backgroundColor.alpha) {
            g.fillStyle(boxStyle().backgroundColor);
            g.fill(bound(), boxStyle().borderRadius);
        }

        if (boxStyle().borderWidth) {
            g.strokeStyle(Gfx::stroke(boxStyle().borderColor)
                              .withWidth(boxStyle().borderWidth)
                              .withAlign(Gfx::INSIDE_ALIGN));
            g.stroke(bound(), boxStyle().borderRadius);
        }

        g.fillStyle(boxStyle().foregroundColor);
        Proxy<Crtp>::child().paint(g, r);
        g.restore();
    }

    void layout(Math::Recti rect) override {
        Proxy<Crtp>::child().layout(boxStyle().spacing.shrink(Layout::Flow::LEFT_TO_RIGHT, rect));
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return Proxy<Crtp>::child().size(s - boxStyle().spacing.all(), hint) + boxStyle().spacing.all();
    }

    Math::Recti bound() override {
        return boxStyle().spacing.grow(Layout::Flow::LEFT_TO_RIGHT, Proxy<Crtp>::child().bound());
    }
};

struct Box : public _Box<Box> {
    using _Box<Box>::_Box;
    BoxStyle _style;

    Box(BoxStyle style, Child child)
        : _Box(child), _style(style) {}

    void reconcile(Box &o) override {
        _style = o._style;
        _Box<Box>::reconcile(o);
    }

    BoxStyle &boxStyle() override {
        return _style;
    }
};

template <typename... T>
Child box(T &&...args) {
    return makeStrong<Box>(std::forward<T>(args)...);
}

struct Spacing : public Proxy<Spacing> {
    Layout::Spacingi _spacing;

    Spacing(Layout::Spacingi spacing, Child child)
        : Proxy(child), _spacing(spacing) {}

    void reconcile(Spacing &o) override {
        _spacing = o._spacing;
        Proxy<Spacing>::reconcile(o);
    }

    void layout(Math::Recti rect) override {
        child().layout(_spacing.shrink(Layout::Flow::LEFT_TO_RIGHT, rect));
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s - _spacing.all(), hint) + _spacing.all();
    }

    Math::Recti bound() override {
        return _spacing.grow(Layout::Flow::LEFT_TO_RIGHT, child().bound());
    }
};

static inline Child spacing(Layout::Spacingi s, Child child) {
    return makeStrong<Spacing>(s, child);
}

} // namespace Karm::Ui
