#include "encoder.h"

namespace Tga {

Res<> encode(Gfx::Pixels pixels, Io::BEmit &e) {
    Tga::Header header = {};
    header.width = pixels.width();
    header.height = pixels.height();
    header.bpp = 32;
    header.imageType = Tga::UNC_TC;
    header.desc = 0;

    e.writeFrom(header);

    for (isize y = pixels.height() - 1; y >= 0; --y)
        for (isize x = 0; x < pixels.width(); ++x) {
            auto color = pixels.load({x, y});
            e.writeU8le(color.blue);
            e.writeU8le(color.green);
            e.writeU8le(color.red);
            e.writeU8le(color.alpha);
        }

    return Ok();
}

} // namespace Tga
