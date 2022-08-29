#pragma once

#include "proxy.h"

namespace Karm::Ui {

struct Scroll : public Proxy<Scroll> {
    Math::Vec2i _size;
    Math::Recti _bound{};

    Scroll(Math::Vec2i size, Child child)
        : Proxy(child), _size(size) {}

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);
    }

    void event(Events::Event &e) override {
        child().event(e);
    }

    void layout(Math::Recti r) override {
        _bound = r;

        child().layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        if (hint == Layout::Hint::MIN)
            return _size;
        else
            return s;
    }

    Math::Recti bound() override {
        return _bound;
    }
};

static inline Child scroll(Math::Vec2i size, Child child) {
    return makeStrong<Scroll>(size, child);
}

static inline Child scroll(Child child) {
    return makeStrong<Scroll>(Math::Vec2i{}, child);
}

} // namespace Karm::Ui
