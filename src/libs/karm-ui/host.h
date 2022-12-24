#pragma once

#include "node.h"

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
        return not _error;
    }

    Math::Recti bound() override {
        return surface().bound();
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();
        g.clip(r);
        g.clear(r, Gfx::ZINC900);

        _root->paint(g, r);

        g.fillStyle(Gfx::WHITE);
        if (debugShowRepaintBounds) {
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
        while (not _error) {
            wait(_shouldAnimate ? 16 : -1);
            if (_shouldAnimate) {
                _shouldAnimate = false;
                Events::AnimateEvent e;
                event(e);
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
};

} // namespace Karm::Ui
