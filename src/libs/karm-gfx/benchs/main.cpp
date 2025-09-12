#include <karm-gfx/cpu/canvas.h>
#include <karm-sys/entry.h>
#include <karm-sys/time.h>

import Karm.Cli;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context&) {
    Vec<Duration> samples;
    auto surface = Gfx::Surface::alloc({1000, 1000});

    for (isize i = 0; i < 100; i++) {
        auto start = Sys::now();
        for (isize size = 100; size < 1000; size += 10) {
            f64 scale = size / 100.0;

            Gfx::CpuCanvas g;
            g.begin(surface->mutPixels());
            g.scale(scale);

            for (isize i = 0; i < 50; i++) {
                Math::Rand rand{};

                f64 s = rand.nextInt(4, 10);
                s *= s;

                g.beginPath();
                g.ellipse({
                    rand.nextVec2(Math::Recti{100, 100}).cast<f64>(),
                    s,
                });

                g.strokeStyle(
                    Gfx::stroke(Gfx::randomColor(rand))
                        .withWidth(rand.nextInt(2, s))
                );
                g.stroke();
            }
        }

        auto elapsed = Sys::now() - start;
        samples.pushBack(elapsed);

        Sys::print("sampling {}/100: {}\r", i + 1, elapsed);
    }

    // median
    sort(samples, [](auto& a, auto& b) {
        return a.toUSecs() <=> b.toUSecs();
    });

    // average
    f64 sum = 0;
    for (auto& s : samples)
        sum += s.toUSecs();

    Sys::println("\n");
    Sys::println("median: {}", samples[samples.len() / 2]);
    Sys::println("average: {}", Duration::fromUSecs(sum / samples.len()));
    Sys::println("min: {}", first(samples));
    Sys::println("max: {}", last(samples));

    co_return Ok();
}
