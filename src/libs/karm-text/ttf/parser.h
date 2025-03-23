#pragma once

#include <karm-logger/logger.h>

#include "table-cmap.h"
#include "table-glyf.h"
#include "table-gpos.h"
#include "table-gsub.h"
#include "table-head.h"
#include "table-hhea.h"
#include "table-hmtx.h"
#include "table-loca.h"
#include "table-maxp.h"
#include "table-name.h"
#include "table-os2.h"
#include "table-post.h"

// https://tchayen.github.io/posts/ttf-file-parsing
// http://stevehanov.ca/blog/?id=143
// https://www.microsoft.com/typography/otspec/otff.htm
// https://docs.microsoft.com/en-us/typography/opentype/spec/glyf
// https://docs.microsoft.com/en-us/typography/opentype/spec/ttch01
// https://docs.microsoft.com/en-us/typography/opentype/spec/otff
// https://fontdrop.info/

namespace Ttf {

static bool DEBUG_PARSER = false;

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

struct Parser {
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
    Name _name;
    Post _post;
    Os2 _os2;

    static Res<Cmap::Table> chooseCmap(Parser& font) {
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
            for (auto& knowCmap : knowCmaps) {
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
            return Error::other("no suitable cmap table found");
        }

        return Ok(*bestCmap);
    }

    static bool sniff(Bytes slice) {
        return slice.len() >= 4 and slice[0] == 0x00 and slice[1] == 0x01 and
               slice[2] == 0x00 and slice[3] == 0x00;
    }

    static Res<Parser> init(Bytes slice) {
        Parser font{slice};

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
        font._name = font.lookupTable<Name>();
        font._post = font.lookupTable<Post>();
        if (not font._post.present())
            logWarn("ttf: 'post' table not found");

        font._os2 = font.lookupTable<Os2>();
        if (not font._os2.present())
            logWarn("ttf: 'OS/2' table not found");

        return Ok(font);
    }

    Parser(Bytes slice) : _slice(slice) {}

    // MARK: Parsing Api -------------------------------------------------------

    Io::BScan begin() const {
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

        return Iter{[scan, i = 0uz, numTables] mutable -> Opt<Table> {
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
            if (table.tag == T::SIG) {
                return T{sub(_slice, table.offset, table.offset + table.length)};
            }
        }

        logDebugIf(DEBUG_PARSER, "ttf: '{}' table not found", T::SIG);
        return T{};
    }

    template <typename T>
    Res<T> requireTable() {
        for (auto table : iterTables()) {
            if (table.tag == T::SIG) {
                return Ok(T{sub(_slice, table.offset, table.offset + table.length)});
            }
        }

        logError("ttf: '{}' table not found", T::SIG);
        return Error::other("table not found");
    }

    Text::Glyph glyph(Rune rune) const {
        return _cmapTable.glyphIdFor(rune);
    }

    GlyphMetrics glyphMetrics(Text::Glyph glyph) const {
        auto glyfOffset = _loca.glyfOffset(glyph.index, _head);
        auto glyf = _glyf.metrics(glyfOffset);
        auto hmtx = _hmtx.metrics(glyph.index, _hhea);

        return {
            (f64)glyf.xMin,
            (f64)-glyf.yMax,
            (f64)glyf.xMax - glyf.xMin,
            (f64)glyf.yMax - glyf.yMin,
            (f64)hmtx.lsb,
            (f64)hmtx.advanceWidth,
        };
    }

    f64 glyphKern(Text::Glyph prev, Text::Glyph curr) const {
        if (not _gpos.present())
            return 0;

        auto positioning = _gpos.adjustments(prev.index, curr.index);

        if (not positioning)
            return 0;

        return positioning.unwrap().v0.xAdvance;
    }

    void glyphContour(Gfx::Canvas& g, Text::Glyph glyph) const {
        auto glyfOffset = _loca.glyfOffset(glyph.index, _head);

        if (glyfOffset == _loca.glyfOffset(glyph.index + 1, _head))
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
