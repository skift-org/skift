#include <karm-sys/addr.h>
#include <karm-test/macros.h>

namespace Karm::Sys::Tests {

test$("ip4-eq") {
    expectEq$(Ip4::localhost(), Ip4::localhost());
    expectEq$(Ip4::localhost(80), Ip4::localhost(80));
    return Ok();
}

} // namespace Karm::Sys::Tests
