#include <karm-test/macros.h>

import Karm.Core;

namespace Karm::Base::Tests {

test$("array-niche") {
    using Test = Array<bool, 2>;
    Test value = Test{true, false};
    Opt<Test> test;

    expectEq$(sizeof(test), sizeof(Test));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = Test{true, false};
    expectEq$(test.unwrap(), value);
    expectEq$(test.take(), value);
    expectEq$(test, NONE);
    test = Test{0, 0};
    expectEq$(test.has(), true);

    return Ok();
}

} // namespace Karm::Base::Tests
