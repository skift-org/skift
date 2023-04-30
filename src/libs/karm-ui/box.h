#pragma once

#include <karm-layout/spacing.h>

#include "node.h"

namespace Karm::Ui {
struct BoxStyle {
    Layout::Spacingi margin{};
    Layout::Spacingi padding{};

    Gfx::BorderRadius borderRadius{};
    f64 borderWidth{};

    Opt<Gfx::Paint> borderPaint{Gfx::ALPHA};
    Opt<Gfx::Paint> backgroundPaint{};
    Gfx::Paint foregroundPaint{GRAY50};
    Opt<Gfx::ShadowStyle> shadowStyle{};

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

    BoxStyle withRadius(Gfx::BorderRadius borderRadius) const {
        auto copy = *this;
        copy.borderRadius = borderRadius;
        return copy;
    }

    BoxStyle withBorderWidth(f64 borderWidth) const {
        auto copy = *this;
        copy.borderWidth = borderWidth;
        return copy;
    }

    BoxStyle withBorderPaint(Gfx::Paint borderPaint) const {
        auto copy = *this;
        copy.borderPaint = borderPaint;
        return copy;
    }

    BoxStyle withBackgroundPaint(Gfx::Paint backgroundPaint) const {
        auto copy = *this;
        copy.backgroundPaint = backgroundPaint;
        return copy;
    }

    BoxStyle withForegroundPaint(Gfx::Paint foregroundPaint) const {
        auto copy = *this;
        copy.foregroundPaint = foregroundPaint;
        return copy;
    }

    BoxStyle withShadowStyle(Gfx::ShadowStyle shadowStyle) const {
        auto copy = *this;
        copy.shadowStyle = shadowStyle;
        return copy;
    }

    void paint(Gfx::Context &g, Math::Recti bound, auto inner) {
        bound = padding.grow(Layout::Flow::LEFT_TO_RIGHT, bound);

        g.save();
        if (backgroundPaint) {
            g.begin();
            g.rect(bound.cast<f64>(), borderRadius);

            if (shadowStyle) {
                g.shadow(*shadowStyle);
            }

            g.fill(*backgroundPaint);
        }

        g.fillStyle(foregroundPaint);
        inner();
        g.restore();

        if (borderWidth and borderPaint) {
            g.strokeStyle(Gfx::stroke(*borderPaint)
                              .withWidth(borderWidth)
                              .withAlign(Gfx::INSIDE_ALIGN));
            g.stroke(bound, borderRadius);
        }
    }

    void paint(Gfx::Context &g, Math::Recti bound) {
        paint(g, bound, [&] {
        });
    }
};

template <typename Crtp>
struct _Box : public ProxyNode<Crtp> {
    _Box(Child child)
        : ProxyNode<Crtp>(child) {}

    virtual BoxStyle &boxStyle() = 0;

    void paint(Gfx::Context &g, Math::Recti r) override {
        boxStyle().paint(g, ProxyNode<Crtp>::_child->bound(), [&] {
            ProxyNode<Crtp>::paint(g, r);
        });
    }

    void layout(Math::Recti rect) override {
        rect = boxStyle().margin.shrink(Layout::Flow::LEFT_TO_RIGHT, rect);
        rect = boxStyle().padding.shrink(Layout::Flow::LEFT_TO_RIGHT, rect);

        ProxyNode<Crtp>::child().layout(rect);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        s = s - boxStyle().margin.all();
        s = s - boxStyle().padding.all();

        s = ProxyNode<Crtp>::child().size(s, hint);

        s = s + boxStyle().padding.all();
        s = s + boxStyle().margin.all();

        return s;
    }

    Math::Recti bound() override {
        return boxStyle().padding.grow(Layout::Flow::LEFT_TO_RIGHT, ProxyNode<Crtp>::child().bound());
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

inline Child box(BoxStyle style, Child inner) {
    return makeStrong<Box>(style, inner);
}

inline auto box(BoxStyle style) {
    return [=](Child inner) {
        return box(style, inner);
    };
}

} // namespace Karm::Ui
