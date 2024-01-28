#include <karm-io/fmt.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>

struct Foo {
    int bar;
    int baz;
};

struct Baz {
    Foo foo;
    int qux;
};

Reflectable$(Baz, foo, qux);
Reflectable$(Foo, bar, baz);

namespace Karm::Io::Tests {

template <typename T>
Res<> testCase(Str expected, T const &value, Str format = "") {
    auto str = try$(toStr(value, format));
    if (str != expected) {
        logError("expected: {}, got: {}", expected, str);
        return Error::invalidData("value does not match expected");
    }
    return Ok();
}

/* --- Number Formatting ---------------------------------------------------- */

test$(fmtUnsignedNumber) {
    // Unsigned
    try$(testCase("0", 0u));
    try$(testCase("1", 1u));
    try$(testCase("123", 123u));
    try$(testCase("1234567890", 1234567890u));

    try$(testCase("0", 0u, "d"));
    try$(testCase("1", 1u, "d"));
    try$(testCase("123", 123u, "d"));
    try$(testCase("1234567890", 1234567890u, "d"));

    try$(testCase("0", 0u, "x"));
    try$(testCase("1", 1u, "x"));
    try$(testCase("7b", 123u, "x"));
    try$(testCase("499602d2", 1234567890u, "x"));

    try$(testCase("586524", 586524, "04"));

    return Ok();
}

test$(fmtSignedNumber) {
    // Signed
    try$(testCase("0", 0));
    try$(testCase("1", 1));
    try$(testCase("-1", -1));
    try$(testCase("123", 123));
    try$(testCase("-123", -123));
    try$(testCase("1234567890", 1234567890));
    try$(testCase("-1234567890", -1234567890));

    try$(testCase("0", 0, "d"));
    try$(testCase("1", 1, "d"));
    try$(testCase("-1", -1, "d"));
    try$(testCase("123", 123, "d"));
    try$(testCase("-123", -123, "d"));
    try$(testCase("1234567890", 1234567890, "d"));
    try$(testCase("-1234567890", -1234567890, "d"));

    try$(testCase("0", 0, "x"));
    try$(testCase("1", 1, "x"));
    try$(testCase("-1", -1, "x"));
    try$(testCase("7b", 123, "x"));
    try$(testCase("-7b", -123, "x"));
    try$(testCase("499602d2", 1234567890, "x"));
    try$(testCase("-499602d2", -1234567890, "x"));

    return Ok();
}

/* --- Format Optionals ----------------------------------------------------- */

test$(fmtOptionals) {
    try$(testCase("None", NONE));

    try$(testCase("Ok", Ok()));

    try$(testCase("1", Ok(1)));

    try$(testCase("None", Opt<usize>{}));
    try$(testCase("123", Opt<usize>{123}));
    try$(testCase("None", Opt<usize>{}, "d"));
    try$(testCase("123", Opt<usize>{123}, "d"));
    try$(testCase("None", Opt<usize>{}, "x"));
    try$(testCase("7b", Opt<usize>{123}, "x"));

    return Ok();
}

test$(fmtError) {
    try$(testCase("test", Error::invalidData("test")));

    return Ok();
}

test$(fmtResult) {
    try$(testCase("Ok", Ok()));
    try$(testCase("1", Ok(1)));
    try$(testCase("test", Error::invalidData("test")));

    return Ok();
}

/* --- Format References ---------------------------------------------------- */

test$(fmtReferences) {
    auto number = makeStrong<int>(123);
    try$(testCase("123", number));

    Opt<Strong<Str>> str = makeStrong<Str>("test");
    try$(testCase("test", str));

    Weak<Str> weakStr = *str;
    try$(testCase("test", weakStr));

    str = NONE;
    try$(testCase("None", weakStr));

    return Ok();
}

/* --- Format Reflectable --------------------------------------------------- */

test$(fmtReflectable) {
    auto fooStr = try$(Io::format("{}", Foo{1, 2}));
    expectEq$(fooStr, "Foo(bar=1, baz=2)");

    auto bazStr = try$(Io::format("{}", Baz{{1, 2}, 3}));
    expectEq$(bazStr, "Baz(foo=Foo(bar=1, baz=2), qux=3)");

    return Ok();
}

/* --- Format Sliceable ----------------------------------------------------- */

test$(fmtSliceable) {
    Array arr{1, 2, 3};
    try$(testCase("{1, 2, 3}", arr));
    return Ok();
}

/* --- Format String -------------------------------------------------------- */

test$(fmtString) {
    try$(testCase("test", Str("test")));
    return Ok();
}

/* --- Format Time ---------------------------------------------------------- */

/* --- Format Tuple --------------------------------------------------------- */

test$(fmtCons) {
    try$(testCase("{1, 2}", Cons{1, 2}));
    return Ok();
}

test$(fmtTuple) {
    try$(testCase("{1}", Tuple{1}));
    try$(testCase("{1, 2}", Tuple{1, 2}));
    try$(testCase("{1, 2, 3}", Tuple{1, 2, 3}));
    try$(testCase("{1, 2, 3, 4}", Tuple{1, 2, 3, 4}));
    try$(testCase("{1, 2, 3, 4, 5}", Tuple{1, 2, 3, 4, 5}));
    try$(testCase("{1, 2, 3, 4, 5, 6}", Tuple{1, 2, 3, 4, 5, 6}));
    try$(testCase("{1, 2, 3, 4, 5, 6, 7}", Tuple{1, 2, 3, 4, 5, 6, 7}));
    try$(testCase("{1, 2, 3, 4, 5, 6, 7, 8}", Tuple{1, 2, 3, 4, 5, 6, 7, 8}));

    return Ok();
}

} // namespace Karm::Io::Tests
