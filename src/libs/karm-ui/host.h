#pragma once

#include "state.h"

namespace Karm::Ui {

struct Host : public Node {
    Child _root;
    Opt<Error> _error;
    Gfx::Context _g{};
    Vec<Math::Recti> _dirty;

    bool _shouldLayout{};
    bool _shouldAnimate{};

    Host(Child root) : _root(root) {
        _root->attach(this);
    }

    ~Host() {
        _root->detach(this);
    }

    virtual Gfx::Surface surface() = 0;

    virtual void flip(Slice<Math::Recti> regions) = 0;

    virtual void pump() = 0;

    virtual void wait(size_t ms) = 0;

    bool alive() {
        return !_error;
    }

    Math::Recti bound() override {
        return surface().bound();
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();
        g.clip(r);
        g.clear(r, Gfx::ZINC900);
        _root->paint(g, r);

        if (DEBUG) {
            g.fillStyle(Gfx::randomColor().withOpacity(0.25));
            g.fill(r);
        }

        g.restore();
    }

    void paint() {
        auto s = surface();
        _g.begin(s);
        for (auto &d : _dirty) {
            paint(_g, d);
        }
        _g.end();
        flip(_dirty);
        _dirty.clear();
    }

    void event(Events::Event &e) override {
        _root->event(e);
    }

    void bubble(Events::Event &event) override {
        event
            .handle<Events::PaintEvent>([this](auto &e) {
                _dirty.pushBack(e.bound);
                return true;
            })
            .handle<Events::LayoutEvent>([this](auto &) {
                _shouldLayout = true;
                return true;
            })
            .handle<Events::AnimateEvent>([this](auto &) {
                _shouldAnimate = true;
                return true;
            })
            .handle<Events::ExitEvent>([this](auto &e) {
                _error = e.error;
                return true;
            });
    }

    void layout(Math::Recti r) override {
        _root->layout(r);
    }

    Error run() {
        layout(bound());
        paint();
        while (!_error) {
            wait(_shouldAnimate ? 16 : -1);
            if (_shouldAnimate) {
                Events::AnimateEvent e;
                event(e);
                _shouldAnimate = false;
            }

            pump();

            if (_shouldLayout) {
                layout(bound());
                _shouldLayout = false;
                _dirty.pushBack(bound());
            }

            if (_dirty.len() > 0) {
                paint();
                _dirty.clear();
            }
        }

        return tryOr(_error, OK);
    }

    void *query(Meta::Id id) override {
        if (id == Meta::makeId<Host>()) {
            return this;
        }

        return nullptr;
    }
};

} // namespace Karm::Ui
