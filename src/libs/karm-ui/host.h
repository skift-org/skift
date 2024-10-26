#pragma once

#include <karm-app/host.h>
#include <karm-base/ring.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-text/loader.h>

#include "node.h"
#include "perf.h"

namespace Karm::Ui {

struct Host : public Node {
    Child _root;
    Opt<Res<>> _res;
    Gfx::CpuCanvas _g;
    Vec<Math::Recti> _dirty;
    PerfGraph _perf;

    bool _shouldLayout{};
    bool _shouldAnimate{};

    Host(Child root) : _root(root) {
        _root->attach(this);
    }

    ~Host() {
        _root->detach(this);
    }

    virtual Gfx::MutPixels mutPixels() = 0;

    Gfx::Pixels pixels() {
        return mutPixels();
    }

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

        if (debugShowRepaintBounds) {
            static auto hue = 0.0;
            g.strokeStyle(Gfx::stroke(Gfx::hsvToRgb({hue, 1, 1}).withOpacity(0.5)).withWidth(4).withAlign(Gfx::INSIDE_ALIGN));
            hue += 1;
            if (hue > 360)
                hue = 0;
            g.stroke(r.cast<f64>());
        }

        if (debugShowPerfGraph)
            _perf.paint(_g);

        g.pop();
    }

    void paint() {
        if (debugShowPerfGraph)
            _dirty.pushBack({0, 0, 256, 100});

        _g.begin(mutPixels());

        _perf.record(PerfEvent::PAINT);
        for (auto &d : _dirty) {
            paint(_g, d);
        }
        auto elapsed = _perf.end();

        static auto threshold = TimeSpan::fromMSecs(15);
        if (elapsed > threshold) {
            logWarn("Stutter detected, paint took {} for {} nodes, threshold raised", elapsed, debugNodeCount);
            threshold = elapsed;
        }

        _g.end();

        flip(_dirty);
        _dirty.clear();
    }

    void layout(Math::Recti r) override {
        _perf.record(PerfEvent::LAYOUT);
        _root->layout(r);
        auto elapsed = _perf.end();
        static auto threshold = TimeSpan::fromMSecs(1);
        if (elapsed > threshold) {
            logWarn("Stutter detected, layout took {} for {} nodes, threshold raised", elapsed, debugNodeCount);
            threshold = elapsed;
        }
    }

    void event(App::Event &event) override {
        _perf.record(PerfEvent::INPUT);
        _root->event(event);
        auto elapsed = _perf.end();
        static auto threshold = TimeSpan::fromMSecs(1);
        if (elapsed > threshold) {
            logWarn("Stutter detected, event took {} for {} nodes, threshold raised", elapsed, debugNodeCount);
            threshold = elapsed;
        }
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
