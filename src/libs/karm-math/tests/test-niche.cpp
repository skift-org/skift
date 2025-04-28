#include <karm-base/opt.h>
#include <karm-math/float.h>
#include <karm-test/macros.h>

namespace Karm::Math::Tests {

test$("f64-niche") {
    Opt<f64> test;

    expectEq$(sizeof(test), sizeof(f64));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = 5;
    expectEq$(test.unwrap(), 5);
    expectEq$(test.take(), 5);
    expectEq$(test, NONE);
    test = Math::NAN;
    expectEq$(test.has(), true);

    return Ok();
}

test$("f32-niche") {
    Opt<f32> test;

    expectEq$(sizeof(test), sizeof(f32));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = 5;
    expectEq$(test.unwrap(), 5);
    expectEq$(test.take(), 5);
    expectEq$(test, NONE);
    test = Math::NAN;
    expectEq$(test.has(), true);

    return Ok();
}

test$("f16-niche") {
    Opt<f16> test;

    expectEq$(sizeof(test), sizeof(f16));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = 5;
    expectEq$(test.unwrap(), 5);
    expectEq$(test.take(), 5);
    expectEq$(test, NONE);
    test = Math::NAN;
    expectEq$(test.has(), true);

    return Ok();
}

} // namespace Karm::Math::Tests
