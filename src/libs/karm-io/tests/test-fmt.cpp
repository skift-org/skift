#include <karm-base/enum.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

template <typename T>
Res<> testCase(Str expected, T const& value, Str format = "") {
    auto str = try$(toStr(value, format));
    if (str != expected) {
        logError("expected: {}, got: {}", expected, str);
        return Error::invalidData("value does not match expected");
    }
    return Ok();
}

// MARK: Number Formatting -----------------------------------------------------

test$("fmt-unsigned-number") {
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
    try$(testCase("0x499602d2", 1234567890u, "#x"));

    try$(testCase("586524", 586524, "04"));

    return Ok();
}

test$("fmt-signed-number") {
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

    try$(testCase("0000", 0, "04"));
    try$(testCase("0001", 1, "04"));
    try$(testCase("1000", 1000, "04"));

    return Ok();
}

enum struct MyEnum {
    BAR,
    BAZ,
};

test$("fmt-enum") {
    try$(testCase("(Karm::Io::Tests::MyEnum 0)", MyEnum::BAR));
    try$(testCase("(Karm::Io::Tests::MyEnum 1)", MyEnum::BAZ));

    return Ok();
}

enum struct MyEnumReflectable {
    BAR,
    BAZ,

    _LEN,
};

test$("fmt-enum-bounded") {
    try$(testCase("BAR", MyEnumReflectable::BAR));
    try$(testCase("BAZ", MyEnumReflectable::BAZ));

    return Ok();
}

// MARK: Boolean Formatting ----------------------------------------------------

test$("fmt-bool") {
    try$(testCase("True", true));
    try$(testCase("False", false));
    return Ok();
}

// MARK: Format Optionals ------------------------------------------------------

test$("fmt-optionals") {
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

// MARK: Format Results --------------------------------------------------------

test$("fmt-error") {
    try$(testCase("test", Error::invalidData("test")));

    return Ok();
}

test$("fmt-result") {
    try$(testCase("Ok", Ok()));
    try$(testCase("1", Ok(1)));
    try$(testCase("test", Error::invalidData("test")));

    return Ok();
}

// MARK: Format Union ----------------------------------------------------------

test$("fmt-union") {
    using U = Union<int, Str>;

    try$(testCase("1", U{1}));
    try$(testCase("test", U{"test"s}));

    return Ok();
}

// MARK: Format References -----------------------------------------------------

test$("fmt-references") {
    auto number = makeRc<int>(123);
    try$(testCase("123", number));

    Opt<Rc<Str>> str = makeRc<Str>("test");
    try$(testCase("test", str));

    Weak<Str> weakStr = *str;
    try$(testCase("test", weakStr));

    str = NONE;
    try$(testCase("None", weakStr));

    return Ok();
}

// MARK: Format Sliceable ------------------------------------------------------

test$("fmt-sliceable") {
    Array arr{1, 2, 3};
    try$(testCase("[1, 2, 3]", arr));
    return Ok();
}

// MARK: Format Map ------------------------------------------------------------

test$("fmt-map") {
    Map<int, int> map;
    map.put(1, 2);
    map.put(3, 4);

    try$(testCase("{1: 2, 3: 4}", map));
    return Ok();
}

// MARK: Format String ---------------------------------------------------------

test$("fmt-string") {
    try$(testCase("test", Str("test")));
    return Ok();
}

// MARK: Format Time -----------------------------------------------------------

// MARK: Format Tuple ----------------------------------------------------------

test$("fmt-cons") {
    try$(testCase("{1, 2}", Pair{1, 2}));
    return Ok();
}

test$("fmt-tuple") {
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
