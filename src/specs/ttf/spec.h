#pragma once

#include <karm-base/endiant.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-gfx/context.h>
#include <karm-logger/logger.h>

#include "../bscan.h"

// https://tchayen.github.io/posts/ttf-file-parsing
// http://stevehanov.ca/blog/?id=143
// https://www.microsoft.com/typography/otspec/otff.htm
// https://docs.microsoft.com/en-us/typography/opentype/spec/glyf
// https://docs.microsoft.com/en-us/typography/opentype/spec/ttch01
// https://docs.microsoft.com/en-us/typography/opentype/spec/otff
// https://fontdrop.info/

namespace Ttf {

struct Head : public BChunk {
    static constexpr Str SIG = "head";

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

struct Maxp : public BChunk {
    static constexpr Str SIG = "maxp";

    int numGlyphs() const {
        auto s = begin();
        s.skip(4);
        return s.nextBeUint16();
    }
};

struct Cmap : public BChunk {
    static constexpr Str SIG = "cmap";

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

            logWarn("ttf: Glyph not found for rune {x}", r);
            return 0;
        }

        size_t _glyphForType12(Rune r) const {
            auto s = begin().skip(12);
            uint32_t nGroups = s.nextBeUint32();

            for (uint32_t i = 0; i < nGroups; i++) {
                uint32_t startCode = s.nextBeUint32();
                uint32_t endCode = s.nextBeUint32();
                uint32_t glyphOffset = s.nextBeUint32();

                if (r < startCode)
                    break;

                if (r > endCode)
                    continue;

                if (r >= startCode and r <= endCode) {
                    return (r - startCode) + glyphOffset;
                }
            }

            logWarn("ttf: glyph not found for rune {x}", r);
            return 0;
        }

