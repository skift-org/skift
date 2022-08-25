#pragma once

#include "node.h"

namespace Karm::Ui {

template <typename Crtp>
struct Proxy : public Widget<Crtp> {
    Child _child;

    Proxy(Child child) : _child(child) {
        _child->attach(this);
    }

    ~Proxy() {
        _child->detach();
    }

    Node &child() {
        return *_child;
    }

    Node const &child() const {
        return *_child;
    }

    void reconcile(Crtp &o) override {
        _child = tryOr(_child->reconcile(o._child), _child);
    }

    void paint(Gfx::Context &g) override {
        child().paint(g);
    }

    void event(Events::Event &e) override {
        child().event(e);
    }

    void layout(Math::Recti r) override {
        child().layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }

    void visit(Visitor &v) override {
        v(*_child);
    }
};

} // namespace Karm::Ui
