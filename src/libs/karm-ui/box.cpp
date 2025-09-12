module;

#include <karm-gfx/shadow.h>
#include <karm-math/insets.h>

export module Karm.Ui:box;

import :node;
import :atoms;

namespace Karm::Ui {

export struct BoxStyle {
    Math::Insetsi margin{};
    Math::Insetsi padding{};

    Math::Radiif borderRadii{};
    f64 borderWidth{};

    Opt<Gfx::Fill> borderFill{Gfx::ALPHA};
    Opt<Gfx::Fill> backgroundFill{};
    Gfx::Fill foregroundFill{GRAY50};
    Opt<Gfx::BoxShadow> shadowStyle{};

    BoxStyle withMargin(Math::Insetsi margin) const {
        auto copy = *this;
        copy.margin = margin;
        return copy;
    }

    BoxStyle withPadding(Math::Insetsi padding) const {
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

    BoxStyle withBorderFill(Gfx::Fill borderFill) const {
        auto copy = *this;
        copy.borderFill = borderFill;
        return copy;
    }

    BoxStyle withBackgroundFill(Gfx::Fill backgroundFill) const {
        auto copy = *this;
        copy.backgroundFill = backgroundFill;
        return copy;
    }

    BoxStyle withForegroundFill(Gfx::Fill foregroundFill) const {
        auto copy = *this;
        copy.foregroundFill = foregroundFill;
        return copy;
    }

    BoxStyle withShadowStyle(Gfx::BoxShadow shadowStyle) const {
        auto copy = *this;
        copy.shadowStyle = shadowStyle;
        return copy;
    }

    void paint(Gfx::Canvas& g, Math::Recti bound, auto inner) {
        bound = bound.grow(padding);

        g.push();
        if (shadowStyle)
            shadowStyle->paint(g, bound);

        if (backgroundFill) {
            g.fillStyle(*backgroundFill);
            g.fill(bound, borderRadii);
        }

        g.fillStyle(foregroundFill);
        inner();

        if (borderWidth and borderFill) {
            g.strokeStyle(
                Gfx::stroke(*borderFill)
                    .withWidth(borderWidth)
                    .withAlign(Gfx::INSIDE_ALIGN)
            );
            g.stroke(bound.cast<f64>(), borderRadii);
        }

        g.pop();
    }

    void paint(Gfx::Canvas& g, Math::Recti bound) {
        paint(g, bound, [&] {
        });
    }
};

template <typename Crtp>
struct _Box : ProxyNode<Crtp> {
    _Box(Child child)
        : ProxyNode<Crtp>(child) {}

    virtual BoxStyle& boxStyle() = 0;

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        boxStyle().paint(g, ProxyNode<Crtp>::_child->bound(), [&] {
            ProxyNode<Crtp>::paint(g, r);
        });
    }

    void layout(Math::Recti rect) override {
        rect = rect.shrink(boxStyle().margin);
        rect = rect.shrink(boxStyle().padding);

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
        auto bound = ProxyNode<Crtp>::child().bound();

        bound = bound.grow(boxStyle().padding);
        bound = bound.grow(boxStyle().margin);

        return bound;
    }
};

struct Box : _Box<Box> {
    using _Box<Box>::_Box;
    BoxStyle _style;

    Box(BoxStyle style, Child child)
        : _Box(child), _style(style) {}

    void reconcile(Box& o) override {
        _style = o._style;
        _Box<Box>::reconcile(o);
    }

    BoxStyle& boxStyle() override {
        return _style;
    }
};

export Child box(BoxStyle style, Child inner) {
    return makeRc<Box>(style, inner);
}

export auto box(BoxStyle style) {
    return [=](Child inner) {
        return box(style, inner);
    };
}

} // namespace Karm::Ui
