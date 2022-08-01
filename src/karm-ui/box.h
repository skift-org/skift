#pragma once

#include <karm-layout/spacing.h>

#include "proxy.h"

namespace Karm::Ui {

struct Box : public Proxy<Box> {
    Gfx::Color _color;
    Layout::Spacingi _spacing;

    Box(Layout::Spacingi spacing, Child child)
        : Proxy(child), _spacing(spacing) {}

    Box(Gfx::Color color, Child child)
        : Proxy(child), _color(color), _spacing{} {}

    Box(Gfx::Color color, Layout::Spacingi spacing, Child child)
        : Proxy(child), _color(color), _spacing(spacing) {}

    void reconcile(Box &o) override {
        _color = o._color;
        _spacing = o._spacing;

        Proxy<Box>::reconcile(o);
    }

    void paint(Gfx::Context &g) const override {
        g.save();

        g.fillStyle(_color);
        g.fill(bound());
        g.origin(_spacing.topStart());
        child().paint(g);
        g.restore();
    }

    void layout(Math::Recti rect) override {
        child().layout(_spacing.shrink(Layout::Flow::LEFT_TO_RIGHT, rect));
    }

    Math::Vec2i size(Math::Vec2i s) const override {
        return child().size(s - _spacing.all()) + _spacing.all();
    }

    Math::Recti bound() const override {
        return _spacing.grow(Layout::Flow::LEFT_TO_RIGHT, child().bound());
    }
};

template <typename... T>
Child box(T &&...args) {
    return makeStrong<Box>(std::forward<T>(args)...);
}

} // namespace Karm::Ui
