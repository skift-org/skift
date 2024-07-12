#include <karm-gfx/context.h>
#include <karm-sys/entry.h>
#include <karm-sys/time.h>

Async::Task<> entryPointAsync(Sys::Context &) {
    for (isize size = 100; size < 1000; size += 10) {
        auto start = Sys::now();
        for (isize i = 0; i < 100; i++) {
            f64 scale = size / 100.0;

            auto surface = Gfx::Surface::alloc({size, size});
            Gfx::Context g;
            g.begin(surface->mutPixels());
            g.scale(scale);

            for (isize i = 0; i < 50; i++) {
                Math::Rand rand{};

                f64 s = rand.nextInt(4, 10);
                s *= s;

                g.begin();
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

        Sys::println("size: {} elapsed: {}", size, elapsed.toUSecs() / 100);
    }

    co_return Ok();
}
