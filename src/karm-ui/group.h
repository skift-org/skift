#pragma once

#include "node.h"

namespace Karm::Ui {

template <typename Crtp>
struct Group : public Widget<Crtp> {
    Children _children;
    Math::Recti _bound;

    Group() = default;

    Group(Children children) : _children(children) {
        for (auto &c : _children) {
            c->attach(this);
        }
    }

    ~Group() {
        for (auto &c : _children) {
            c->detach();
        }
    }

    Children &children() {
        return _children;
    }

    Children const &children() const {
        return _children;
    }

    void reconcile(Crtp &o) override {
        auto &us = children();
        auto &them = o.children();

        for (size_t i = 0; i < them.len(); i++) {
            if (i < us.len()) {
                us.replace(i, tryOr(us[i]->reconcile(them[i]), us[i]));
            } else {
                us.insert(i, them[i]);
            }
            us[i]->attach(this);
        }

        us.truncate(them.len());
    }

    void paint(Gfx::Context &g) const override {
        for (auto &child : children()) {
            child->paint(g);
        }
    }

    void event(Events::Event &e) override {
        for (auto &child : children()) {
            child->event(e);
            if (e.accepted)
                return;
        }
    }

    void layout(Math::Recti r) override {
        _bound = r;

        for (auto &child : children()) {
            child->layout(r);
        }
    }

    Math::Recti bound() const override {
        return _bound;
    }

    Math::Vec2i size(Math::Vec2i s) const override {
        for (auto &child : children()) {
            s = child->size(s);
        }
        return s;
    }

    void visit(Visitor &v) override {
        for (auto &child : children()) {
            v(*child);
        }
    }
};

} // namespace Karm::Ui
