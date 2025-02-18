#include <karm-base/ring.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("ring-avoid-pop-back-underflow") {
    Ring<int> ring(5);

    ring.pushBack(0);
    ring.pushBack(1);
    ring.pushBack(2);
    ring.pushBack(3);
    ring.pushBack(4);

    expectEq$(ring._head, 0u);
    ring.popBack();
    expectEq$(ring._head, 4u);

    return Ok();
}

} // namespace Karm::Base::Tests
