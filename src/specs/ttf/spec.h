#pragma once

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

    u16 unitPerEm() const {
        auto s = begin();
        s.skip(18);
        return s.nextU16be();
    }

    u16 locaFormat() const {
        auto s = begin();
        s.skip(50);
        return s.nextU16be();
    }
};

struct Maxp : public BChunk {
    static constexpr Str SIG = "maxp";

    u16 numGlyphs() const {
        auto s = begin();
        s.skip(4);
        return s.nextU16be();
    }
};

struct Cmap : public BChunk {
    static constexpr Str SIG = "cmap";

    struct Table {
        u16 platformId;
        u16 encodingId;
        u16 type;

        Bytes slice;

        BScan begin() const {
            return slice;
        }

        usize _glyphIdForType4(Rune r) const {
            u16 segCountX2 = begin().skip(6).nextU16be();
            u16 segCount = segCountX2 / 2;

            for (usize i = 0; i < segCount; i++) {
                auto s = begin().skip(14);

                u16 endCode = s.skip(i * 2).peekU16be();

                if (r >= endCode)
                    continue;

                // + 2 for reserved padding
                u16 startCode = s.skip(segCountX2 + 2).peekU16be();

                if (r < startCode)
                    break;

                u16 idDelta = s.skip(segCountX2).peekI16be();
                u16 idRangeOffset = s.skip(segCountX2).peekU16be();

                if (idRangeOffset == 0) {
                    return (r + idDelta) & 0xFFFF;
                } else {
                    auto offset = idRangeOffset + (r - startCode) * 2;
                    return s.skip(offset).nextU16be();
                }
            }

            logWarn("ttf: Glyph not found for rune {x}", r);
            return 0;
        }

