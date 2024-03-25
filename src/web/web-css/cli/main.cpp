#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-css/lexer.h>
#include <web-css/parser.h>

Res<> entryPoint(Sys::Ctx &) {
    auto file = try$(Sys::File::open("bundle://web-css-cli/exemple_1.css"_url));

    auto buf = try$(Io::readAllUtf8(file));

    Sys::println("Orginal Source:");
    Sys::println("{}", buf);

    Sys::println("Parsing:");
    Web::Css::Parser parser{};
    parser.write(buf);

    Sys::println("Result:");
    Io::Emit emit{Sys::out()};
    try$(emit.flush());
    emit("\n");

    return Ok();
}
