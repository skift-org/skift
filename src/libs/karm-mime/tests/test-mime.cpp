#include <karm-mime/mime.h>
#include <karm-test/macros.h>

namespace Karm::Mime::Tests {

test$(parseMime) {
    auto mime = "text/plain"_mime;
    expectEq$(mime.type(), "text"s);
    expectEq$(mime.subtype(), "plain"s);
    expectEq$(mime.str(), "text/plain"s);
    return Ok();
}

} // namespace Karm::Mime::Tests
