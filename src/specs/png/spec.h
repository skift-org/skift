#pragma once

#include <karm-base/string.h>
#include <karm-gfx/context.h>
#include <karm-logger/logger.h>

#include "../bscan.h"

namespace Png {

struct Ihdr : public BChunk {
    static constexpr Str SIG = "IHDR";

    Math::Vec2i size() {
        auto s = begin();
        return {
            (isize)s.nextU32be(),
            (isize)s.nextU32be(),
        };
    }

    u8 bitDepth() {
        return begin().skip(8).nextU8be();
    }

    u8 colorType() {
        return begin().skip(9).nextU8be();
    }

    u8 compressionMethod() {
        return begin().skip(10).nextU8be();
    }

    u8 filterMethod() {
        return begin().skip(11).nextU8be();
    }

    u8 interlaceMethod() {
        return begin().skip(12).nextU8be();
    }
};

struct Plte : public BChunk {
    static constexpr Str SIG = "PLTE";
};

struct Idat : public BChunk {
    static constexpr Str SIG = "IDAT";
};

struct Iend : public BChunk {
    static constexpr Str SIG = "IEND";
};

struct Image {
    static constexpr Array<u8, 8> SIG = {
        0x89, 0x50, 0x4E, 0x47,
        0x0D, 0x0A, 0x1A, 0x0A};

    Bytes _slice;

    Ihdr _ihdr;
    Plte _plte;
    Idat _idat;

    Bytes sig() {
        return begin().nextBytes(8);
    }

    static bool isPng(Bytes slice) {
        return slice.len() >= 8 and Op::eq(sub(slice, 0, 8), bytes(SIG));
    }

    static Res<Image> load(Bytes slice) {
        Image image{slice};

        if (not Op::eq(image.sig(), bytes(SIG)))
            return Error::invalidData("invalid signature");

        image._ihdr = image.lookupChunk<Ihdr>();
        image._plte = image.lookupChunk<Plte>();
        image._idat = image.lookupChunk<Idat>();

        return Ok(image);
    }

    Image(Bytes slice)
        : _slice(slice) {}

    BScan begin() const {
        return _slice;
    }

    auto iterChunks() {
        auto s = begin();
        s.skip(8);

        struct Chunk {
            Str sig;
            usize len;
            Bytes data;
            u32 crc32;
        };

        return Iter{[s]() mutable -> Opt<Chunk> {
            Chunk c;

            c.len = s.nextI32be();
            c.sig = s.nextStr(4);
            c.data = s.nextBytes(c.len);
            c.crc32 = s.nextI32be();

            if (Op::eq(c.sig, Iend::SIG)) {
                return NONE;
            }

            return c;
        }};
    }

    template <typename T>
    T lookupChunk() {
        for (auto chunk : iterChunks()) {
            if (Op::eq(chunk.sig, T::SIG)) {
                return T{chunk.data};
            }
        }

        logWarn("png: '{}' chunk not found", T::SIG);
        return T{};
    }

    isize width() {
        return _ihdr.size().x;
    }

    isize height() {
        return _ihdr.size().y;
    }

    Res<> decode(Gfx::MutPixels) {
        return Ok();
    }
};

} // namespace Png
