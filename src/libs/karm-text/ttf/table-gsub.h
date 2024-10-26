#pragma once

#include "otlayout.h"

namespace Ttf {

struct Gsub : public Io::BChunk {
    static constexpr Str SIG = "GSUB";

    using ScriptListOffset = Io::BField<u16be, 4>;
    using FeatureListOffset = Io::BField<u16be, 6>;
    using LookupListOffset = Io::BField<u16be, 8>;

    ScriptList scriptList() const {
        return ScriptList{begin().skip(get<ScriptListOffset>()).remBytes()};
    }

    FeatureList featureList() const {
        return FeatureList{begin().skip(get<FeatureListOffset>()).remBytes()};
    }

    LookupList lookupList() const {
        return LookupList{begin().skip(get<LookupListOffset>()).remBytes()};
    }

    // 1. Locate the current script in the GSUB ScriptList table.

    // 2. If the language system is known, search the script for the correct
    //    LangSys table; otherwise, use the script’s default LangSys table.

    // 3. The LangSys table provides index numbers into the GSUB FeatureList
    //    table to access a required feature and a number of additional features.

    // 4. Inspect the featureTag of each Feature table, and select the feature
    //    tables to apply to an input glyph string.

    // 5. If a Feature Variations table is present, evaluate conditions in the
    //    Feature Variation table to determine if any of the initially-selected
    //    feature tables should be substituted by an alternate feature table.

    // 6. Each Feature table provides an array of index numbers into the GSUB
    //    LookupList table.Assemble all lookups from the set of chosen features,
    //    and apply the lookups in the order given in the LookupList table.
};

} // namespace Ttf
