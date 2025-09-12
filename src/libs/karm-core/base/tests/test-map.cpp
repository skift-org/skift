import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("map-get-or-default") {
    Map<int, int> map;

    // Test getting a key that does not exist
    expectEq$(map.getOrDefault(1), 0);

    // Test getting a key that exists
    map.put(1, 10);
    expectEq$(map.getOrDefault(1), 10);

    // Test getting another key that does not exist
    expectEq$(map.getOrDefault(2), 0);

    // Test putting a new key and getting it
    map.put(2, 20);
    expectEq$(map.getOrDefault(2), 20);

    return Ok();
}

} // namespace Karm::Base::Tests
