#pragma once

#include <karm-gfx/shadow.h>
#include <karm-math/spacing.h>

#include "node.h"

namespace Karm::Ui {

struct BoxStyle {
    Math::Spacingi margin{};
    Math::Spacingi padding{};

    Math::Radiif borderRadii{};
    f64 borderWidth{};

    Opt<Gfx::Paint> borderPaint{Gfx::ALPHA};
    Opt<Gfx::Paint> backgroundPaint{};
    Gfx::Paint foregroundPaint{GRAY50};
    Opt<Gfx::BoxShadow> shadowStyle{};

    BoxStyle withMargin(Math::Spacingi margin) const {
        auto copy = *this;
        copy.margin = margin;
        return copy;
    }

    BoxStyle withPadding(Math::Spacingi padding) const {
        auto copy = *this;
        copy.padding = padding;
        return copy;
    }

    BoxStyle withRadii(Math::Radiif borderRadii) const {
        auto copy = *this;
        copy.borderRadii = borderRadii;
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

    BoxStyle withShadowStyle(Gfx::BoxShadow shadowStyle) const {
        auto copy = *this;
        copy.shadowStyle = shadowStyle;
        return copy;
    }

    void paint(Gfx::Context &g, Math::Recti bound, auto inner) {
        bound = padding.grow(Math::Flow::LEFT_TO_RIGHT, bound);

        g.save();
        if (shadowStyle)
            shadowStyle->paint(g, bound);

        if (backgroundPaint) {
            g.fillStyle(*backgroundPaint);
            g.fill(bound, borderRadii);
        }

        g.fillStyle(foregroundPaint);
        inner();

        if (borderWidth and borderPaint) {
            g.strokeStyle(Gfx::stroke(*borderPaint)
                              .withWidth(borderWidth)
                              .withAlign(Gfx::INSIDE_ALIGN));
            g.stroke(bound.cast<f64>(), borderRadii);
        }

        g.restore();
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
        rect = boxStyle().margin.shrink(Math::Flow::LEFT_TO_RIGHT, rect);
        rect = boxStyle().padding.shrink(Math::Flow::LEFT_TO_RIGHT, rect);

        ProxyNode<Crtp>::child().layout(rect);
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        s = s - boxStyle().margin.all();
        s = s - boxStyle().padding.all();

        s = ProxyNode<Crtp>::child().size(s, hint);

        s = s + boxStyle().padding.all();
        s = s + boxStyle().margin.all();

        return s;
    }

    Math::Recti bound() override {
        return boxStyle().padding.grow(Math::Flow::LEFT_TO_RIGHT, ProxyNode<Crtp>::child().bound());
    }
};

Child box(BoxStyle style, Child inner);

inline auto box(BoxStyle style) {
    return [=](Child inner) {
        return box(style, inner);
    };
}

} // namespace Karm::Ui
