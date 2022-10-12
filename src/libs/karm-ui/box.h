#pragma once

#include <karm-layout/spacing.h>

#include "proxy.h"

namespace Karm::Ui {
struct BoxStyle {
    Layout::Spacingi margin{};
    Layout::Spacingi padding{};

    double borderRadius{};
    double borderWidth{};

    Gfx::Color borderColor{Gfx::ALPHA};
    Gfx::Color backgroundColor{Gfx::ALPHA};
    Gfx::Color foregroundColor{Gfx::WHITE};

    BoxStyle withMargin(Layout::Spacingi margin) const {
        auto copy = *this;
        copy.margin = margin;
        return copy;
    }

    BoxStyle withPadding(Layout::Spacingi padding) const {
        auto copy = *this;
        copy.padding = padding;
        return copy;
    }

    BoxStyle withRadius(double borderRadius) const {
        auto copy = *this;
        copy.borderRadius = borderRadius;
        return copy;
    }

    void paint(Gfx::Context &g, Math::Recti bound, auto inner) {
        g.save();

        if (backgroundColor.alpha) {
            g.fillStyle(backgroundColor);
            g.fill(bound, borderRadius);
        }

        g.fillStyle(foregroundColor);
        inner();
        g.restore();

        if (borderWidth) {
            g.strokeStyle(Gfx::stroke(borderColor)
                              .withWidth(borderWidth)
                              .withAlign(Gfx::INSIDE_ALIGN));
            g.stroke(bound, borderRadius);
        }
    }
};

template <typename Crtp>
struct _Box : public Proxy<Crtp> {
    _Box(Child child)
        : Proxy<Crtp>(child) {}

    virtual BoxStyle &boxStyle() = 0;

    void paint(Gfx::Context &g, Math::Recti r) override {
        boxStyle().paint(g, bound(), [&] {
            Proxy<Crtp>::paint(g, r);
        });
    }

    void layout(Math::Recti rect) override {
        rect = boxStyle().margin.shrink(Layout::Flow::LEFT_TO_RIGHT, rect);
        rect = boxStyle().padding.shrink(Layout::Flow::LEFT_TO_RIGHT, rect);

        Proxy<Crtp>::child().layout(rect);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        s = s - boxStyle().margin.all();
        s = s - boxStyle().padding.all();

        s = Proxy<Crtp>::child().size(s, hint);

        s = s + boxStyle().padding.all();
        s = s + boxStyle().margin.all();

        return s;
    }

    Math::Recti bound() override {
        return boxStyle().padding.grow(Layout::Flow::LEFT_TO_RIGHT, Proxy<Crtp>::child().bound());
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

inline Child box(BoxStyle style, Child child) {
    return makeStrong<Box>(style, child);
}


} // namespace Karm::Ui
