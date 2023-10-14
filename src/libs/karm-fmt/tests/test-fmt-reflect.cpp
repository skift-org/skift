#include <karm-fmt/fmt.h>
#include <karm-test/macros.h>

struct Foo {
    int bar;
    int baz;
};

Reflectable$(Foo, bar, baz);

struct Baz {
    Foo foo;
    int qux;
};

Reflectable$(Baz, foo, qux);

test$(fmtReflectable) {
    auto fooStr = try$(Fmt::format("{}", Foo{1, 2}));
    expectEq$(fooStr, "Foo(bar=1, baz=2)");

    auto bazStr = try$(Fmt::format("{}", Baz{{1, 2}, 3}));
    expectEq$(bazStr, "Baz(foo=Foo(bar=1, baz=2), qux=3)");

    return Ok();
}
