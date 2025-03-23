#pragma once

#include <karm-io/bscan.h>
#include <karm-logger/logger.h>

#include "../base.h"

namespace Ttf {

struct Cmap : public Io::BChunk {
    static constexpr Str SIG = "cmap";

    struct Table {
        u16 platformId;
        u16 encodingId;
        u16 type;

        Bytes slice;

        Io::BScan begin() const {
            return slice;
        }

        Map<u16, u16> _extractMappingForType12() {
            Map<u16, u16> codeMappings;

            auto s = begin().skip(12);
            u32 nGroups = s.nextU32be();

            for (usize i = 0; i < nGroups; i++) {
                u32 startCode = s.nextU32be();
                u32 endCode = s.nextU32be();

                u32 glyphOffset = s.nextU32be();

                for (usize r = startCode; r <= endCode; ++r) {
                    codeMappings.put(r, (r - startCode) + glyphOffset);
                }
            }
            return codeMappings;
        }

        Text::Glyph _glyphIdForType4(Rune r) const {
            u16 segCountX2 = begin().skip(6).nextU16be();
            u16 segCount = segCountX2 / 2;

            for (usize i = 0; i < segCount; i++) {
                auto s = begin().skip(14);

                u16 endCode = s.skip(i * 2).peekU16be();

                if (r > endCode)
                    continue;

                // + 2 for reserved padding
                u16 startCode = s.skip(segCountX2 + 2).peekU16be();

                if (r < startCode)
                    break;

                u16 idDelta = s.skip(segCountX2).peekI16be();
                u16 idRangeOffset = s.skip(segCountX2).peekU16be();

                if (idRangeOffset == 0) {
                    return Text::Glyph((r + idDelta) & 0xFFFF);
                }

                auto offset = idRangeOffset + (r - startCode) * 2;
                return Text::Glyph(s.skip(offset).nextU16be());
            }

            logWarn("ttf: glyph not found for rune {x}", r);
            return Text::Glyph(0);
        }

        Map<u16, u16> _extractMappingForType4() {
            Map<u16, u16> codeMappings;

            u16 segCountX2 = begin().skip(6).nextU16be();
            u16 segCount = segCountX2 / 2;

            for (usize i = 0; i < segCount; i++) {
                auto s = begin().skip(14);

                u16 endCode = s.skip(i * 2).peekU16be();

                // + 2 for reserved padding
                u16 startCode = s.skip(segCountX2 + 2).peekU16be();

                u16 idDelta = s.skip(segCountX2).peekI16be();
                u16 idRangeOffset = s.skip(segCountX2).peekU16be();

                if (idRangeOffset == 0) {
                    for (usize code = startCode; code <= endCode; code++) {
                        codeMappings.put(code, (u16)((code + idDelta) & 0xFFFF));
                    }
                } else {
                    for (usize code = startCode; code <= endCode; code++) {
                        auto offset = idRangeOffset + (code - startCode) * 2;
                        codeMappings.put(code, s.skip(offset).nextU16be());
                    }
                }
            }

            return codeMappings;
        }

        Text::Glyph _glyphForType12(Rune r) const {
            auto s = begin().skip(12);
            u32 nGroups = s.nextU32be();

            for (usize i = 0; i < nGroups; i++) {
                u32 startCode = s.nextU32be();
                u32 endCode = s.nextU32be();
                u32 glyphOffset = s.nextU32be();

                if (r < startCode)
                    break;

                if (r > endCode)
                    continue;

                if (r >= startCode and r <= endCode) {
                    return Text::Glyph((r - startCode) + glyphOffset);
                }
            }

            return Text::Glyph(0);
        }

        Map<u16, u16> extractMapping() {
            if (type == 4) {
                return _extractMappingForType4();
            } else if (type == 12) {
                return _extractMappingForType12();
            } else {
                return {};
            }
        }

        Text::Glyph glyphIdFor(Rune r) const {
            if (type == 4) {
                return _glyphIdForType4(r);
            } else if (type == 12) {
                return _glyphForType12(r);
            } else {
                return Text::Glyph(0);
            }
        }
    };

    auto iterTables() {
        auto s = begin();
        s.skip(2);
        usize numTables = s.nextU16be();

        return Iter{[this, s, i = 0uz, numTables] mutable -> Opt<Table> {
            if (i == numTables) {
                return NONE;
            }
            i++;

            Table t;
            t.platformId = s.nextU16be();
            t.encodingId = s.nextU16be();
            u32 offset = s.nextU32be();
            t.slice = sub(_slice, offset, _slice.len());
            t.type = Io::BScan{t.slice}.nextU16be();

            return t;
        }};
    }
};

} // namespace Ttf
