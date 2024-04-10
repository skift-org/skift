#include <karm-base/vec.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$(defaultConstructedVec) {
    Vec<int> vec;
    expectEq$(vec.len(), 0uz);
    expectEq$(vec.cap(), 0uz);
    expectEq$(vec.buf(), nullptr);
    return Ok();
}

} // namespace Karm::Base::Tests
