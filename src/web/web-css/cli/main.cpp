#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-css/lexer.h>
#include <web-css/parser.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    Sys::println("Parsing:");
    co_try$(Web::Css::parseStylesheet("bundle://web-css.cli/exemple_1.css"));
    co_return Ok();
}
