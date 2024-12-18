#include <karm-io/aton.h>
#include <karm-math/funcs.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$("atoi") {
    expectEq$(Io::atoi("0"s), 0);
    expectEq$(Io::atoi("1"s), 1);
    expectEq$(Io::atoi("10"s), 10);
    expectEq$(Io::atoi("100"s), 100);

    expectEq$(Io::atoi("-1"s), -1);
    expectEq$(Io::atoi("-10"s), -10);
    expectEq$(Io::atoi("-100"s), -100);

    return Ok();
}

test$("atof") {
    expect$(Math::epsilonEq(try$(Io::atof("0.0"s)), 0.0));
    expect$(Math::epsilonEq(try$(Io::atof("0.1"s)), 0.1));
    expect$(Math::epsilonEq(try$(Io::atof("0.5"s)), 0.5));

    expect$(Math::epsilonEq(try$(Io::atof(".0"s)), 0.0));
    expect$(Math::epsilonEq(try$(Io::atof(".1"s)), 0.1));
    expect$(Math::epsilonEq(try$(Io::atof(".5"s)), 0.5));

    expect$(Math::epsilonEq(try$(Io::atof("0"s)), 0.));
    expect$(Math::epsilonEq(try$(Io::atof("1"s)), 1.));
    expect$(Math::epsilonEq(try$(Io::atof("5"s)), 5.));

    expect$(Math::epsilonEq(try$(Io::atof("0."s)), 0.));
    expect$(Math::epsilonEq(try$(Io::atof("1."s)), 1.));
    expect$(Math::epsilonEq(try$(Io::atof("5."s)), 5.));

    expect$(Math::epsilonEq(try$(Io::atof("-0"s)), -0.));
    expect$(Math::epsilonEq(try$(Io::atof("-1"s)), -1.));
    expect$(Math::epsilonEq(try$(Io::atof("-5"s)), -5.));

    expect$(Math::epsilonEq(try$(Io::atof("-0."s)), -0.));
    expect$(Math::epsilonEq(try$(Io::atof("-1."s)), -1.));
    expect$(Math::epsilonEq(try$(Io::atof("-5."s)), -5.));

    expect$(Math::epsilonEq(try$(Io::atof("-.0"s)), -.0));
    expect$(Math::epsilonEq(try$(Io::atof("-.1"s)), -.1));
    expect$(Math::epsilonEq(try$(Io::atof("-.5"s)), -.5));

    expect$(Math::epsilonEq(try$(Io::atof("-0.0"s)), -0.0));
    expect$(Math::epsilonEq(try$(Io::atof("-0.1"s)), -0.1));
    expect$(Math::epsilonEq(try$(Io::atof("-0.5"s)), -0.5));

    return Ok();
}

} // namespace Karm::Io::Tests
