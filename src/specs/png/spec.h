#pragma once

#include <karm-base/endiant.h>
#include <karm-base/string.h>
#include <karm-debug/logger.h>
#include <karm-gfx/context.h>

#include "../bscan.h"

namespace Png {

struct Ihdr : public BChunk {
    static constexpr Str SIG = "IHDR";

    Math::Vec2i size() {
        auto s = begin();
        return {
            (int)s.nextBeUint32(),
            (int)s.nextBeUint32(),
        };
    }

    uint8_t bitDepth() {
        return begin().skip(8).nextBeUint8();
    }

    uint8_t colorType() {
        return begin().skip(9).nextBeUint8();
    }

    uint8_t compressionMethod() {
        return begin().skip(10).nextBeUint8();
    }

    uint8_t filterMethod() {
        return begin().skip(11).nextBeUint8();
    }

    uint8_t interlaceMethod() {
        return begin().skip(12).nextBeUint8();
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
    static constexpr Array<uint8_t, 8> SIG = {
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
        return slice.len() >= 8 && Op::eq(sub(slice, 0, 8), bytes(SIG));
    }

    static Result<Image> load(Bytes slice) {
        Image image{slice};

        if (!Op::eq(image.sig(), bytes(SIG)))
            return Error("invalid signature");

        image._ihdr = image.lookupChunk<Ihdr>();
        image._plte = image.lookupChunk<Plte>();
        image._idat = image.lookupChunk<Idat>();

        return image;
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
            size_t len;
            Bytes data;
            uint32_t crc32;
        };

        return Iter{[s]() mutable -> Opt<Chunk> {
            Chunk c;

            c.len = s.nextBeInt32();
            c.sig = s.nextStr(4);
            c.data = s.nextBytes(c.len);
            c.crc32 = s.nextBeInt32();

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

        Debug::lwarn("'{}' chunk not found", T::SIG);
        return T{};
    }

    int width() {
        return _ihdr.size().x;
    }

    int height() {
        return _ihdr.size().y;
    }
};

} // namespace Png