        usize _glyphForType12(Rune r) const {
            auto s = begin().skip(12);
            u32 nGroups = s.nextU32be();

            for (u32 i = 0; i < nGroups; i++) {
                u32 startCode = s.nextU32be();
                u32 endCode = s.nextU32be();
                u32 glyphOffset = s.nextU32be();

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

        usize glyphIdFor(Rune r) const {
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
        usize numTables = s.nextU16be();

        return Iter{[this, s, i = 0uz, numTables]() mutable -> Opt<Table> {
            if (i == numTables) {
                return NONE;
            }

            u16 platformId = s.nextU16be();
            u16 encodingId = s.nextU16be();
            u32 offset = s.nextU32be();
            auto slice = sub(_slice, offset, _slice.len());
            u16 type = BScan{slice}.nextU16be();
            i++;

            return Table{platformId, encodingId, type, slice};
        }};
    }
};

struct Loca : public BChunk {
    static constexpr Str SIG = "loca";

    usize glyfOffset(isize glyphId, Head const &head) const {
        auto s = begin();
        if (head.locaFormat() == 0) {
            s.skip(glyphId * 2);
            return s.nextU16be();
        } else {
            s.skip(glyphId * 4);
            return s.nextU32be();
        }
    }
};

struct Glyf : public BChunk {
    static constexpr Str SIG = "glyf";

    static constexpr u8 ON_CURVE_POINT = 0x01;
    static constexpr u8 X_SHORT_VECTOR = 0x02;
    static constexpr u8 Y_SHORT_VECTOR = 0x04;
    static constexpr u8 REPEAT = 0x08;
    static constexpr u8 SAME_OR_POSITIVE_X = 0x10;
    static constexpr u8 SAME_OR_POSITIVE_Y = 0x20;
    static constexpr u8 OVERLAY_SIMPLE = 0x40;

    struct Metrics {
        i16 numContours;
        i16 xMin;
        i16 yMin;
        i16 xMax;
        i16 yMax;
    };

    ALWAYS_INLINE Metrics metrics(BScan &s, usize glyfOffset) const {
        s.skip(glyfOffset);
        auto numContours = s.nextI16be();
        if (numContours == 0) {
            return {};
        }
        auto xMin = s.nextI16be();
        auto yMin = s.nextI16be();
        auto xMax = s.nextI16be();
        auto yMax = s.nextI16be();
        return {numContours, xMin, yMin, xMax, yMax};
    }

    ALWAYS_INLINE Metrics metrics(usize glyfOffset) const {
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
        u8 flags;
        i16 x;
        i16 y;
    };

    void contourSimple(Gfx::Context &g, Metrics m, BScan &s) const {
        auto endPtsOfContours = s;
        auto nPoints = s.peek(2 * (m.numContours - 1)).nextU16be() + 1u;
        u16 instructionLength = s.skip(m.numContours * 2).nextU16be();

        auto flagsScan = s.skip(instructionLength);

        usize nXCoords = 0;
        u8 flags = 0;
        u8 flagsRepeat = 0;

        for (usize i = 0; i < nPoints; i++) {
            if (not flagsRepeat) {
                flags = s.nextU8be();
                if (flags & REPEAT) {
                    flagsRepeat = s.nextU8be();
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

        usize start = 0;
        Math::Vec2f curr{};
        flags = 0;
        flagsRepeat = 0;
        for (isize c = 0; c < m.numContours; c++) {
            usize end = endPtsOfContours.nextU16be();

            Math::Vec2f cp{};
            Math::Vec2f startP{};
            bool wasCp = false;

            for (usize i = start; i <= end; i++) {
                if (not flagsRepeat) {
                    flags = flagsScan.nextU8be();
                    if (flags & REPEAT) {
                        flagsRepeat = flagsScan.nextU8be();
                    }
                } else {
                    flagsRepeat--;
                }

                isize x = (flags & X_SHORT_VECTOR)
                              ? ((flags & SAME_OR_POSITIVE_X) ? xCoordsScan.nextU8be() : -xCoordsScan.nextU8be())
                              : ((flags & SAME_OR_POSITIVE_X) ? 0 : xCoordsScan.nextI16be());

                isize y = (flags & Y_SHORT_VECTOR)
                              ? ((flags & SAME_OR_POSITIVE_Y) ? yCoordsScan.nextU8be() : -yCoordsScan.nextU8be())
                              : ((flags & SAME_OR_POSITIVE_Y) ? 0 : yCoordsScan.nextI16be());

                curr = curr + Math::Vec2f{(f64)x, (f64)-y};

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

    void contour(Gfx::Context &g, usize glyfOffset) const {
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

    isize ascender() const {
        auto s = begin();
        s.skip(4);
        return s.nextI16be();
    }

    isize descender() const {
        auto s = begin();
        s.skip(6);
        return -s.nextI16be();
    }

    isize lineGap() const {
        auto s = begin();
        s.skip(8);
        return s.nextI16be();
    }

    isize advanceWidthMax() const {
        auto s = begin();
        s.skip(10);
        return s.nextU16be();
    }

    isize numberOfHMetrics() const {
        auto s = begin();
        s.skip(34);
        return s.nextU16be();
    }
};

struct Hmtx : public BChunk {
    static constexpr Str SIG = "hmtx";
    static constexpr isize LONG_RECORD_SIZE = 4;
    static constexpr isize SHORT_RECORD_SIZE = 2;

    struct Metrics {
        isize advanceWidth;
        isize lsb;
    };

    Metrics metrics(isize glyphId, Hhea const &hhea) const {
        if (glyphId < hhea.numberOfHMetrics()) {
            auto s = begin().skip(glyphId * 4);
            return {s.nextU16be(), s.nextI16be()};
        } else {
            usize advOffset = (hhea.numberOfHMetrics() - 1) * LONG_RECORD_SIZE;
            usize lsbOffset = hhea.numberOfHMetrics() * LONG_RECORD_SIZE +
                              (glyphId - hhea.numberOfHMetrics()) * SHORT_RECORD_SIZE;

            isize adv = begin().skip(advOffset).nextU16be();
            isize lsb = begin().skip(lsbOffset).nextU16be();

            return {adv, lsb};
        }
    }
};

struct LangTable : public BChunk {
    using LookupOrderOffset = BField<u16be, 0>;
    using ReqFeatureIndex = BField<u16be, 2>;
    using FeatureCount = BField<u16be, 4>;

    Str tag;

    LangTable(Str tag, Bytes bytes)
        : BChunk{bytes}, tag(tag) {}

    auto iterFeatures() const {
        auto s = begin().skip(6);
        return range((usize)get<FeatureCount>())
            .map([s](auto) mutable {
                return s.nextU16be();
            });
    }
};

struct ScriptTable : public BChunk {
    using DefaultLangSysOffset = BField<u16be, 0>;
    using LangSysCount = BField<u16be, 2>;

    Str tag;

    ScriptTable(Str tag, Bytes bytes)
        : BChunk{bytes}, tag(tag) {}

    LangTable defaultLangSys() const {
        return {"DFLT", begin().skip(get<DefaultLangSysOffset>()).restBytes()};
    }

    usize len() const {
        return get<LangSysCount>();
    }

    LangTable at(usize i) const {
        auto s = begin().skip(4 + i * 6);
        return {s.nextStr(4), begin().skip(s.nextU16be()).restBytes()};
    }

    auto iter() const {
        return range(len())
            .map([&](auto i) mutable {
                return at(i);
            });
    }
};

struct ScriptList : public BChunk {
    using ScriptCount = BField<u16be, 0>;

    usize len() const {
        return get<ScriptCount>();
    }

    ScriptTable at(usize i) const {
        auto s = begin().skip(2 + i * 6);
        auto tag = s.nextStr(4);
        auto off = s.nextU16be();
        return ScriptTable{tag, begin().skip(off).restBytes()};
    }

    auto iter() const {
        return range(len())
            .map([&](auto i) mutable {
                return at(i);
            });
    }
};

struct FeatureTable : public BChunk {
    using FeatureParamsOffset = BField<u16be, 0>;
    using LookupCount = BField<u16be, 2>;

    Str tag;

    FeatureTable(Str tag, Bytes bytes)
        : BChunk{bytes}, tag(tag) {}

    auto iterLookups() const {
        auto s = begin().skip(4);
        return range((usize)get<LookupCount>())
            .map([s](auto) mutable {
                return s.nextU16be();
            });
    }
};

struct FeatureList : public BChunk {
    using featureCount = BField<u16be, 0>;

    usize len() const {
        return get<featureCount>();
    }

    FeatureTable at(usize i) const {
        auto s = begin().skip(2 + i * 6);
        auto tag = s.nextStr(4);
        auto off = s.nextU16be();
        return FeatureTable{tag, begin().skip(off).restBytes()};
    }

    auto iter() const {
        return range(len())
            .map([&](auto i) mutable {
                return at(i);
            });
    }
};

struct LookupTable : public BChunk {
    using LookupType = BField<u16be, 0>;
    using LookupFlag = BField<u16be, 2>;
    using SubTableCount = BField<u16be, 4>;

    enum LookupFlags : u16 {
        RIGHT_TO_LEFT = 1 << 0,
        IGNORE_BASE_GLYPHS = 1 << 1,
        IGNORE_LIGATURES = 1 << 2,
        IGNORE_MARKS = 1 << 3,
        USE_MARK_FILTERING_SET = 1 << 4,
        MARK_ATTACHMENT_TYPE = 1 << 5,
    };

    u16 lookupType() const { return get<LookupType>(); }

    u16 lookupFlag() const { return get<LookupFlag>(); }

    u16 markFilteringSet() const {
        return lookupFlag() & USE_MARK_FILTERING_SET
                   ? begin().skip(6 + get<SubTableCount>() * 2).nextU16be()
                   : 0;
    }

    usize len() const { return get<SubTableCount>(); }
};

struct LookupList : public BChunk {
    using LookupCount = BField<u16be, 0>;

    usize len() const { return get<LookupCount>(); }

    LookupTable at(usize i) const {
        auto s = begin().skip(2 + i * 2);
        auto off = s.nextU16be();
        return LookupTable{begin().skip(off).restBytes()};
    }

    auto iter() const {
        return range(len())
            .map([&](auto i) mutable {
                return at(i);
            });
    }
};

enum struct GposLookupType : u16 {
    SINGLE_ADJUSTMENT = 1,
    PAIR_ADJUSTMENT = 2,
    CURSIVE_ATTACHMENT = 3,
    MARK_TO_BASE_ATTACHMENT = 4,
    MARK_TO_LIGATURE_ATTACHMENT = 5,
    MARK_TO_MARK_ATTACHMENT = 6,
    CONTEXT_POSITIONING = 7,
    CHAIN_CONTEXT_POSITIONING = 8,
    EXTENSION_POSITIONING = 9,
};

struct Gpos : public BChunk {
    static constexpr Str SIG = "GPOS";

    using ScriptListOffset = BField<u16be, 4>;
    using FeatureListOffset = BField<u16be, 6>;
    using LookupListOffset = BField<u16be, 8>;

    ScriptList scriptList() const {
        return ScriptList{begin().skip(get<ScriptListOffset>()).restBytes()};
    }

    FeatureList featureList() const {
        return FeatureList{begin().skip(get<FeatureListOffset>()).restBytes()};
    }

    LookupList lookupList() const {
        return LookupList{begin().skip(get<LookupListOffset>()).restBytes()};
    }

    // Positioning

    /*
    Res<Math::Vec2i> positione(usize prev, usize curr) const {
        (void)lhs;
        (void)rhs;
        // 1. Locate the current script in the GPOS ScriptList table.

        // 2. If the language system is known, search the script for the correct LangSys table; otherwise, use the script’s default LangSys table.

        // FIXME: hard-coded to latin
        auto scriptOff = try$(findScript("latn"));
        (void)scriptOff;

        // 3. The LangSys table provides index numbers into the GPOS FeatureList table to access a required feature and a number of additional features.

        // 4. Inspect the featureTag of each feature, and select the feature tables to apply to an input glyph string.

        // 5. If a Feature Variation table is present, evaluate conditions in the Feature Variation table to determine if any of the initially-selected feature tables should be substituted by an alternate feature table.

        // 6. Each feature provides an array of index numbers into the GPOS LookupList table. Assemble all lookups from the set of chosen feature tables, and apply the lookups in the order given in the LookupList table.

        return Ok(Math::Vec2i{0, 0});
    }
*/
};

struct Gsub : public BChunk {
    static constexpr Str SIG = "GSUB";

    using ScriptListOffset = BField<u16be, 4>;
    using FeatureListOffset = BField<u16be, 6>;
    using LookupListOffset = BField<u16be, 8>;

    ScriptList scriptList() const {
        return ScriptList{begin().skip(get<ScriptListOffset>()).restBytes()};
    }

    FeatureList featureList() const {
        return FeatureList{begin().skip(get<FeatureListOffset>()).restBytes()};
    }

    LookupList lookupList() const {
        return LookupList{begin().skip(get<LookupListOffset>()).restBytes()};
    }
};

struct GlyphMetrics {
    f64 x;
    f64 y;
    f64 width;
    f64 height;
    f64 lsb;
    f64 advance;
};

struct Metrics {
    f64 ascend;
    f64 descend;
    f64 linegap;
    f64 maxWidth;
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
    Gpos _gpos;
    Gsub _gsub;

    static Res<Cmap::Table> chooseCmap(Font &font) {
        Opt<Cmap::Table> bestCmap = NONE;
        isize bestScore = 0;

        struct KnowCmap {
            isize platformId;
            isize encodingId;
            isize type;
            isize score;
        };

        Array<KnowCmap, 4> knowCmaps = {
            KnowCmap{0, 3, 4, 150},
            KnowCmap{3, 1, 4, 100},
            KnowCmap{0, 4, 12, 1050},
            KnowCmap{3, 10, 12, 1000},
        };

        for (auto table : font._cmap.iterTables()) {
            for (auto &knowCmap : knowCmaps) {
                if (knowCmap.platformId == table.platformId and
                    knowCmap.encodingId == table.encodingId and
                    knowCmap.type == table.type and
                    knowCmap.score > bestScore) {

                    bestCmap = table;
                    bestScore = knowCmap.score;
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
            logError("ttf: version {x} is not supported", (u64)font.version());
            return Error::other("invalid version");
        }

        font._head = try$(font.requireTable<Head>());
        font._cmap = try$(font.requireTable<Cmap>());
        font._cmapTable = try$(chooseCmap(font));
        font._glyf = try$(font.requireTable<Glyf>());
        font._loca = try$(font.requireTable<Loca>());
        font._hhea = try$(font.requireTable<Hhea>());
        font._hmtx = try$(font.requireTable<Hmtx>());
        font._gpos = font.lookupTable<Gpos>();
        font._gsub = font.lookupTable<Gsub>();

        return Ok(font);
    }

    Font(Bytes slice) : _slice(slice) {}

    /* --- Parsing Api ------------------------------------------------------ */

    BScan begin() const {
        return _slice;
    }

    u32 version() {
        return begin().nextU32be();
    }

    auto iterTables() {
        auto scan = begin();
        /* auto version = */ scan.nextU32be();
        auto numTables = scan.nextU16be();
        /* auto searchRange = */ scan.nextU16be();
        /* auto entrySelector = */ scan.nextU16be();
        /* auto rangeShift = */ scan.nextU16be();

        struct Table {
            Str tag;
            u32 checkSum;
            u32 offset;
            u32 length;
        };

        return Iter{[scan, i = 0uz, numTables]() mutable -> Opt<Table> {
            if (i == numTables) {
                return NONE;
            }

            Table table{};
            table.tag = scan.nextStr(4);
            table.checkSum = scan.nextU32be();
            table.offset = scan.nextU32be();
            table.length = scan.nextU32be();

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
            (f64)glyf.xMin,
            (f64)-glyf.yMax,
            (f64)glyf.xMax - glyf.xMin,
            (f64)glyf.yMax - glyf.yMin,
            (f64)hmtx.lsb,
            (f64)hmtx.advanceWidth,
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
            (f64)_hhea.ascender(),
            (f64)_hhea.descender(),
            (f64)_hhea.lineGap(),
            (f64)_hhea.advanceWidthMax(),
        };
    }

    f64 unitPerEm() const {
        return _head.unitPerEm();
    }
};

} // namespace Ttf
