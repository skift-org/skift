#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <vaev-xml/parser.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() != 2) {
        Sys::errln("usage: vaev-html.cli <verb> <url>\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];
    auto url = co_try$(Mime::parseUrlOrPath(args[1]));

    if (verb == "dump-dom") {
        auto file = co_try$(Sys::File::open(url));
        auto buf = co_try$(Io::readAllUtf8(file));

        auto start = Sys::now();

        Vaev::Xml::Parser parser{};
        Io::SScan s{buf};
        auto res = parser.parse(s, Vaev::HTML);

        auto elapsed = Sys::now() - start;
        logInfo("parsed in {}ms", elapsed.toUSecs() / 1000.0);

        // Sys::println("{}", res);

        co_return Ok();
    } else {
        Sys::errln("unknown verb: {} (expected: dump-dom, dump-tokens)\n", verb);
        co_return Error::invalidInput();
    }
}
