#pragma once

#include "state.h"

namespace Karm::Ui {

struct Host : public Node {
    Child _root;
    Opt<Error> _error;
    Gfx::Context _g{};

    bool _shouldRepaint{};
    bool _shouldLayout{};
    bool _shouldAnimate{};

    Host(Child root) : _root(root) {
        _root->attach(this);
    }

    ~Host() {
        _root->detach();
    }

    virtual Gfx::Surface surface() = 0;

    virtual void flip(Slice<Math::Recti> regions) = 0;

    virtual void pump() = 0;

    virtual void wait(size_t ms) = 0;

    bool alive() {
        return !_error;
    }

    void paint(Gfx::Context &g) override {
        _root->paint(g);
    }

    void paint() {
        auto s = surface();
        _g.begin(s);
        _g.clear(Gfx::ZINC900);
        paint(_g);
        _g.end();
        Array<Math::Recti, 1> dirty = {s.bound()};
        flip(dirty);
    }

    void event(Events::Event &e) override {
        _root->event(e);
    }

    void bubble(Events::Event &event) override {
        event
            .handle<Events::PaintEvent>([this](auto &) {
                _shouldRepaint = true;
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
        layout(surface().bound());
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
                layout(surface().bound());
                _shouldLayout = false;
                _shouldRepaint = true;
            }

            if (_shouldRepaint) {
                paint();
                _shouldRepaint = false;
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
