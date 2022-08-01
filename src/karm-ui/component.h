#pragma once

#include "node.h"

namespace Karm::Ui {

template <typename Crtp>
struct Component : public Node<Crtp> {
    bool _rebuild = true;
    Opt<Child> _child;

    void shouldRebuild() {
        _rebuild = true;
    }

    virtual Child build() = 0;

    void reconcile(Crtp &) override {}

    void paint(Gfx::Context &g) const override {
        _child.with([&](auto &child) {
            child->paint(g);
        });
    }

    void event(Events::Event &e) override {
        _child.with([&](auto &child) {
            child->event(e);
        });
    }

    void layout(Math::Recti r) override {
        if (_rebuild) {
            auto newChild = build();

            if (!_child.with([&](auto &child) {
                    child->reconcile(newChild);
                })) {
                _child = newChild;
            }

            _rebuild = false;
        }

        _child.with([&](auto &child) {
            child->layout(r);
        });
    }

    Math::Vec2i size(Math::Vec2i s) const override {
        return _child.visit([&](auto &child) {
            return child->size(s);
        });
    }

    Math::Recti bound() const override {
        return _child.visit([&](auto &child) {
            return child->bound();
        });
    }

    void visit(Visitor &v) override {
        if (_child)
            v(**_child);
    }
};

} // namespace Karm::Ui
