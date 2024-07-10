#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>
#include <karm-text/book.h>
#include <karm-text/loader.h>
#include <karm-text/ttf.h>

static void _dumpGpos(Ttf::Gpos const &gpos) {
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

static void _dumpName(Ttf::Name const &name) {
    Sys::println("Name table:");
    if (not name.present()) {
        Sys::println("  not present");
        return;
    }

    Sys::println("  Records");
    for (auto record : name.iterRecords()) {
        if (not record.isUnicode())
            continue;
        Sys::println("    {}:{}:{} {}={#}", record.platformId, record.encodingId, record.languageId, record.nameId, name.string(record));
    }
}

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto &args = useArgs(ctx);

    if (args.len() < 1)
        co_return Error::invalidInput("Usage: karm-text.cli <verb> <args...>");

    auto verb = args[0];

    if (verb == "dump-ttf") {
        if (args.len() != 2)
            co_return Error::invalidInput("Usage: karm-text.cli dump-ttf <url>");

        auto url = co_try$(Mime::parseUrlOrPath(args[1]));
        auto file = co_try$(Sys::File::open(url));
        auto map = co_try$(Sys::mmap().map(file));
        auto ttf = co_try$(Ttf::Parser::init(map.bytes()));

        Sys::println("ttf is valid");
        _dumpGpos(ttf._gpos);
        _dumpName(ttf._name);

        co_return Ok();
    } else if (verb == "dump-db") {
        Text::FontBook book;
        co_try$(book.loadAll());
        co_return Ok();
    } else if (verb == "dump-attr") {
        if (args.len() != 2)
            co_return Error::invalidInput("Usage: karm-text.cli dump-attr <url>");

        auto url = co_try$(Mime::parseUrlOrPath(args[1]));
        auto font = co_try$(Text::loadFontface(url));

        Sys::println("{}", font->attrs());
        co_return Ok();
    } else {
        Sys::errln("unknown verb: {} (expected: dump-ttf, dump-db, dump-attr)", verb);
        co_return Error::invalidInput();
    }
}
