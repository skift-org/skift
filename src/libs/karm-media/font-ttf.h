#pragma once

#include <karm-base/endiant.h>
#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-debug/logger.h>
#include <karm-gfx/context.h>
#include <karm-io/impls.h>

#include "font.h"

// https://tchayen.github.io/posts/ttf-file-parsing
// http://stevehanov.ca/blog/?id=143
// https://www.microsoft.com/typography/otspec/otff.htm

namespace Karm::Media {

struct BScan {
    Cursor<uint8_t> _cursor;

    BScan(Bytes bytes) : _cursor(bytes) {}

    bool ended() {
        return _cursor.ended();
    }

    size_t rem() {
        return _cursor.rem();
    }

    BScan &skip(size_t n) {
        n = min(n, rem());
        _cursor.next(n);
        return *this;
    }

    template <typename T>
    bool readTo(T *buf, size_t n) {
        if (rem() < n) {
            return false;
        }

        uint8_t *b = reinterpret_cast<uint8_t *>(buf);
        for (size_t i = 0; i < n; i++) {
            b[i] = _cursor.next();
        }
        return true;
    }

    template <typename T>
    T nextBe() {
        _Be<T> r{};
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T nextLe() {
        _Le<T> r;
        readTo(&r, sizeof(T));
        return r;
    }

    uint8_t nextBeUint8() { return nextBe<uint8_t>(); }

    uint16_t nextBeUint16() { return nextBe<uint16_t>(); }

    uint32_t nextBeUint32() { return nextBe<uint32_t>(); }

    uint64_t nextBeUint64() { return nextBe<uint64_t>(); }

    uint8_t nextLeUint8() { return nextLe<uint8_t>(); }

    uint16_t nextLeUint16() { return nextLe<uint16_t>(); }

    uint32_t nextLeUint32() { return nextLe<uint32_t>(); }

    uint64_t nextLeUint64() { return nextLe<uint64_t>(); }

    int8_t nextBeInt8() { return nextBe<int8_t>(); }

    int16_t nextBeInt16() { return nextBe<int16_t>(); }

    int32_t nextBeInt32() { return nextBe<int32_t>(); }

    int64_t nextBeInt64() { return nextBe<int64_t>(); }

    int8_t nextLeInt8() { return nextLe<int8_t>(); }

    int16_t nextLeInt16() { return nextLe<int16_t>(); }

    int32_t nextLeInt32() { return nextLe<int32_t>(); }

    int64_t nextLeInt64() { return nextLe<int64_t>(); }

    Str nextStr(size_t n) {
        n = clamp(n, 0uz, rem());
        Str s{(char const *)_cursor.buf(), n};
        _cursor.next(n);
        return s;
    }
};

struct TtfTable {
    Bytes _slice{};

    TtfTable() = default;

    TtfTable(Bytes slice) : _slice(slice) {}

    bool present() const {
        return _slice.len() > 0;
    }

    BScan begin() const {
        return _slice;
    }
};

struct TtfHead : public TtfTable {
    static constexpr Str NAME = "head";

    int unitPerEm() const {
        auto s = begin();
        s.skip(14);
        return s.nextBeUint16();
    }

    int locaFormat() const {
        auto s = begin();
        s.skip(50);
        return s.nextBeUint16();
    }
};

struct TtfMaxp : public TtfTable {
    static constexpr Str NAME = "maxp";

    int numGlyphs() const {
        auto s = begin();
        s.skip(4);
        return s.nextBeUint16();
    }
};

struct TtfCmap : public TtfTable {
    static constexpr Str NAME = "cmap";

    struct Table {
        uint16_t platformId;
        uint16_t encodingId;
        uint16_t type;

        Bytes slice;

        BScan begin() const {
            return slice;
        }
    };

    auto iterTables() {
        auto s = begin();
        s.skip(2);
        size_t numTables = s.nextBeUint16();

        return Iter{[this, s, i = 0uz, numTables]() mutable -> Opt<Table> {
            if (i == numTables) {
                return NONE;
            }

            uint16_t platformId = s.nextBeUint16();
            uint16_t encodingId = s.nextBeUint16();
            uint32_t offset = s.nextBeUint32();
            auto slice = sub(_slice, offset, _slice.len());
            uint16_t type = BScan{slice}.nextBeUint16();
            i++;

            return Table{platformId, encodingId, type, slice};
        }};
    }

    int glyphIdFor(Rune) {
        // TODO

        return 0;
    }
};

struct TtfLoca : public TtfTable {
    static constexpr Str NAME = "loca";

    size_t glyfOffset(int glyphId, TtfHead const &head) const {
        auto s = begin();
        if (head.locaFormat() == 0) {
            s.skip(glyphId * 2);
            return s.nextBeUint16();
        } else {
            s.skip(glyphId * 4);
            return s.nextBeUint32();
        }
    }
};

struct TtfGlyf : public TtfTable {
    static constexpr Str NAME = "glyf";

    struct Metrics {
        size_t numContours;
        int xMin;
        int yMin;
        int xMax;
        int yMax;
    };

    Metrics metrics(size_t glyfOffset) const {
        auto s = begin();
        auto offset = glyfOffset;
        s.skip(offset);
        auto numContours = s.nextBeUint16();
        if (numContours == 0) {
            return {};
        }
        auto xMin = s.nextBeInt16();
        auto yMin = s.nextBeInt16();
        auto xMax = s.nextBeInt16();
        auto yMax = s.nextBeInt16();
        return {numContours, xMin, yMin, xMax, yMax};
    }
};

struct TtfHhea : public TtfTable {
    static constexpr Str NAME = "hhea";

