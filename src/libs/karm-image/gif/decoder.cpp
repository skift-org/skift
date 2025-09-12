module;

#include <karm-gfx/buffer.h>

export module Karm.Image:gif.decoder;

import Karm.Core;

// GIF Image decoder
// References:
//  - https://www.w3.org/Graphics/GIF/spec-gif89a.txt

namespace Karm::Image::Gif {

struct Decoder {
    struct Header {
        Array<u8, 6> signature;
    };

    struct LogicalScreenDescriptor {
        u16le width;
        u16le height;
        u8 packed;
        u8 backgroundColorIndex;
        u8 pixelAspectRatio;
    };

    static bool sniff(Bytes slice) {
        bool isGif = slice.len() >= 6 and
                     slice[0] == 'G' and
                     slice[1] == 'I' and
                     slice[2] == 'F';

        bool isGif89a = slice.len() >= 6 and
                        slice[3] == '8' and
                        slice[4] == '9' and
                        slice[5] == 'a';

        bool isGif87a = slice.len() >= 6 and
                        slice[3] == '8' and
                        slice[4] == '7' and
                        slice[5] == 'a';

        return isGif and (isGif89a or isGif87a);
    }

    static Res<Decoder> init(Bytes slice) {
        if (not sniff(slice))
            return Error::invalidData("invalid signature");

        Decoder dec{};
        Io::BScan scan{slice};

        Header header{};
        scan.readTo(&header);

        LogicalScreenDescriptor lsd{};
        scan.readTo(&lsd);

        dec._size = {lsd.width, lsd.height};

        return Ok(dec);
    }

    Math::Vec2i _size;

    isize width() const { return _size.x; }

    isize height() const { return _size.y; }

    Res<> decode(Gfx::MutPixels pixels) {
        (void)pixels;
        return Ok();
    }
};

} // namespace Karm::Image::Gif
