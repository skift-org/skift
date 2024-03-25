#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-html/lexer.h>
#include <web-html/parser.h>

Res<> entryPoint(Sys::Ctx &) {
    auto file = try$(Sys::File::open("bundle://web-html-cli/exemple.html"_url));

    auto buf = try$(Io::readAllUtf8(file));

    Sys::println("Orginal Source:");
    Sys::println("{}", buf);

    Sys::println("Parsing:");
    auto doc = makeStrong<Web::Dom::Document>();
    Web::Html::Parser parser{doc};
    parser.write(buf);

    Sys::println("Result:");
    Io::Emit emit{Sys::out()};
    doc->dump(emit);
    try$(emit.flush());
    emit("\n");

    return Ok();
}
