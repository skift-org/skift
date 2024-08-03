#pragma once

#include <karm-gfx/context.h>
#include <karm-sys/time.h>
#include <karm-text/prose.h>

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

} // namespace Karm::Ui
