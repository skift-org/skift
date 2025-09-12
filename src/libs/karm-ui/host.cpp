module;

#include <karm-gfx/cpu/canvas.h>
#include <karm-logger/logger.h>
#include <karm-sys/time.h>

export module Karm.Ui:host;

import Karm.Core;
import Karm.App;
import Karm.Font;
import :node;
import :atoms;

namespace Karm::Ui {

// FIXME: Don't hardcode this
export auto FRAME_RATE = 60;
export auto FRAME_TIME = 1.0 / FRAME_RATE;

export struct Host : Node {
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

    virtual Async::Task<> waitAsync(Instant) = 0;

    bool alive() {
        return not _res;
    }

    Math::Recti bound() override {
        return pixels().bound();
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(r);
        g.clear(r, GRAY950);
        g.fillStyle(GRAY50);

        _root->paint(g, r);

        g.pop();
    }

    void paint() {
        _g.begin(mutPixels());

        for (auto& d : _dirty) {
            paint(_g, d);
        }

        _g.end();

        flip(_dirty);
        _dirty.clear();
    }

    void layout(Math::Recti r) override {
        _root->layout(r);
    }

    void event(App::Event& event) override {
        _root->event(event);
    }

    void bubble(App::Event& event) override {
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

    Async::Task<> runAsync() {
        _shouldLayout = true;

        auto lastFrame = Sys::instant();
        auto nextFrame = lastFrame;
        bool nextFrameScheduled = false;

        auto scheduleFrame = [&] {
            auto instant = Sys::instant();

            if (instant < nextFrame)
                return false;

            while (nextFrame < instant)
                nextFrame += Duration::fromMSecs(FRAME_TIME * 1000);

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
                Host::paint();
                _dirty.clear();
            }

            co_trya$(waitAsync(nextFrameScheduled ? nextFrame : Instant::endOfTime()));
            nextFrameScheduled = false;
        }

        co_return _res.unwrap();
    }
};

} // namespace Karm::Ui
