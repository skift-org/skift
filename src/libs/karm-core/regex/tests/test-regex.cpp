#include <karm-test/macros.h>
import Karm.Core;

namespace Karm::Regex::Tests {

test$("regex-atom") {
    expect$("a"_regex.contains("a"));
    expectNot$("a"_regex.contains("b"));
    expect$("\\"_regex.contains("\\"));

    return Ok();
}

test$("regex-chain") {
    expect$("ab"_regex.contains("ab"));
    expect$("abc"_regex.contains("abc"));
    expectNot$("abc"_regex.contains("cba"));

    return Ok();
}

test$("regex-disjunction") {
    auto re = "a|b|c"_regex;
    expect$(re.contains("a"));
    expect$(re.contains("b"));
    expect$(re.contains("c"));
    expectNot$(re.contains("d"));

    return Ok();
}

} // namespace Karm::Regex::Tests