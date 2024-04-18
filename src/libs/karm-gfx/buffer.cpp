#include "buffer.h"

namespace Karm::Gfx {

[[gnu::flatten]] void blitUnsafe(MutPixels dst, Pixels src) {
    if (dst.width() != src.width() or dst.height() != src.height()) [[unlikely]]
        panic("blitUnsafe() called with buffers of different sizes");

    dst._fmt.visit([&](auto fd) {
        src._fmt.visit([&](auto fs) {
            for (isize y = 0; y < dst.height(); y++) {
                for (isize x = 0; x < dst.width(); x++) {
                    auto c = fs.load(src.pixelUnsafe({x, y}));
                    fd.store(dst.pixelUnsafe({x, y}), c);
                }
            }
        });
    });
}

} // namespace Karm::Gfx
