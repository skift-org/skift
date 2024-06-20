#include <karm-test/macros.h>
#include <vaev-dom/document.h>

namespace Vaev::Dom::Tests {

test$("dom-tree") {
    auto doc = makeStrong<Document>();
    return Ok();
}

} // namespace Vaev::Dom::Tests
