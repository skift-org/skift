#include <karm-test/macros.h>

import Karm.Core;

namespace Karm::Math::Tests {

test$("floori") {
    expectEq$(0, floori(0.0));
    expectEq$(0, floori(0.1));
    expectEq$(0, floori(0.5));
    expectEq$(0, floori(0.9));
    expectEq$(1, floori(1.0));
    expectEq$(-1, floori(-0.1));
    expectEq$(-1, floori(-0.5));
    expectEq$(-1, floori(-0.9));

    return Ok();
}

test$("ceili") {
    expectEq$(0, ceili(0.0));
    expectEq$(1, ceili(0.1));
    expectEq$(1, ceili(0.5));
    expectEq$(1, ceili(0.9));
    expectEq$(1, ceili(1.0));
    expectEq$(0, ceili(-0.1));
    expectEq$(0, ceili(-0.5));
    expectEq$(0, ceili(-0.9));

    return Ok();
}

test$("roundi") {
    expectEq$(0, roundi(0.0));

    expectEq$(0, roundi(0.1));
    expectEq$(1, roundi(0.5));
    expectEq$(1, roundi(0.9));
    expectEq$(1, roundi(1.0));

    expectEq$(0, roundi(-0.1));
    expectEq$(-1, roundi(-0.5));
    expectEq$(-1, roundi(-0.9));
    expectEq$(-1, roundi(-1.0));

    return Ok();
}

} // namespace Karm::Math::Tests
