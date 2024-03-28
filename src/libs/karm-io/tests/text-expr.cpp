#include <karm-io/expr.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$(exprEither) {
    expect$(Re::match('a'_re | 'b'_re, ""_str) == Match::NO);
    expect$(Re::match('a'_re | 'b'_re, "a"_str) == Match::YES);
    expect$(Re::match('a'_re | 'b'_re, "b"_str) == Match::YES);
    expect$(Re::match('a'_re | 'b'_re, "c"_str) == Match::NO);
    expect$(Re::match('a'_re | 'b'_re, "ab"_str) == Match::PARTIAL);

    return Ok();
}

test$(exprChain) {
    expect$(Re::match('a'_re & 'b'_re, ""_str) == Match::NO);
    expect$(Re::match('a'_re & 'b'_re, "ba"_str) == Match::NO);
    expect$(Re::match('a'_re & 'b'_re, "ab"_str) == Match::YES);
    expect$(Re::match('a'_re & 'b'_re, "abc"_str) == Match::PARTIAL);

    return Ok();
}

test$(exprNegate) {
    expect$(Re::match(~'a'_re, ""_str) == Match::NO);
    expect$(Re::match(~'a'_re, "b"_str) == Match::YES);
    expect$(Re::match(~'a'_re, "a"_str) == Match::NO);
    expect$(Re::match((~'a'_re) & 'a'_re, "ba"_str) == Match::YES);
    expect$(Re::match((~'a'_re) & 'a'_re, "aa"_str) == Match::NO);

    return Ok();
}

test$(exprSingle) {

    expect$(Re::match('a'_re, ""_str) == Match::NO);
    expect$(Re::match('a'_re, "a"_str) == Match::YES);
    expect$(Re::match('a'_re, "b"_str) == Match::NO);
    expect$(Re::match('a'_re, "aa"_str) == Match::PARTIAL);

    return Ok();
}

} // namespace Karm::Io::Tests
