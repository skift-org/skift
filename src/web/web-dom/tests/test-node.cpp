#include <karm-test/macros.h>
#include <web-dom/node.h>

namespace Web::Dom::Tests {

test$(domTree) {
    auto &root = *new Node();

    delete &root;
    return Ok();
}

} // namespace Web::Dom::Tests
