module;

#include <karm-gfx/canvas.h>
#include <karm-logger/logger.h>

export module Karm.Image:png.decoder;

import Karm.Core;
import Karm.Archive;
import Karm.Debug;

namespace Karm::Image::Png {

static Debug::Flag debugPng{"png"};

enum struct ColorType : u8 {
    GREYSCALE = 0,
    TRUECOLOR = 2,
    INDEXED = 3,
    GREYSCALE_ALPHA = 4,
    TRUECOLOR_ALPHA = 6,

    _LEN
};

enum struct CompressionMethod : u8 {
    DEFLATE = 0,

    _LEN
};

enum struct FilterMethod : u8 {
    STANDARD = 0,

    _LEN
};

enum struct Filter : u8 {
    NONE,
    SUB,
    UP,
    AVERAGE,
    PAETH,

    _LEN
};

enum struct InterlacingMethod : u8 {
    NULL,
    ADAM7,

    _LEN,
};

static constexpr Array<u8, 8> SIG = {
    0x89, 0x50, 0x4E, 0x47,
    0x0D, 0x0A, 0x1A, 0x0A
};

static constexpr Str IHDR = "IHDR";
static constexpr Str PLTE = "PLTE";
static constexpr Str IDAT = "IDAT";
static constexpr Str IEND = "IEND";

export struct Decoder {
    Bytes _slice;

    Bytes sig() {
        return begin().nextBytes(8);
    }

    static bool sniff(Bytes slice) {
        return slice.len() >= 8 and sub(slice, 0, 8) == SIG;
    }

    static Res<Decoder> init(Bytes slice) {
        Decoder dec{slice};

        if (dec.sig() != SIG)
            return Error::invalidData("invalid signature");

        try$(dec._loaderHeader());

        return Ok(dec);
    }

    Decoder(Bytes slice)
        : _slice(slice) {}

    Io::BScan begin() const {
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

        return Iter{[s] mutable -> Opt<Chunk> {
            if (s.ended())
                return NONE;

            Chunk c;

            c.len = s.nextI32be();
            c.sig = s.nextStr(4);
            c.data = s.nextBytes(c.len);
            c.crc32 = s.nextI32be();

            return c;
        }};
    }

    // MARK: Header ------------------------------------------------------------

    u32 _width = 0;
    u32 _height = 0;
    u8 _bitDepth = 0;
    ColorType _colorType;
    CompressionMethod _compressionMethod;
    FilterMethod _filterMethod;
    InterlacingMethod _interlacingMethod;

    usize numChannels() const {
        switch (_colorType) {
        case ColorType::GREYSCALE:
            return 1;

        case ColorType::TRUECOLOR:
            return 3;

        case ColorType::INDEXED:
            return 1;

        case ColorType::GREYSCALE_ALPHA:
            return 2;

        case ColorType::TRUECOLOR_ALPHA:
            return 4;

        default:
            unreachable();
        }
    }

    usize bitsPerPixel() const {
        return _bitDepth * numChannels();
    }

    usize bytesPerPixel() const {
        return bitsPerPixel() / 8;
    }

    isize width() const {
        return _width;
    }

    isize height() const {
        return _height;
    }

    Res<> _loaderHeader() {
        auto maybeIhdr = iterChunks().next();
        if (not maybeIhdr)
            return Error::invalidData("missing first chunk");

        if (maybeIhdr->sig != IHDR)
            return Error::invalidData("missing image header");

        Io::BScan ihdr = maybeIhdr->data;
        try$(_handleIhdr(ihdr));
        return Ok();
    }

    Res<> _handleIhdr(Io::BScan& s) {
        _width = s.nextU32be();
        _height = s.nextU32be();
        _bitDepth = s.nextU8be();
        _colorType = static_cast<ColorType>(s.nextU8be());
        _compressionMethod = static_cast<CompressionMethod>(s.nextU8be());
        _filterMethod = static_cast<FilterMethod>(s.nextU8be());
        _interlacingMethod = static_cast<InterlacingMethod>(s.nextU8be());

        return Ok();
    }

    // MARK: Palette -----------------------------------------------------------

    Vec<Gfx::Color> _palette;

    Res<> _handlePlte(Io::BScan& s) {
        while (not s.ended()) {
            u8 red = s.nextU8le();
            u8 green = s.nextU8le();
            u8 blue = s.nextU8le();
            _palette.pushBack(Gfx::Color{red, green, blue});
        }

        return Ok();
    }

    // MARK: Image Data --------------------------------------------------------

    Io::BufferWriter _compressedData;

    Res<> _handleIdat(Io::BScan& s) {
        try$(_compressedData.write(s.remBytes()));

        return Ok();
    }

    // MARK: Image End ---------------------------------------------------------

    bool _ended = false;

    Res<> _handleIend(Io::BScan&) {
        if (_ended)
            return Error::invalidData("multiple image end");
        _ended = true;

        return Ok();
    }

    // MARK: Unfiltering -------------------------------------------------------

    static u8 paeth(u16 a, u16 b, u16 c) {
        u16 pa = Math::abs(b - c);
        u16 pb = Math::abs(a - c);
        u16 pc = Math::abs(a + b - c - c);

        if (pb < pa) {
            a = b;
            pa = pb;
        }

        return (pc < pa) ? c : a;
    }

