#include <karm-test/macros.h>
#include <web-dom/document.h>

namespace Web::Dom::Tests {

test$(domTree) {
    auto doc = makeStrong<Document>();
    return Ok();
}

} // namespace Web::Dom::Tests
