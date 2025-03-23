#include <karm-io/aton.h>
#include <karm-math/funcs.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$("atoi") {
    expectEq$(Io::atoi("0"s), 0);
    expectEq$(Io::atoi("1"s), 1);
    expectEq$(Io::atoi("10"s), 10);
    expectEq$(Io::atoi("100"s), 100);
    expectEq$(Io::atoi("+100"s), 100);

    expectEq$(Io::atoi("-1"s), -1);
    expectEq$(Io::atoi("-10"s), -10);
    expectEq$(Io::atoi("-100"s), -100);

    expectEq$(Io::atoi("+"s), NONE);
    expectEq$(Io::atoi("-"s), NONE);
    expectEq$(Io::atoi("+-100"s), NONE);
    expectEq$(Io::atoi("-+100"s), NONE);

    return Ok();
}

test$("atof") {
    expect$(Math::epsilonEq(try$(Io::atof("0.0"s)), 0.0));
    expect$(Math::epsilonEq(try$(Io::atof("0.1"s)), 0.1));
    expect$(Math::epsilonEq(try$(Io::atof("0.5"s)), 0.5));
    expect$(Math::epsilonEq(try$(Io::atof("+0.5"s)), 0.5));

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

    expect$(Math::epsilonEq(try$(Io::atof("2.5e3"s)), 2500.0));
    expect$(Math::epsilonEq(try$(Io::atof("-1.0e6"s)), -1000000.0));
    expect$(Math::epsilonEq(try$(Io::atof("3e0"s)), 3.0));
    expect$(Math::epsilonEq(try$(Io::atof("-7.89e2"s)), -789.0));
    expect$(Math::epsilonEq(try$(Io::atof("5.00e-3"s)), 0.005));

    expectEq$(Io::atof("+"s), NONE);
    expectEq$(Io::atof("."s), NONE);
    expectEq$(Io::atof("+."s), NONE);
    expectEq$(Io::atof("-"s), NONE);
    expectEq$(Io::atof("-+100"s), NONE);
    expectEq$(Io::atof("+-100"s), NONE);

    expect$(Math::epsilonEq(try$(Io::atof("1px"s)), 1.0));

    return Ok();
}

} // namespace Karm::Io::Tests
