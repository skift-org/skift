#include <karm-test/macros.h>

import Karm.Core;

namespace Karm::Base::Tests {

test$("opt-default-constructor") {
    Opt<int> opt{};

    expect$(not opt.has());

    return Ok();
}

test$("opt-constructed") {
    Opt<int> opt{420};

    expect$(opt.has());
    expectEq$(opt.unwrap(), 420);

    return Ok();
}

test$("opt-assign") {
    Opt<int> opt{};

    opt = 420;

    expect$(opt.has());
    expectEq$(opt.unwrap(), 420);

    return Ok();
}

test$("opt-assign-none") {
    Opt<int> opt{420};

    opt = NONE;

    expect$(not opt.has());

    return Ok();
}

test$("opt-unwrap") {
    Opt<int> opt{420};

    expectEq$(opt.unwrap(), 420);

    return Ok();
}

test$("opt-take") {
    Opt<int> opt{420};

    expectEq$(opt.take(), 420);
    expect$(not opt.has());

    return Ok();
}

test$("opt-equal") {
    Opt<int> opt = NONE;
    expectEq$(opt, NONE);
    expectNe$(opt, 42);

    opt = 42;
    expectEq$(opt, 42);
    expectNe$(opt, NONE);

    return Ok();
}

test$("bool-niche") {
    Opt<bool> test;

    expectEq$(sizeof(test), sizeof(bool));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = true;
    expectEq$(test.unwrap(), true);
    expectEq$(test.take(), true);
    expectEq$(test, NONE);
    test = false;
    expectEq$(test.has(), true);
    test = 2;
    expectEq$(test.has(), true);

    return Ok();
}

enum struct TestEnum {
    A,
    B,

    _LEN,
};

test$("bool-niche") {
    Opt<TestEnum> test;

    expectEq$(sizeof(test), sizeof(TestEnum));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = TestEnum::A;
    expectEq$(test.unwrap(), TestEnum::A);
    expectEq$(test.take(), TestEnum::A);
    expectEq$(test, NONE);
    test = TestEnum::_LEN;
    expectEq$(test.has(), true);

    return Ok();
}

} // namespace Karm::Base::Tests
