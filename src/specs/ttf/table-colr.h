#pragma once

// https://learn.microsoft.com/en-us/typography/opentype/spec/colr

#include <karm-io/bscan.h>

#include "table-cpal.h"

namespace Ttf {

struct BaseGlyphList {
    using NumBaseGlyphPaintRecords = Io::BField<u16be, 0>;

    struct Record {
        u16 glyphId;
        u32 paintOffset;

        static Record read(Io::BScan &scan) {
            return {
                .glyphId = scan.nextU16be(),
                .paintOffset = scan.nextU32be(),
            };
        }
    };
};

struct Colr : public Io::BChunk {
    static constexpr Str SIG = "COLR";

    using Version = Io::BField<u16be, 0>;

    // Version 0
    using NumBaseGlyphRecords = Io::BField<u16be, 2>;
    using BaseGlyphRecordsOffset = Io::BField<u32be, 4>;
    using LayerRecordsOffset = Io::BField<u32be, 8>;
    using NumLayerRecords = Io::BField<u16be, 12>;

    // Version 1
    using BaseGlyphListOffset = Io::BField<u32be, 14>;
    using layerListOffset = Io::BField<u32be, 18>;
    using ClipListOffset = Io::BField<u32be, 22>;
    using VarIndexMapOffset = Io::BField<u32be, 26>;
    using ItemVariationStoreOffset = Io::BField<u32be, 30>;

    bool isVersion0Compatible() const {
        return get<Version>() == 0 and
               get<NumBaseGlyphRecords>() > 0 and
               get<NumLayerRecords>() > 0;
    }
};

} // namespace Ttf
