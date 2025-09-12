#pragma once

#include <karm-logger/logger.h>
#include <karm-math/vec.h>

#include "otlayout.h"

namespace Karm::Font::Ttf {

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

// https://learn.microsoft.com/en-us/typography/opentype/spec/gpos
struct Gpos : Io::BChunk {
    static constexpr Str SIG = "GPOS";

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

    Res<Pair<ValueRecord>> adjustments(usize prev, usize curr) const {
        // 1. Locate the current script in the GPOS ScriptList table.

        // FIXME: We assume that the script is always "latn".
        auto scriptTable = try$(scriptList().lookup("latn"));

        // 2. If the language system is known, search the script for the correct
        //    LangSys table; otherwise, use the script’s default LangSys table.

        // FIXME: We assume that the language system is always "dflt".
        auto langSys = scriptTable.defaultLangSys();

        // 3. The LangSys table provides index numbers into the GPOS FeatureList
        //    table to access a required feature and a number of additional features.
        Opt<FeatureTable> kernFeatureTable;
        for (auto featureIndex : langSys.iterFeatures()) {
            auto featureTable = featureList().at(featureIndex);

            // 4. Inspect the featureTag of each feature, and select the feature
            //    tables to apply to an input glyph string.
            if (featureTable.tag == "kern") {
                kernFeatureTable = featureTable;
                break;
            }
        }

        if (not kernFeatureTable)
            return Ok(Pair<ValueRecord>{});

        // 5. If a Feature Variation table is present, evaluate conditions in
        //    the Feature Variation table to determine if any of the initially-
        //    selected feature tables should be substituted by an alternate
        //    feature table.

        // FIXME: We don't support feature variations.

        // 6. Each feature provides an array of index numbers into the GPOS
        //    LookupList table. Assemble all lookups from the set of chosen
        //    feature tables, and apply the lookups in the order given in the
        //    LookupList table.
        for (auto lookupIndex : kernFeatureTable->iterLookups()) {
            auto lookupTable = lookupList().at(lookupIndex);

            // FIXME: We only support pair adjustment lookups.
            if (lookupTable.lookupType() != (u16)GposLookupType::PAIR_ADJUSTMENT)
                continue;

            for (auto lookupSubtable : lookupTable.iter()) {
                if (auto glyphPair = lookupSubtable.is<GlyphPairAdjustment>()) {
                    auto pair = glyphPair->adjustments(prev, curr);
                    if (pair)
                        return Ok(*pair);
                } else if (auto classPair = lookupSubtable.is<ClassPairAdjustment>()) {
                    auto pair = classPair->adjustments(prev, curr);
                    if (pair)
                        return Ok(*pair);
                } else {
                    logWarn("ttf: unsupported GPOS lookup subtable");
                }
            }
        }

        return Ok(Pair<ValueRecord>{});
    }
};

} // namespace Karm::Font::Ttf