    int ascender() const {
        auto s = begin();
        s.skip(4);
        return s.nextBeInt16();
    }

    int descender() const {
        auto s = begin();
        s.skip(6);
        return s.nextBeInt16();
    }

    int lineGap() const {
        auto s = begin();
        s.skip(8);
        return s.nextBeInt16();
    }

    int advanceWidthMax() const {
        auto s = begin();
        s.skip(10);
        return s.nextBeUint16();
    }

    int numberOfHMetrics() const {
        auto s = begin();
        s.skip(32);
        return s.nextBeUint16();
    }
};

struct TtfHmtx : public TtfTable {
    static constexpr Str NAME = "hmtx";
    static constexpr int LONG_RECORD_SIZE = 4;
    static constexpr int SHORT_RECORD_SIZE = 2;

    struct Metrics {
        int advanceWidth;
        int lsb;
    };

    Metrics metrics(int glyphId, TtfHhea const &hhea) {
        if (glyphId < hhea.numberOfHMetrics()) {
            auto s = begin().skip(glyphId * 4);
            return {s.nextBeUint16(), s.nextBeUint16()};
        } else {
            size_t advOffset = (hhea.numberOfHMetrics() - 1) * LONG_RECORD_SIZE;
            size_t lsbOffset = hhea.numberOfHMetrics() * LONG_RECORD_SIZE +
                               (glyphId - hhea.numberOfHMetrics()) * SHORT_RECORD_SIZE;

            int adv = begin().skip(advOffset).nextBeUint16();
            int lsb = begin().skip(lsbOffset).nextBeUint16();

            return {adv, lsb};
        }
    }
};

struct TtfName : public TtfTable {
    static constexpr Str NAME = "name";
};

struct TtfOs2 : public TtfTable {
    static constexpr Str NAME = "OS/2";
};

struct TtfKern : public TtfTable {
    static constexpr Str NAME = "kern";
};

struct TtfFont : public Font {
    Bytes _slice;

    TtfHead _head;
    TtfCmap _cmap;
    TtfCmap::Table _cmapTable;
    TtfGlyf _glyf;
    TtfLoca _loca;
    TtfHhea _hhea;
    TtfHmtx _hmtx;
    TtfName _name;
    TtfKern _kern;

    static Result<TtfCmap::Table> chooseCmap(TtfFont &font) {
        for (auto table : font._cmap.iterTables()) {
            Debug::ldebug("cmap table: {}, {}, {}", table.platformId, table.encodingId, table.type);
            if (table.platformId == 3 && table.encodingId == 1 && table.type == 4) {
                Debug::ldebug("found cmap table ------------------------------------------");
                return table;
            }
        }
        return Error("no cmap table");
    }

    static Result<TtfFont> load(Bytes slice) {
        TtfFont font{slice};

        if (font.version() != 0x00010000 &&
            font.version() != 0x4F54544F) {
            Debug::lerror("TTF version is not supported: {x}", (uint64_t)font.version());
            return Error{"invalid version"};
        }

        font._head = font.lookupTable<TtfHead>();
        font._cmap = font.lookupTable<TtfCmap>();
        font._cmapTable = try$(chooseCmap(font));
        font._glyf = font.lookupTable<TtfGlyf>();
        font._loca = font.lookupTable<TtfLoca>();
        font._hhea = font.lookupTable<TtfHhea>();
        font._hmtx = font.lookupTable<TtfHmtx>();
        font._name = font.lookupTable<TtfName>();
        font._kern = font.lookupTable<TtfKern>();

        return font;
    }

    TtfFont(Bytes slice) : _slice(slice) {}

    /* --- Parsing Api ------------------------------------------------------ */

    BScan begin() const {
        return _slice;
    }

    uint32_t version() {
        return begin().nextBeUint32();
    }

    auto iterTables() {
        auto scan = begin();
        /* auto version = */ scan.nextBeUint32();
        auto numTables = scan.nextBeUint16();
        /* auto searchRange = */ scan.nextBeUint16();
        /* auto entrySelector = */ scan.nextBeUint16();
        /* auto rangeShift = */ scan.nextBeUint16();

        Debug::ldebug("There is {} tables", numTables);

        struct Table {
            Str tag;
            uint32_t checkSum;
            uint32_t offset;
            uint32_t length;
        };

        return Iter{[scan, i = 0uz, numTables]() mutable -> Opt<Table> {
            if (i == numTables) {
                return NONE;
            }

            Table table{};
            table.tag = scan.nextStr(4);
            table.checkSum = scan.nextBeUint32();
            table.offset = scan.nextBeUint32();
            table.length = scan.nextBeUint32();

            Debug::ldebug("{}: {}: {}", i, table.tag, table.length);

            i++;
            return table;
        }};
    }

    template <typename T>
    T lookupTable() {
        for (auto table : iterTables()) {
            if (Op::eq(table.tag, T::NAME)) {
                Debug::ldebug("Found {}", T::NAME);
                return T{sub(_slice, table.offset, table.offset + table.length)};
            }
        }

        Debug::lerror("'{}' table not found", T::NAME);

        return T{};
    }

    /* --- Rendering API ---------------------------------------------------- */

    FontMetrics metrics() const override {
        return {};
    }

    double advance(Rune) const override {
        return {};
    }

    void traceRune(Gfx::Context &, Math::Vec2i, Rune) const {
    }

    void fillRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        g.begin();
        traceRune(g, baseline, rune);
        g.end();
        g.fill();
    }

    void strokeRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        g.begin();
        traceRune(g, baseline, rune);
        g.end();
        g.stroke();
    }
};

} // namespace Karm::Media
