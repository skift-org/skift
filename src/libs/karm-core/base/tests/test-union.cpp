import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("union-niche") {
    Opt<Union<float, int, double>> test;

    expectEq$(sizeof(test), sizeof(Union<float, int, double>));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = 2;
    expectEq$(test.unwrap(), 2);
    expectEq$(test.take(), 2);
    expectEq$(test, NONE);
    test = 1.0f;
    expectEq$(test.has(), true);

    return Ok();
}

} // namespace Karm::Base::Tests
