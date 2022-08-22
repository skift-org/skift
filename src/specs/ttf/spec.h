#pragma once

#include <karm-base/endiant.h>
#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-debug/logger.h>
#include <karm-gfx/context.h>
#include <karm-io/impls.h>

// https://tchayen.github.io/posts/ttf-file-parsing
// http://stevehanov.ca/blog/?id=143
// https://www.microsoft.com/typography/otspec/otff.htm
// https://docs.microsoft.com/en-us/typography/opentype/spec/glyf
// https://docs.microsoft.com/en-us/typography/opentype/spec/ttch01
// https://docs.microsoft.com/en-us/typography/opentype/spec/otff
// https://fontdrop.info/

namespace Ttf {

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

    BScan peek(size_t n) {
        BScan c{*this};
        c.skip(n);
        return c;
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
    bool peekTo(T *buf, size_t n) {
        if (rem() < n) {
            return false;
        }

        uint8_t *b = reinterpret_cast<uint8_t *>(buf);
        for (size_t i = 0; i < n; i++) {
            b[i] = _cursor.buf()[i];
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

    template <typename T>
    T peekBe() {
        _Be<T> r{};
        peekTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T peekLe() {
        _Le<T> r;
        peekTo(&r, sizeof(T));
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

    uint8_t peekBeUint8() { return peekBe<uint8_t>(); }
    uint16_t peekBeUint16() { return peekBe<uint16_t>(); }
    uint32_t peekBeUint32() { return peekBe<uint32_t>(); }
    uint64_t peekBeUint64() { return peekBe<uint64_t>(); }

    uint8_t peekLeUint8() { return peekLe<uint8_t>(); }
    uint16_t peekLeUint16() { return peekLe<uint16_t>(); }
    uint32_t peekLeUint32() { return peekLe<uint32_t>(); }
    uint64_t peekLeUint64() { return peekLe<uint64_t>(); }

    int8_t peekBeInt8() { return peekBe<int8_t>(); }
    int16_t peekBeInt16() { return peekBe<int16_t>(); }
    int32_t peekBeInt32() { return peekBe<int32_t>(); }
    int64_t peekBeInt64() { return peekBe<int64_t>(); }

    int8_t peekLeInt8() { return peekLe<int8_t>(); }
    int16_t peekLeInt16() { return peekLe<int16_t>(); }
    int32_t peekLeInt32() { return peekLe<int32_t>(); }
    int64_t peekLeInt64() { return peekLe<int64_t>(); }

    Str nextStr(size_t n) {
        n = clamp(n, 0uz, rem());
        Str s{(char const *)_cursor.buf(), n};
        _cursor.next(n);
        return s;
    }
};

struct Table {
    Bytes _slice{};

    Table() = default;

    Table(Bytes slice) : _slice(slice) {}

    bool present() const {
        return _slice.len() > 0;
    }

    BScan begin() const {
        return _slice;
    }
};

struct Head : public Table {
    static constexpr Str NAME = "head";

    int unitPerEm() const {
        auto s = begin();
        s.skip(18);
        return s.nextBeUint16();
    }

    int locaFormat() const {
        auto s = begin();
        s.skip(50);
        return s.nextBeUint16();
    }
};

struct Maxp : public Table {
    static constexpr Str NAME = "maxp";

    int numGlyphs() const {
        auto s = begin();
        s.skip(4);
        return s.nextBeUint16();
    }
};

struct Cmap : public Table {
    static constexpr Str NAME = "cmap";

    struct Table {
        uint16_t platformId;
        uint16_t encodingId;
        uint16_t type;

        Bytes slice;

        BScan begin() const {
            return slice;
        }

        size_t _glyphIdForType4(Rune r) const {
            uint16_t segCountX2 = begin().skip(6).nextBeUint16();
            uint16_t segCount = segCountX2 / 2;

            for (size_t i = 0; i < segCount; i++) {
                auto s = begin().skip(14);

                uint16_t endCode = s.skip(i * 2).peekBeUint16();

                if (r >= endCode)
                    continue;

                // + 2 for reserved padding
                uint16_t startCode = s.skip(segCountX2 + 2).peekBeUint16();

                if (r < startCode)
                    break;

                uint16_t idDelta = s.skip(segCountX2).peekBeInt16();
                uint16_t idRangeOffset = s.skip(segCountX2).peekBeUint16();

                if (idRangeOffset == 0) {
                    return (r + idDelta) & 0xFFFF;
                } else {
                    auto offset = idRangeOffset + (r - startCode) * 2;
                    return s.skip(offset).nextBeUint16();
                }
            }

            return 0;
        }

        size_t glyphIdFor(Rune r) const {
            if (type == 4) {
                return _glyphIdForType4(r);
            } else {
                return 0;
            }
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
};

struct Loca : public Table {
    static constexpr Str NAME = "loca";

    size_t glyfOffset(int glyphId, Head const &head) const {
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

struct Glyf : public Table {
    static constexpr Str NAME = "glyf";

    static constexpr uint8_t ON_CURVE_POINT = 0x01;
    static constexpr uint8_t X_SHORT_VECTOR = 0x02;
    static constexpr uint8_t Y_SHORT_VECTOR = 0x04;
    static constexpr uint8_t REPEAT = 0x08;
    static constexpr uint8_t SAME_OR_POSITIVE_X = 0x10;
    static constexpr uint8_t SAME_OR_POSITIVE_Y = 0x20;
    static constexpr uint8_t OVERLAY_SIMPLE = 0x40;

    struct Metrics {
        int numContours;
        int xMin;
        int yMin;
        int xMax;
        int yMax;
    };

    Metrics metrics(BScan &s, size_t glyfOffset) const {
        auto offset = glyfOffset;
        s.skip(offset);
        auto numContours = s.nextBeInt16();
        if (numContours == 0) {
            return {};
        }
        auto xMin = s.nextBeInt16();
        auto yMin = s.nextBeInt16();
        auto xMax = s.nextBeInt16();
        auto yMax = s.nextBeInt16();
        return {numContours, xMin, yMin, xMax, yMax};
    }

    Metrics metrics(size_t glyfOffset) const {
        auto s = begin();
        return metrics(s, glyfOffset);
    }

    struct SimpleContour {
        enum Flag {
            ON_CURVE_POINT = 0x01,
            X_SHORT_VECTOR = 0x02,
            Y_SHORT_VECTOR = 0x04,
            REPEAT = 0x08,
            X_SAME_OR_POSITIVE_X_SHORT_VECTOR = 0x10,
            Y_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 0x20,
            OVERLAP_SIMPLE = 0x40,
        };
        uint8_t flags;
        int16_t x;
        int16_t y;
    };

    void contourSimple(Gfx::Context &g, Metrics m, BScan &s, Math::Vec2f baseline, double size) const {
        auto endPtsOfContours = s;
        auto nPoints = s.peek(2 * (m.numContours - 1)).nextBeUint16() + 1u;
        uint16_t instructionLength = s.skip(m.numContours * 2).nextBeUint16();
        // auto instructions = s;

        auto flagsScan = s.skip(instructionLength);

        size_t nXCoords = 0;
        uint8_t flags = 0;
        uint8_t flagsRepeat = 0;

        for (size_t i = 0; i < nPoints; i++) {
            if (!flagsRepeat) {
                flags = s.nextBeUint8();
                if (flags & REPEAT) {
                    flagsRepeat = s.nextBeUint8();
                }
            } else {
                flagsRepeat--;
            }

            nXCoords += flags & SimpleContour::X_SHORT_VECTOR
                            ? 1
                            : (flags & SAME_OR_POSITIVE_X ? 0 : 2);
        }

        auto xCoordsScan = s;
        auto yCoordsScan = s.skip(nXCoords);

        size_t start = 0;
        Math::Vec2i curr{};
        for (int c = 0; c < m.numContours; c++) {
            size_t end = endPtsOfContours.nextBeUint16();
            uint8_t flags = 0;
            uint8_t flagsRepeat = 0;

            Math::Vec2f cp{};
            Math::Vec2f startP{};
            bool wasCp = false;

            for (size_t i = start; i <= end; i++) {
                if (!flagsRepeat) {
                    flags = flagsScan.nextBeUint8();
                    if (flags & REPEAT) {
                        flagsRepeat = flagsScan.nextBeUint8();
                    }
                } else {
                    flagsRepeat--;
                }

                int x = (flags & X_SHORT_VECTOR)
                            ? ((flags & SAME_OR_POSITIVE_X) ? xCoordsScan.nextBeUint8() : -xCoordsScan.nextBeUint8())
                            : ((flags & SAME_OR_POSITIVE_X) ? 0 : xCoordsScan.nextBeInt16());

                int y = (flags & Y_SHORT_VECTOR)
                            ? ((flags & SAME_OR_POSITIVE_Y) ? yCoordsScan.nextBeUint8() : -yCoordsScan.nextBeUint8())
                            : ((flags & SAME_OR_POSITIVE_Y) ? 0 : yCoordsScan.nextBeInt16());

                curr = curr + Math::Vec2i{x, -y};
                auto p = baseline + (curr.cast<double>() * size);

                if (i == start) {
                    g.moveTo(p);
                    startP = p;
                } else {
                    if (flags & ON_CURVE_POINT) {
                        if (wasCp) {
                            g.quadTo(cp, p);
                        } else {
                            g.lineTo(p);
                        }
                        wasCp = false;
                    } else {
                        if (wasCp) {
                            auto p1 = (cp + p) / 2;
                            g.quadTo(cp, p1);
                            cp = p;
                            wasCp = true;
                        } else {
                            cp = p;
                            wasCp = true;
                        }
                    }
                }
            }

            if (wasCp) {
                g.quadTo(cp, startP);
            }

            g.close();
            start = end + 1;
        }
    }

    void contourComposite(Gfx::Context &, Metrics, BScan &) const {
        Debug::lwarn("Glyf::strokeComposite not implemented");
    }

    void contour(Gfx::Context &g, size_t glyfOffset, Math::Vec2f baseline, double size) const {
        auto s = begin();
        auto m = metrics(s, glyfOffset);

        if (m.numContours > 0) {
            contourSimple(g, m, s, baseline, size);
        } else if (m.numContours < 0) {
            contourComposite(g, m, s);
        }
    }
};

struct Hhea : public Table {
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
        s.skip(34);
        return s.nextBeUint16();
    }
};

struct Hmtx : public Table {
    static constexpr Str NAME = "hmtx";
    static constexpr int LONG_RECORD_SIZE = 4;
    static constexpr int SHORT_RECORD_SIZE = 2;

    struct Metrics {
        int advanceWidth;
        int lsb;
    };

    Metrics metrics(int glyphId, Hhea const &hhea) const {
        if (glyphId < hhea.numberOfHMetrics()) {
            auto s = begin().skip(glyphId * 4);
            return {s.nextBeUint16(), s.nextBeInt16()};
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

struct Name : public Table {
    static constexpr Str NAME = "name";
};

struct Os2 : public Table {
    static constexpr Str NAME = "OS/2";
};

struct Kern : public Table {
    static constexpr Str NAME = "kern";
};

struct GlyphMetrics {
    double x;
    double y;
    double width;
    double height;
    double lsb;
    double advance;
};

struct Metrics {
    double ascend;
    double descend;
    double linegap;
    double maxWidth;
};

struct Font {
    Bytes _slice;

    Head _head;
    Cmap _cmap;
    Cmap::Table _cmapTable;
    Glyf _glyf;
    Loca _loca;
    Hhea _hhea;
    Hmtx _hmtx;
    Name _name;
    Kern _kern;

    static Result<Cmap::Table> chooseCmap(Font &font) {
        for (auto table : font._cmap.iterTables()) {
            if (table.platformId == 3 &&
                table.encodingId == 1 &&
                table.type == 4) {
                return table;
            }
        }
        Debug::lerror("No suitable cmap table found");
        return Error("no cmap table");
    }

    static Result<Font> load(Bytes slice) {
        Font font{slice};

        if (font.version() != 0x00010000 &&
            font.version() != 0x4F54544F) {
            Debug::lerror("Version {x} is not supported", (uint64_t)font.version());
            return Error{"invalid version"};
        }

        font._head = try$(font.requireTable<Head>());
        font._cmap = try$(font.requireTable<Cmap>());
        font._cmapTable = try$(chooseCmap(font));
        font._glyf = try$(font.requireTable<Glyf>());
        font._loca = try$(font.requireTable<Loca>());
        font._hhea = try$(font.requireTable<Hhea>());
        font._hmtx = try$(font.requireTable<Hmtx>());
        font._name = font.lookupTable<Name>();
        font._kern = font.lookupTable<Kern>();

        return font;
    }

    Font(Bytes slice) : _slice(slice) {}

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

            i++;
            return table;
        }};
    }

    template <typename T>
    T lookupTable() {
        for (auto table : iterTables()) {
            if (Op::eq(table.tag, T::NAME)) {
                return T{sub(_slice, table.offset, table.offset + table.length)};
            }
        }

        Debug::lwarn("'{}' table not found", T::NAME);
        return T{};
    }

    template <typename T>
    Result<T> requireTable() {
        for (auto table : iterTables()) {
            if (Op::eq(table.tag, T::NAME)) {
                return T{sub(_slice, table.offset, table.offset + table.length)};
            }
        }

        Debug::lerror("'{}' table not found", T::NAME);
        return Error{"table not found"};
    }

    GlyphMetrics glyphMetrics(Rune rune) const {
        auto glyphId = _cmapTable.glyphIdFor(rune);

        auto glyfOffset = _loca.glyfOffset(glyphId, _head);
        auto glyf = _glyf.metrics(glyfOffset);
        auto hmtx = _hmtx.metrics(glyphId, _hhea);

        return {
            glyf.xMin / (double)_head.unitPerEm(),
            -glyf.yMax / (double)_head.unitPerEm(),
            (glyf.xMax - glyf.xMin) / (double)_head.unitPerEm(),
            (glyf.yMax - glyf.yMin) / (double)_head.unitPerEm(),
            hmtx.lsb / (double)_head.unitPerEm(),
            hmtx.advanceWidth / (double)_head.unitPerEm(),
        };
    }

    void glyphContour(Rune rune, Gfx::Context &g, Math::Vec2f baseline, double size) const {
        auto glyphId = _cmapTable.glyphIdFor(rune);
        auto glyfOffset = _loca.glyfOffset(glyphId, _head);

        if (glyfOffset == _loca.glyfOffset(glyphId + 1, _head))
            return;

        _glyf.contour(g, glyfOffset, baseline, (1 / (double)_head.unitPerEm()) * size);
    }

    Metrics metrics() const {
        return {
            _hhea.ascender() / (double)_head.unitPerEm(),
            _hhea.descender() / (double)_head.unitPerEm(),
            _hhea.lineGap() / (double)_head.unitPerEm(),
            _hhea.advanceWidthMax() / (double)_head.unitPerEm(),
        };
    }
};

} // namespace Ttf
