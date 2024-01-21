#include <karm-base/async.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

Async::_Task<int> taskValue() {
    co_return 42;
}

} // namespace Karm::Base::Tests
