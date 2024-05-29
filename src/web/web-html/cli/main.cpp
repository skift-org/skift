#include <karm-io/funcs.h>
#include <karm-sys/entry-async.h>
#include <karm-sys/file.h>
#include <web-html/lexer.h>
#include <web-html/parser.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    auto file = co_try$(Sys::File::open("bundle://web-html-cli/exemple.html"_url));

    auto buf = co_try$(Io::readAllUtf8(file));

    Sys::println("Orginal Source:");
    Sys::println("{}", buf);

    Sys::println("Parsing:");
    auto doc = makeStrong<Web::Dom::Document>();
    Web::Html::Parser parser{doc};
    parser.write(buf);

    Sys::println("Result:");
    Io::Emit emit{Sys::out()};
    doc->dump(emit);
    co_try$(emit.flush());
    emit("\n");

    co_return Ok();
}
