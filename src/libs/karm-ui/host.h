#pragma once

#include <karm-base/ring.h>
#include <karm-sys/time.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>

#include "node.h"

namespace Karm::Ui {

static constexpr auto FRAME_RATE = 60;
static constexpr auto FRAME_TIME = 1.0 / FRAME_RATE;

enum struct PerfEvent {
    NONE,
    PAINT,
    LAYOUT,
    INPUT,
};

struct PerfRecord {
    PerfEvent event;
    TimeStamp start;
    TimeStamp end;

    Gfx::Color color() const {
        switch (event) {
        case PerfEvent::PAINT:
            return duration().toMSecs() > 16 ? Gfx::RED : Gfx::GREEN;

        case PerfEvent::LAYOUT:
            return Gfx::PINK;

        case PerfEvent::INPUT:
            return Gfx::BLUE;

        default:
            return Gfx::BLACK;
        }
    }

    TimeSpan duration() const {
        return end - start;
    }
};

struct PerfGraph {
    usize _index{};
    Array<PerfRecord, 256> _records{};
    f64 _frameTime = 1;

    void record(PerfEvent e) {
        auto now = Sys::now();
        _records[_index % 256] = PerfRecord{e, now, now};
    }

    auto end() {
        auto n = Sys::now();
        auto rec = _records[_index % 256];
        auto elapsed = n - rec.start;
        _records[_index++ % 256].end = n;

        if (rec.event == PerfEvent::PAINT) {
            _frameTime = (_frameTime * 0.9) + (elapsed.toMSecs() * 0.1);
        }
        return elapsed;
    }

    f64 fps() {
        return 1000.0 / _frameTime;
    }

    Math::Recti bound() {
        return {0, 0, 256, 64};
    }

    void paint(Gfx::Context &g) {
        g.save();
        g.clip(bound());
        g.fillStyle(Gfx::GREEN900.withOpacity(0.5));
        g.fill(bound());

        g.strokeStyle(Gfx::stroke(Gfx::GREEN.withOpacity(0.5)).withAlign(Gfx::INSIDE_ALIGN));
        g.stroke();
        g.strokeStyle(Gfx::stroke(Gfx::WHITE.withOpacity(0.5)).withAlign(Gfx::INSIDE_ALIGN));
        g.stroke(Math::Edgef{0, (isize)(FRAME_TIME * 1000 * 2), (f64)bound().width, FRAME_TIME * 1000 * 2});

        for (isize i = 0; i < 256; ++i) {
            auto e = _records[(_index + i) % 256];

            g.plot(
                Math::Recti{i, 0, 1, (isize)e.duration().toMSecs() * 2},
                e.color()
            );
        }

        auto text = Io::format("FPS: {}", (isize)fps()).take();
        Text::Prose gText{Text::ProseStyle{.font = Text::Font::fallback()}};
        gText.append(text.str());
        gText.layout(256);

        g.fillStyle(Gfx::WHITE);
        g.origin({8, 4});
        gText.paint(g);

        g.restore();
    }
};

struct Host : public Node {
    Child _root;
    Opt<Res<>> _res;
    Gfx::Context _g;
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

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();
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

        g.restore();
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

        static usize maxStutter = 31;
        if (elapsed.toMSecs() > maxStutter) {
            logWarn("Stutter detected, paint took {} ms for {} nodes", elapsed.toMSecs(), debugNodeCount);
            maxStutter = elapsed.toMSecs();
        }

        _g.end();

        flip(_dirty);
        _dirty.clear();
    }

    void layout(Math::Recti r) override {
        _perf.record(PerfEvent::LAYOUT);
        _root->layout(r);
        auto elapsed = _perf.end();
        static usize maxStutter = 1;
        if (elapsed.toMSecs() > maxStutter) {
            logWarn("Stutter detected, layout took {}ms for {} nodes", elapsed.toMSecs(), debugNodeCount);
            maxStutter = elapsed.toMSecs();
        }
    }

    void event(Sys::Event &event) override {
        _perf.record(PerfEvent::INPUT);
        _root->event(event);
        auto elapsed = _perf.end();
        static usize maxStutter = 1;
        if (elapsed.toMSecs() > maxStutter) {
            logWarn("Stutter detected, event took {} ms for {} nodes", elapsed.toMSecs(), debugNodeCount);
            maxStutter = elapsed.toMSecs();
        }
    }

    void bubble(Sys::Event &event) override {
        if (auto *e = event.is<Node::PaintEvent>()) {
            _dirty.pushBack(e->bound);
            event.accept();
        } else if (auto *e = event.is<Node::LayoutEvent>()) {
            _shouldLayout = true;
            event.accept();
        } else if (auto *e = event.is<Node::AnimateEvent>()) {
            _shouldAnimate = true;
            event.accept();
        } else if (auto *e = event.is<Events::RequestExitEvent>()) {
            _res = e->res;
            event.accept();
        }

        if (not event.accepted()) {
            logWarn("unhandled event, bouncing down");
            _root->event(event);
        }
    }

    void doLayout() {
        layout(bound());
        _shouldLayout = false;
        _shouldAnimate = true;
        _dirty.pushBack(bound());
    }

    void doPaint() {
        paint();
        _dirty.clear();
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
                auto e = Sys::makeEvent<Node::AnimateEvent>(FRAME_TIME);
                event(*e);
            }

            if (_shouldLayout)
                doLayout();

            if (_dirty.len() > 0)
                doPaint();

            try$(wait(nextFrameScheduled ? nextFrame : TimeStamp::endOfTime()));
            nextFrameScheduled = false;
        }

        return _res.unwrap();
    }
};

} // namespace Karm::Ui
