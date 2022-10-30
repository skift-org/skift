#pragma once

#include "funcs.h"

namespace Karm::Ui {

template <typename Crtp>
struct React : public Widget<Crtp> {
    bool _rebuild = true;
    Opt<Child> _child;

    ~React() {
        _child.with([&](auto &child) {
            child->detach(this);
        });
    }

    virtual Child build() = 0;

    void rebuild() {
        auto newChild = build();

        if (_child) {
            auto tmp = (*_child)->reconcile(newChild);
            if (tmp) {
                (*_child)->detach(this);
                _child = tmp;
                (*_child)->attach(this);
            }
        } else {
            _child = newChild;
            (*_child)->attach(this);
        }
    }

    void ensureBuild() {
        if (_rebuild) {
            rebuild();
            _rebuild = false;
        }
    }

    void reconcile(Crtp &) override {}

    void paint(Gfx::Context &g, Math::Recti r) override {
        _child.with([&](auto &child) {
            child->paint(g, r);
        });
    }

    void event(Events::Event &e) override {
        _child.with([&](auto &child) {
            child->event(e);
        });
    }

    void bubble(Events::Event &e) override {
        if (e.is<Events::BuildEvent>()) {
            _rebuild = true;
            Ui::shouldLayout(*this);
        } else
            Widget<Crtp>::bubble(e);
    }

    void layout(Math::Recti r) override {
        ensureBuild();

        _child.with([&](auto &child) {
            child->layout(r);
        });
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        ensureBuild();

        return _child.visit([&](auto &child) {
            return child->size(s, hint);
        });
    }

    Math::Recti bound() override {
        ensureBuild();

        return _child.visit([&](auto &child) {
            return child->bound();
        });
    }

    void visit(Visitor &v) override {
        ensureBuild();
        v(**_child);
    }
};

} // namespace Karm::Ui
