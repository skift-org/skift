#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry-async.h>
#include <karm-sys/file.h>
#include <web-css/lexer.h>
#include <web-css/parser.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    Sys::println("Parsing:");
    co_try$(Web::Css::parseStylesheet("bundle://web-css.cli/exemple_1.css"));

    Sys::println("Result:");
    Io::Emit emit{Sys::out()};
    co_try$(emit.flush());
    emit("\n");

    co_return Ok();
}