        size_t glyphIdFor(Rune r) const {
            if (type == 4) {
                return _glyphIdForType4(r);
            } else if (type == 12) {
                return _glyphForType12(r);
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

struct Loca : public BChunk {
    static constexpr Str SIG = "loca";

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

struct Glyf : public BChunk {
    static constexpr Str SIG = "glyf";

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
        s.skip(glyfOffset);
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

    void contourSimple(Gfx::Context &g, Metrics m, BScan &s) const {
        auto endPtsOfContours = s;
        auto nPoints = s.peek(2 * (m.numContours - 1)).nextBeUint16() + 1u;
        uint16_t instructionLength = s.skip(m.numContours * 2).nextBeUint16();

        auto flagsScan = s.skip(instructionLength);

        size_t nXCoords = 0;
        uint8_t flags = 0;
        uint8_t flagsRepeat = 0;

        for (size_t i = 0; i < nPoints; i++) {
            if (not flagsRepeat) {
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
        Math::Vec2f curr{};
        flags = 0;
        flagsRepeat = 0;
        for (int c = 0; c < m.numContours; c++) {
            size_t end = endPtsOfContours.nextBeUint16();

            Math::Vec2f cp{};
            Math::Vec2f startP{};
            bool wasCp = false;

            for (size_t i = start; i <= end; i++) {
                if (not flagsRepeat) {
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

                curr = curr + Math::Vec2f{(double)x, (double)-y};

                if (i == start) {
                    g.moveTo(curr);
                    startP = curr;
                } else {
                    if (flags & ON_CURVE_POINT) {
                        if (wasCp) {
                            g.quadTo(cp, curr);
                        } else {
                            g.lineTo(curr);
                        }
                        wasCp = false;
                    } else {
                        if (wasCp) {
                            auto p1 = (cp + curr) / 2;
                            g.quadTo(cp, p1);
                            cp = curr;
                            wasCp = true;
                        } else {
                            cp = curr;
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
        logTodo();
    }

    void contour(Gfx::Context &g, size_t glyfOffset) const {
        auto s = begin();
        auto m = metrics(s, glyfOffset);

        if (m.numContours > 0) {
            contourSimple(g, m, s);
        } else if (m.numContours < 0) {
            contourComposite(g, m, s);
        }
    }
};

struct Hhea : public BChunk {
    static constexpr Str SIG = "hhea";

    int ascender() const {
        auto s = begin();
        s.skip(4);
        return s.nextBeInt16();
    }

    int descender() const {
        auto s = begin();
        s.skip(6);
        return -s.nextBeInt16();
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

struct Hmtx : public BChunk {
    static constexpr Str SIG = "hmtx";
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

    static Res<Cmap::Table> chooseCmap(Font &font) {
        Opt<Cmap::Table> bestCmap = NONE;
        int bestScore = 0;

        struct KnowCmap {
            int platformId;
            int encodingId;
            int type;
            int score;
        };

        Array<KnowCmap, 4> knowCmaps = {
            KnowCmap{0, 3, 4, 150},
            KnowCmap{3, 1, 4, 100},
            KnowCmap{0, 4, 12, 1050},
            KnowCmap{3, 10, 12, 1000},
        };

        for (auto table : font._cmap.iterTables()) {
            for (auto &knowCmap : knowCmaps) {
                if (knowCmap.platformId == table.platformId and knowCmap.encodingId == table.encodingId and knowCmap.type == table.type) {
                    if (knowCmap.score > bestScore) {
                        bestCmap = table;
                        bestScore = knowCmap.score;
                    }
                }
            }
        }

        if (not bestCmap) {
            logError("ttf: no suitable cmap table found");
            return Error::other("no cmap table");
        }

        return Ok(*bestCmap);
    }

    static Res<Font> load(Bytes slice) {
        Font font{slice};

        if (font.version() != 0x00010000 and
            font.version() != 0x4F54544F) {
            logError("ttf: version {x} is not supported", (uint64_t)font.version());
            return Error::other("invalid version");
        }

        font._head = try$(font.requireTable<Head>());
        font._cmap = try$(font.requireTable<Cmap>());
        font._cmapTable = try$(chooseCmap(font));
        font._glyf = try$(font.requireTable<Glyf>());
        font._loca = try$(font.requireTable<Loca>());
        font._hhea = try$(font.requireTable<Hhea>());
        font._hmtx = try$(font.requireTable<Hmtx>());

        return Ok(font);
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
            if (Op::eq(table.tag, T::SIG)) {
                return T{sub(_slice, table.offset, table.offset + table.length)};
            }
        }

        logWarn("ttf: '{}' table not found", T::SIG);
        return T{};
    }

    template <typename T>
    Res<T> requireTable() {
        for (auto table : iterTables()) {
            if (Op::eq(table.tag, T::SIG)) {
                return Ok(T{sub(_slice, table.offset, table.offset + table.length)});
            }
        }

        logError("ttf: '{}' table not found", T::SIG);
        return Error::other("table not found");
    }

    GlyphMetrics glyphMetrics(Rune rune) const {
        auto glyphId = _cmapTable.glyphIdFor(rune);

        auto glyfOffset = _loca.glyfOffset(glyphId, _head);
        auto glyf = _glyf.metrics(glyfOffset);
        auto hmtx = _hmtx.metrics(glyphId, _hhea);

        return {
            (double)glyf.xMin,
            (double)-glyf.yMax,
            (double)glyf.xMax - glyf.xMin,
            (double)glyf.yMax - glyf.yMin,
            (double)hmtx.lsb,
            (double)hmtx.advanceWidth,
        };
    }

    void glyphContour(Gfx::Context &g, Rune rune) const {
        auto glyphId = _cmapTable.glyphIdFor(rune);
        auto glyfOffset = _loca.glyfOffset(glyphId, _head);

        if (glyfOffset == _loca.glyfOffset(glyphId + 1, _head))
            return;

        _glyf.contour(g, glyfOffset);
    }

    Metrics metrics() const {
        return {
            (double)_hhea.ascender(),
            (double)_hhea.descender(),
            (double)_hhea.lineGap(),
            (double)_hhea.advanceWidthMax(),
        };
    }

    double unitPerEm() const {
        return _head.unitPerEm();
    }
};

} // namespace Ttf
