// Some test case from https://github.com/MikeLankamp/fpm/blob/master/LICENSE
// License: MIT
// Copyright (c) 2019 Mike Lankamp

#include <karm-math/fixed.h>
#include <karm-math/funcs.h>
#include <karm-test/macros.h>

namespace Karm::Math::Tests {

test$("fixed-arithmethic-negation") {
    using P = i24f8;

    expectEq$(P(-13.125), -P(13.125));
    expectEq$(P(13.125), -P(-13.125));

    return Ok();
}

test$("fixed-arithmethic-addition") {
    using P = i24f8;

    expectEq$(P(10.75), P(3.5) + P(7.25));

    return Ok();
}

test$("fixed-arithmethic-subtraction") {
    using P = i24f8;

    expectEq$(P(-3.75), P(3.5) - P(7.25));

    return Ok();
}

test$("fixed-arithmethic-multiplication") {
    using P = i24f8;

    expectEq$(P(-25.375), P(3.5) * P(-7.25));

    return Ok();
}

test$("fixed-arithmethic-division") {
    using P = i24f8;

    expectEq$(P(3.5 / 7.25), P(3.5) / P(7.25));
    expectEq$(P(-3.5 / 7.25), P(-3.5) / P(7.25));
    expectEq$(P(3.5 / -7.25), P(3.5) / P(-7.25));
    expectEq$(P(-3.5 / -7.25), P(-3.5) / P(-7.25));

    return Ok();
}

test$("fixed-arithmethic-division-range") {
    using P = Fixed<i32, 12>;

    // These calculation will overflow and produce
    // wrong results without the intermediate type.
    expectEq$(P(32), P(256) / P(8));

    return Ok();
}

test$("fixed-abs") {
    using P = i24f8;

    expectEq$(P(13.125), abs(P(13.125)));
    expectEq$(P(13.125), abs(P(-13.125)));

    return Ok();
}

} // namespace Karm::Math::Tests
