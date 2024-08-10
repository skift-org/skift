#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <vaev-js>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() != 2) {
        Sys::errln("usage: vaev-js.cli <verb> <url>\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];
    auto url = co_try$(Mime::parseUrlOrPath(args[1]));
    if (verb == "dump-tokens") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));
        Io::SScan s{buf};
        Vaev::Js::Lexer lex{s};
        while (not lex.ended())
            Sys::println("{}", lex.next());
        co_return Ok();
    } else {
        Sys::errln("unknown verb: {} (expected: dump-tokens)\n", verb);
        co_return Error::invalidInput();
    }
}
