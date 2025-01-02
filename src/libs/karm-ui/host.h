#pragma once

#include <karm-app/host.h>
#include <karm-base/ring.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-sys/time.h>
#include <karm-text/loader.h>

#include "node.h"

namespace Karm::Ui {

static constexpr auto FRAME_RATE = 60;
static constexpr auto FRAME_TIME = 1.0 / FRAME_RATE;

struct Host : public Node {
    Child _root;
    Opt<Res<>> _res;
    Gfx::CpuCanvas _g;
    Vec<Math::Recti> _dirty;

    bool _shouldLayout{};
    bool _shouldAnimate{};

    Host(Child root) : _root(root) {
        _root->attach(this);
    }

    ~Host() {
        _root->detach(this);
    }

    virtual Gfx::MutPixels mutPixels() = 0;

    Gfx::Pixels pixels() { return mutPixels(); }

    virtual void flip(Slice<Math::Recti> regions) = 0;

    virtual Res<> wait(TimeStamp) = 0;

    bool alive() {
        return not _res;
    }

    Math::Recti bound() override {
        return pixels().bound();
    }

    void paint(Gfx::Canvas &g, Math::Recti r) override {
        g.push();
        g.clip(r);
        g.clear(r, GRAY950);
        g.fillStyle(GRAY50);

        _root->paint(g, r);

        g.pop();
    }

    void paint() {
        _g.begin(mutPixels());

        for (auto &d : _dirty) {
            paint(_g, d);
        }

        _g.end();

        flip(_dirty);
        _dirty.clear();
    }

    void layout(Math::Recti r) override {
        _root->layout(r);
    }

    void event(App::Event &event) override {
        _root->event(event);
    }

    void bubble(App::Event &event) override {
        if (auto e = event.is<Node::PaintEvent>()) {
            _dirty.pushBack(e->bound);
            event.accept();
        } else if (auto e = event.is<Node::LayoutEvent>()) {
            _shouldLayout = true;
            event.accept();
        } else if (auto e = event.is<Node::AnimateEvent>()) {
            _shouldAnimate = true;
            event.accept();
        } else if (auto e = event.is<App::RequestExitEvent>()) {
            _res = e->res;
            event.accept();
        }

        if (not event.accepted()) {
            logWarn("unhandled event, bouncing down");
            _root->event(event);
        }
    }

    Res<> run() {
        _shouldLayout = true;

        auto lastFrame = Sys::now();
        auto nextFrame = lastFrame;
        bool nextFrameScheduled = false;

        auto scheduleFrame = [&] {
            auto instant = Sys::now();

            if (instant < nextFrame)
                return false;

            while (nextFrame < instant)
                nextFrame += TimeSpan::fromMSecs(FRAME_TIME * 1000);

            lastFrame = nextFrame;
            nextFrameScheduled = true;
            return true;
        };

        while (not _res) {
            if (_shouldAnimate and scheduleFrame()) {
                _shouldAnimate = false;
                auto e = App::makeEvent<Node::AnimateEvent>(FRAME_TIME);
                event(*e);
            }

            if (_shouldLayout) {
                layout(bound());
                _shouldLayout = false;
                _shouldAnimate = true;
                _dirty.pushBack(bound());
            }

            if (_dirty.len() > 0) {
                paint();
                _dirty.clear();
            }

            try$(wait(nextFrameScheduled ? nextFrame : TimeStamp::endOfTime()));
            nextFrameScheduled = false;
        }

        return _res.unwrap();
    }
};

} // namespace Karm::Ui
