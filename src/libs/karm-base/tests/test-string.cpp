#include <karm-base/string.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$(defaultConstructedString) {
    String str;
    expectEq$(str.len(), 0uz);
    expectEq$(str, ""s);
    // We have to use _buf here because in the case of a default
    // constructed String, buf() will lie to us and return ""
    // but internally it is nullptr and no buffer has been allocated.
    expectEq$(str._buf, nullptr);

    return Ok();
}

} // namespace Karm::Base::Tests
