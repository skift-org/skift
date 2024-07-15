#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <vaev-driver/fetcher.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() != 2) {
        Sys::errln("usage: vaev-css.cli <verb> <url>\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];
    auto url = co_try$(Mime::parseUrlOrPath(args[1]));

    if (verb == "dump-stylesheet") {
        auto start = Sys::now();
        auto stylesheet = co_try$(Vaev::Driver::fetchStylesheet(url));
        auto elapsed = Sys::now() - start;
        logInfo("fetched in {}ms", elapsed.toUSecs() / 1000.0);

        Sys::println("{#}", stylesheet);
        co_return Ok();
    } else if (verb == "dump-sst") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));

        auto start = Sys::now();

        Io::SScan s{buf};
        Vaev::Css::Lexer lex{s};
        Vaev::Css::Sst sst = Vaev::Css::consumeRuleList(lex, true);

        auto elapsed = Sys::now() - start;
        logInfo("parsed in {}ms", elapsed.toUSecs() / 1000.0);

        Sys::println("{}", sst);
        co_return Ok();
    } else if (verb == "dump-tokens") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));
        Io::SScan s{buf};
        Vaev::Css::Lexer lex{s};
        while (not lex.ended())
            Sys::println("{}", lex.next());
        co_return Ok();
    } else {
        Sys::errln("unknown verb: {} (expected: dump-stylesheet, dump-sst, dump-tokens)\n", verb);
        co_return Error::invalidInput();
    }
}
