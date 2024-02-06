#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>
#include <ttf/spec.h>

void dumpGpos(Ttf::Gpos const &gpos) {
    Sys::println("GPOS table:");
    if (not gpos.present()) {
        Sys::println("  not present");
        return;
    }

    Sys::println("  ScriptList (len:{})", gpos.scriptList().len());
    for (auto script : gpos.scriptList().iter()) {
        Sys::println("    '{}'", script.tag);

        Sys::println("      '{}'", script.defaultLangSys().tag);
        for (auto lang : script.iter()) {
            Sys::println("      '{}'", lang.tag);

            Sys::println("        Features:");
            for (auto feat : lang.iterFeatures()) {
                Sys::println("          '{}'", feat);
            }
        }
    }

    Sys::println("  FeatureList (len:{})", gpos.featureList().len());
    for (auto feat : gpos.featureList().iter()) {
        Sys::println("    '{}'", feat.tag);
        for (auto lookup : feat.iterLookups()) {
            Sys::println("      LookupIndex: {}", lookup);
        }
    }

    Sys::println("  LookupList (len:{})", gpos.lookupList().len());
}

void dumpCpal(Ttf::Cpal const &cpal) {
    Sys::println("CPAL table:");
    if (not cpal.present()) {
        Sys::println("  not present");
        return;
    }

    Sys::println("  version: {}", cpal.get<Ttf::Cpal::Version>());
}

void dumpColr(Ttf::Colr const &colr) {
    Sys::println("COLR table:");
    if (not colr.present()) {
        Sys::println("  not present");
        return;
    }
    Sys::println("  version: {}", colr.get<Ttf::Colr::Version>());

    if (colr.isVersion0Compatible()) {
        Sys::println("  Compatible With Version 0", colr.get<Ttf::Colr::Version>());
        Sys::println("  NumBaseGlyphRecords: {}", colr.get<Ttf::Colr::NumBaseGlyphRecords>());
        Sys::println("  BaseGlyphRecordsOffset: {}", colr.get<Ttf::Colr::BaseGlyphRecordsOffset>());
        Sys::println("  LayerRecordsOffset: {}", colr.get<Ttf::Colr::LayerRecordsOffset>());
        Sys::println("  NumLayerRecords: {}", colr.get<Ttf::Colr::NumLayerRecords>());
    } else {
        Sys::println("  Not Compatible With Version 0", colr.get<Ttf::Colr::Version>());
    }
}

Res<> entryPoint(Sys::Ctx &ctx) {
    auto &args = useArgs(ctx);

    if (args.len() == 0)
        return Error::invalidInput("Usage: ttf-dump <dtb-file>");

    auto url = try$(Url::parseUrlOrPath(args[0]));
    auto file = try$(Sys::File::open(url));
    auto map = try$(Sys::mmap().map(file));
    auto ttf = try$(Ttf::Font::load(map.bytes()));

    Sys::println("ttf is valid");
    Sys::println("");

    dumpGpos(ttf._gpos);
    Sys::println("");

    dumpCpal(ttf._cpal);
    Sys::println("");

    dumpColr(ttf._colr);
    Sys::println("");

    return Ok();
}
