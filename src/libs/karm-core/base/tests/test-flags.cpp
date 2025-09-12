import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Base::Tests {

enum struct Option : u8 {
    FOO = 1 << 0,
    BAR = 1 << 1,
    BAZ = 1 << 2,
};

test$("flags-empty") {
    Flags<Option> flags;

    expectEq$(flags.empty(), true);
    expectEq$(flags.any(), false);
    expectEq$(flags.raw(), 0);

    expectEq$(flags.has(Option::FOO), false);
    expectEq$(flags.has(Option::BAR), false);
    expectEq$(flags.has(Option::BAZ), false);

    return Ok();
}

test$("flags-all") {
    Flags<Option> flags{Option::FOO, Option::BAR, Option::BAZ};

    expectEq$(flags.empty(), false);
    expectEq$(flags.any(), true);
    expectEq$(flags.raw(), 0b111);

    expectEq$(flags.has(Option::FOO), true);
    expectEq$(flags.has(Option::BAR), true);
    expectEq$(flags.has(Option::BAZ), true);

    return Ok();
}

test$("flags-union") {
    Flags<Option> a = {Option::FOO};
    Flags<Option> b = {Option::BAZ};
    Flags<Option> c = a | b;

    expectEq$(c.empty(), false);
    expectEq$(c.any(), true);
    expectEq$(c.raw(), 0b101);

    expectEq$(c.has(Option::FOO), true);
    expectEq$(c.has(Option::BAR), false);
    expectEq$(c.has(Option::BAZ), true);

    return Ok();
}

test$("flags-clear") {
    Flags<Option> flags{Option::FOO, Option::BAR, Option::BAZ};

    expectEq$(flags.empty(), false);
    expectEq$(flags.any(), true);
    expectEq$(flags.raw(), 0b111);

    flags.clear();

    expectEq$(flags.empty(), true);
    expectEq$(flags.any(), false);
    expectEq$(flags.raw(), 0);

    return Ok();
}

} // namespace Karm::Base::Tests
