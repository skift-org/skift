#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-css/lexer.h>
#include <web-css/parser.h>

Res<> entryPoint(Sys::Ctx &) {
    Sys::println("Parsing:");
    try$(Web::Css::parseStylesheet("bundle://web-css-cli/exemple_1.css"));

    Sys::println("Result:");
    Io::Emit emit{Sys::out()};
    try$(emit.flush());
    emit("\n");

    return Ok();
}
