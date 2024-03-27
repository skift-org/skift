#include <karm-io/expr.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$(exprEither) {
    auto RE_EITHER_A_B = 'a'_re | 'b'_re;

    expect$(Re::match(RE_EITHER_A_B, "a") == Match::YES);
    expect$(Re::match(RE_EITHER_A_B, "b") == Match::YES);
    expect$(Re::match(RE_EITHER_A_B, "c") == Match::NO);
    expect$(Re::match(RE_EITHER_A_B, "ab") == Match::PARTIAL);

    return Ok();
}

test$(exprSingle) {
    auto RE_SINGLE_A = 'a'_re;

    expect$(Re::match(RE_SINGLE_A, "a") == Match::YES);
    expect$(Re::match(RE_SINGLE_A, "b") == Match::NO);
    expect$(Re::match(RE_SINGLE_A, "aa") == Match::PARTIAL);

    return Ok();
}

test$(exprNegate) {
    expect$(Re::match(~'a'_re, "") == Match::NO);
    expect$(Re::match(~'a'_re, "b") == Match::YES);
    expect$(Re::match(~'a'_re, "a") == Match::NO);
    expect$(Re::match((~'a'_re) & 'a'_re, "ba") == Match::YES);
    expect$(Re::match((~'a'_re) & 'a'_re, "aa") == Match::NO);

    return Ok();
}

} // namespace Karm::Io::Tests
