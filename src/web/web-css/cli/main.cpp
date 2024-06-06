#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <web-css/mod.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    Sys::println("Parsing:");
    co_try$(Web::Css::fetchStylesheet("bundle://web-css.cli/exemple_1.css"_url));
    co_return Ok();
}
