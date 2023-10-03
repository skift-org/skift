#pragma once

#include <karm-base/ring.h>
#include <karm-sys/time.h>

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
    f64 _frameTime = 0;

    void record(PerfEvent e) {
        _records[_index % 256] = PerfRecord{e, Sys::now(), 0};
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
        return {0, 0, 256, 100};
    }

    void paint(Gfx::Context &g) {
        g.save();
        g.clip(bound());
        g.fillStyle(Gfx::GREEN900.withOpacity(0.5));
        g.fill(bound());

        g.strokeStyle(Gfx::stroke(Gfx::GREEN.withOpacity(0.5)).withAlign(Gfx::INSIDE_ALIGN));
        g.stroke();
        g.strokeStyle(Gfx::stroke(Gfx::WHITE.withOpacity(0.5)).withAlign(Gfx::INSIDE_ALIGN));
        g.stroke(Math::Edgei{0, (isize)(FRAME_TIME * 1000 * 2), 256, (isize)(FRAME_TIME * 1000 * 2)});

        for (usize i = 0; i < 256; ++i) {
            auto e = _records[(_index + i) % 256];

            g.debugRect(
                {(isize)i, 0, 1, (isize)e.duration().toMSecs() * 2},
                e.color());
        }

        auto text = Fmt::format("FPS: {}", (isize)fps()).take();
        g.fillStyle(Gfx::WHITE);
        g.fill({8, 16}, text);

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

    virtual void pump() = 0;

    virtual void wait(TimeSpan) = 0;

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
            g.fillStyle(Gfx::randomColor().withOpacity(0.25));
            g.fill(r);
        }

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

        if (elapsed.toMSecs() > 32) {
            logWarn("Paint took {} ms for {} nodes alive", elapsed.toMSecs(), debugNodeCount);
        }

        if (debugShowPerfGraph)
            _perf.paint(_g);

        _g.end();

        flip(_dirty);
        _dirty.clear();
    }

    void event(Async::Event &e) override {
        _perf.record(PerfEvent::INPUT);
        _root->event(e);
        auto elapsed = _perf.end();
        if (elapsed.toMSecs() > 1) {
            logWarn("Event took {}ms", elapsed.toMSecs());
            logDebug("There is {} nodes alive", debugNodeCount);
        }
    }

    void bubble(Async::Event &event) override {
        event
            .handle<Node::PaintEvent>([this](auto &e) {
                _dirty.pushBack(e.bound);
                return true;
            })
            .handle<Node::LayoutEvent>([this](auto &) {
                _shouldLayout = true;
                return true;
            })
            .handle<Node::AnimateEvent>([this](auto &) {
                _shouldAnimate = true;
                return true;
            })
            .handle<Events::ExitEvent>([this](auto &e) {
                _res = e.res;
                return true;
            });
    }

    void layout(Math::Recti r) override {
        _perf.record(PerfEvent::LAYOUT);
        _root->layout(r);
        auto elapsed = _perf.end();
        if (elapsed.toMSecs() > 1) {
            logWarn("Layout took {}ms", elapsed.toMSecs());
            logDebug("There is {} nodes alive", debugNodeCount);
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
        doLayout();
        doPaint();

        auto lastFrame = Sys::now();
        while (not _res) {
            isize waitTime = -1;
            if (_shouldAnimate) {
                auto elapsed = Sys::now() - lastFrame;
                waitTime = ((FRAME_TIME * 1000) - elapsed.toMSecs());
                if (waitTime < 0)
                    waitTime = 0;
            }

            wait(TimeSpan::fromMSecs(waitTime));
            lastFrame = Sys::now();

            if (_shouldAnimate) {
                _shouldAnimate = false;
                auto e = Async::makeEvent<Node::AnimateEvent>(Async::Propagation::DOWN, FRAME_TIME);
                event(*e);
            }

            pump();

            if (_shouldLayout) {
                doLayout();
            }

            if (_dirty.len() > 0) {
                doPaint();
            }
        }

        return _res.unwrap();
    }
};

} // namespace Karm::Ui
