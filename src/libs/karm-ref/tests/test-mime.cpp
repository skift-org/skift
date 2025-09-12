#include <karm-test/macros.h>

import Karm.Ref;

namespace Karm::Ref::Tests {

test$("karm-ref-mime-parse") {
    auto mime = "text/plain"_mime;
    expectEq$(mime.type(), "text"s);
    expectEq$(mime.subtype(), "plain"s);
    expectEq$(mime.str(), "text/plain"s);
    return Ok();
}

} // namespace Karm::Ref::Tests
