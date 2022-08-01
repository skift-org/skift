#pragma once

#include <karm-app/host.h>
#include <karm-main/base.h>

#include "state.h"

namespace Karm::Ui {

struct Host : public Karm::App::Client {
    Child _root;

    Host(Child root) : _root(root) {}

    void paint(Gfx::Context &g) override {
        _root->paint(g);
    }

    void event(Events::Event &e) override {
        _root->event(e);
    }

    void layout(Math::Recti r) override {
        _root->layout(r);
    }
};

static inline ExitCode show(Child root) {
    return App::run<Host>(root);
}

static inline ExitCode show(Func<Child()> root) {
    return App::run<Host>(root());
}

static inline ExitCode show(auto initial, auto root) {
    return App::run<Host>(state(initial, std::move(root)));
}

} // namespace Karm::Ui
