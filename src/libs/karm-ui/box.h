#pragma once

#include <karm-layout/spacing.h>

#include "proxy.h"

namespace Karm::Ui {
struct BoxStyle {
    Layout::Spacingi spacing{};
    double borderRadius{};
    double borderWidth{};
    Gfx::Color borderColor{Gfx::ALPHA};
    Gfx::Color backgroundColor{Gfx::ALPHA};
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

        g.fillStyle(boxStyle().foregroundColor);
        Proxy<Crtp>::child().paint(g, r);
        g.restore();

        if (boxStyle().borderWidth) {
            g.strokeStyle(Gfx::stroke(boxStyle().borderColor)
                              .withWidth(boxStyle().borderWidth)
                              .withAlign(Gfx::INSIDE_ALIGN));
            g.stroke(bound(), boxStyle().borderRadius);
        }
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

static inline Child box(BoxStyle style, Child child) {
    return makeStrong<Box>(style, child);
}

} // namespace Karm::Ui
