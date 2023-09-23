#include <karm-main/main.h>
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

Res<> entryPoint(Ctx &ctx) {
    auto &args = useArgs(ctx);

    if (args.len() == 0) {
        return Error::invalidInput("Usage: dtb-dump <dtb-file>");
    }

    auto url = try$(Url::parseUrlOrPath(args[0]));
    auto file = try$(Sys::File::open(url));
    auto map = try$(Sys::mmap().map(file));
    auto ttf = try$(Ttf::Font::load(map.bytes()));

    Sys::println("ttf is valid");
    dumpGpos(ttf._gpos);

    return Ok();
}