    // https://www.w3.org/TR/2003/REC-PNG-20031110/#9Filter-types
    auto _unfilter(
        Filter filter,
        usize x,
        usize bytesPerPixel,

        Bytes filt,
        MutBytes curr,
        Bytes prev
    ) {
        u8 a = 0;
        if (x >= bytesPerPixel)
            a = curr[x - bytesPerPixel];

        u8 b = 0;
        if (prev)
            b = prev[x];

        u8 c = 0;
        if (prev and x >= bytesPerPixel)
            c = prev[x - bytesPerPixel];

        if (filter == Filter::NONE) {
            // Recon(x) = Filt(x)
            curr[x] = filt[x];
        } else if (filter == Filter::SUB) {
            // Recon(x) = Filt(x) + Recon(a)
            curr[x] = filt[x] + a;
        } else if (filter == Filter::UP) {
            // Recon(x) = Filt(x) + Recon(b)
            curr[x] = filt[x] + b;
        } else if (filter == Filter::AVERAGE) {
            // Recon(x) = Filt(x) + floor((Recon(a) + Recon(b)) / 2)
            curr[x] = filt[x] + (a + b) / 2;
        } else if (filter == Filter::PAETH) {
            // Recon(x) = Filt(x) + PaethPredictor(Recon(a), Recon(b), Recon(c))
            curr[x] =
                filt[x] +
                paeth(
                    (a < 0 ? 0 : a),
                    (b < 0 ? 0 : b),
                    (c < 0 ? 0 : c)
                );
        } else {
            unreachable();
        }
    };

    Res<Vec<u8>> _unfilter(Bytes data) {
        usize bytePerPixel = bitsPerPixel() < 8 ? 1 : bitsPerPixel() / 8;
        usize bytesPerScan = bytePerPixel * width();

        Vec<u8> res;
        res.resize(bytesPerScan * height());

        Io::BScan s = data;
        Bytes prev = {};
        for (usize scanline : range(height())) {
            Filter filter = static_cast<Filter>(s.nextU8be());
            auto filt = s.nextBytes(bytesPerScan);
            MutBytes curr = mutSub(res, bytesPerScan * scanline, bytesPerScan * scanline + bytesPerScan);

            for (usize x : range(bytesPerScan))
                _unfilter(
                    filter,
                    x,
                    bytePerPixel,
                    filt,
                    curr,
                    prev
                );

            prev = curr;
        }

        return Ok(std::move(res));
    }

    // MARK: Decoding ----------------------------------------------------------

    Res<> _decodeScanline(Io::BScan& s, Gfx::MutPixels out, usize scanline) {
        for (usize i : range(_width)) {
            auto r = s.nextU8be();
            auto g = s.nextU8be();
            auto b = s.nextU8be();
            u8 a = 255;
            if (_colorType == ColorType::TRUECOLOR_ALPHA)
                a = s.nextU8be();
            out.store(Math::Vec2u{i, scanline}.cast<isize>(), Gfx::Color{r, g, b, a});
        }
        return Ok();
    }

    Res<> _decodeImage(Gfx::MutPixels out, Bytes unfiltered) {
        Io::BScan s = unfiltered;
        for (usize const scanline : range(_height)) {
            try$(_decodeScanline(s, out, scanline));
        }
        return Ok();
    }

    Res<> decode(Gfx::MutPixels out) {
        for (auto chunk : iterChunks()) {
            Io::BScan data = chunk.data;
            if (chunk.sig == IHDR)
                try$(_handleIhdr(data));
            else if (chunk.sig == PLTE)
                try$(_handlePlte(data));
            else if (chunk.sig == IDAT)
                try$(_handleIdat(data));
            else if (chunk.sig == IEND)
                try$(_handleIend(data));
            else
                logWarnIf(debugPng, "unknow chunk {#}", chunk.sig);
        }

        if (not _ended)
            return Error::invalidData("missing image end");

        logDebugIf(debugPng, "{}", *this);

        if (_bitDepth != 8)
            return Error::invalidData("unsupported bit depth");

        if (_compressionMethod != CompressionMethod::DEFLATE)
            return Error::invalidData("unsupported compression methode");

        if (_filterMethod != FilterMethod::STANDARD)
            return Error::invalidData("unsupported filter methode");

        if (_interlacingMethod != InterlacingMethod::NULL)
            return Error::invalidData("unsupported interlacing methode");

        auto imageData = try$(Archive::zlibDecompress(_compressedData.bytes()));
        auto unfiltered = try$(_unfilter(imageData));
        try$(_decodeImage(out, unfiltered));

        return Ok();
    }

    void repr(Io::Emit& e) const {
        e("PNG image");

        e.indentNewline();

        e.ln("width: {}", _width);
        e.ln("height: {}", _height);
        e.ln("bit depth: {}", _bitDepth);
        e.ln("color type: {}", _colorType);
        e.ln("compression method: {}", _compressionMethod);
        e.ln("filter method: {}", _filterMethod);
        e.ln("interlacing method: {}", _interlacingMethod);

        e.deindent();
    }
};

} // namespace Karm::Image::Png
