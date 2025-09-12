#include <karm-test/macros.h>
import Karm.Core;

namespace Karm::Io::Tests {

test$("expr-either") {
    expect$(Re::match('a'_re | 'b'_re, ""s) == Match::NO);
    expect$(Re::match('a'_re | 'b'_re, "a"s) == Match::YES);
    expect$(Re::match('a'_re | 'b'_re, "b"s) == Match::YES);
    expect$(Re::match('a'_re | 'b'_re, "c"s) == Match::NO);
    expect$(Re::match('a'_re | 'b'_re, "ab"s) == Match::PARTIAL);

    return Ok();
}

test$("expr-chain") {
    expect$(Re::match('a'_re & 'b'_re, ""s) == Match::NO);
    expect$(Re::match('a'_re & 'b'_re, "ba"s) == Match::NO);
    expect$(Re::match('a'_re & 'b'_re, "ab"s) == Match::YES);
    expect$(Re::match('a'_re & 'b'_re, "abc"s) == Match::PARTIAL);

    return Ok();
}

test$("expr-negate") {
    expect$(Re::match(~'a'_re, ""s) == Match::NO);
    expect$(Re::match(~'a'_re, "b"s) == Match::YES);
    expect$(Re::match(~'a'_re, "a"s) == Match::NO);
    expect$(Re::match((~'a'_re) & 'a'_re, "ba"s) == Match::YES);
    expect$(Re::match((~'a'_re) & 'a'_re, "aa"s) == Match::NO);

    return Ok();
}

test$("expr-single") {

    expect$(Re::match('a'_re, ""s) == Match::NO);
    expect$(Re::match('a'_re, "a"s) == Match::YES);
    expect$(Re::match('a'_re, "b"s) == Match::NO);
    expect$(Re::match('a'_re, "aa"s) == Match::PARTIAL);

    return Ok();
}

} // namespace Karm::Io::Tests
