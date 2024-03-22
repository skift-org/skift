#include <karm-io/expr.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$(exprEither) {
    auto RE_EITHER_A_B = Re::single('a') | Re::single('b');

    expect$(Re::match(RE_EITHER_A_B, "a") == Match::YES);
    expect$(Re::match(RE_EITHER_A_B, "b") == Match::YES);
    expect$(Re::match(RE_EITHER_A_B, "c") == Match::NO);

    return Ok();
}

test$(exprSingle) {
    auto RE_SINGLE_A = Re::single('a');
    expect$(Re::match(RE_SINGLE_A, "a") == Match::YES);
    expect$(Re::match(RE_SINGLE_A, "b") == Match::NO);

    auto RE_SINGLE_A_B = Re::single('a', 'b');
    expect$(Re::match(RE_SINGLE_A_B, "a") == Match::YES);
    expect$(Re::match(RE_SINGLE_A_B, "b") == Match::YES);
    expect$(Re::match(RE_SINGLE_A_B, "c") == Match::NO);

    return Ok();
}

} // namespace Karm::Io::Tests
