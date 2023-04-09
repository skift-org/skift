#pragma once

#include <karm-base/ring.h>
#include <karm-sys/time.h>

#include "node.h"

namespace Karm::Ui {

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

    void record(PerfEvent e) {
        _records[_index % 256] = PerfRecord{e, Sys::now(), 0};
    }

    auto end() {
        auto n = Sys::now();
        auto elapsed = n - _records[_index % 256].start;
        _records[_index++ % 256].end = n;
        return elapsed;
    }

    Math::Recti bound() {
        return {0, 0, 256, 100};
    }

    void paint(Gfx::Context &g) {
        for (usize i = 0; i < 256; ++i) {
            auto e = _records[(_index + i) % 256];

            g.debugRect(
                {(isize)i, 0, 1, (isize)e.duration().toMSecs()},
                e.color());
        }
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

    virtual void wait(usize ms) = 0;

    bool alive() {
        return not _res;
    }

    Math::Recti bound() override {
        return pixels().bound();
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
        if (debugShowPerfGraph)
            _dirty.pushBack({0, 0, 256, 100});

        _g.begin(mutPixels());

        _perf.record(PerfEvent::PAINT);
        for (auto &d : _dirty) {
            paint(_g, d);
        }
        _perf.end();

        if (debugShowPerfGraph)
            _perf.paint(_g);

        _g.end();

        flip(_dirty);
        _dirty.clear();
    }

    void event(Events::Event &e) override {
        _perf.record(PerfEvent::INPUT);
        _root->event(e);
        auto elapsed = _perf.end();
        if (elapsed.toMSecs() > 1) {
            logWarn("Event took {}ms", elapsed.toMSecs());
        }
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
        }
    }

    Res<> run() {
        layout(bound());
        paint();
        while (not _res) {
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

        return _res.unwrap();
    }
};

} // namespace Karm::Ui
