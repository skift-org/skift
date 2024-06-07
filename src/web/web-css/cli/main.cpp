#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-css/mod.h>
#include <web-css/parser.h>

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() != 2) {
        Sys::errln("usage: web-css.cli <verb> <url>\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];
    auto url = co_try$(Mime::parseUrlOrPath(args[1]));

    if (verb == "dump-stylesheet") {
        logInfo("fetching & parsing {}\n", url);
        co_try$(Web::Css::fetchStylesheet(url));
        logInfo("Stylesheet parsed\n");
        Sys::println("{}", Web::Css::fetchStylesheet(url).unwrap());
        co_return Ok();
    } else if (verb == "dump-sst") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));
        Io::SScan s{buf};
        Web::Css::Lexer lex{s};
        Web::Css::Sst sst = Web::Css::consumeRuleList(lex);
        Sys::println("{}", sst);
        co_return Ok();
    } else if (verb == "dump-token") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));
        Io::SScan s{buf};
        Web::Css::Lexer lex{s};
        while (not lex.ended())
            Sys::println("{}", lex.next());
        co_return Ok();
    } else {
        Sys::errln("unknown verb: {} (expected: dump-stylesheet, dump-sst, dump-token)\n", verb);
        co_return Error::invalidInput();
    }
}
