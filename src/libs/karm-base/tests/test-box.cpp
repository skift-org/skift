#include <karm-base/box.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("box-niche") {
    Opt<Box<int>> test;

    expectEq$(sizeof(test), sizeof(Box<int>));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = makeBox<int>(5);
    expectEq$(test.unwrap(), 5);
    expectEq$(test.take(), 5);
    expectEq$(test, NONE);
    test = makeBox<int>();
    expectEq$(test.has(), true);

    return Ok();
}

} // namespace Karm::Base::Tests
