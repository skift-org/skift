import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("will-add-overflow") {
    expect$(willAddOverflow<u32>(0xFFFFFFFF, 1));
    expect$(willAddOverflow<i32>(Limits<i32>::MAX, 1));

    return Ok();
}

test$("will-add-underflow") {
    expect$(willAddUnderflow<i32>(Limits<i32>::MIN, -1));

    return Ok();
}

test$("will-sub-overflow") {
    expect$(willSubOverflow<u32>(0, -1));
    expect$(willSubOverflow<i32>(Limits<i32>::MAX, -1));

    return Ok();
}

test$("will-sub-underflow") {
    expect$(willSubUnderflow<u32>(0, 1));
    expect$(willSubUnderflow<i32>(Limits<i32>::MIN, 1));

    return Ok();
}

} // namespace Karm::Base::Tests
