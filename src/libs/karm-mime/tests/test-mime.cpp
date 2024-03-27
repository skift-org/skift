#include <karm-mime/mime.h>
#include <karm-test/macros.h>

namespace Karm::Mime::Tests {

test$(parseMime) {
    auto mime = "text/plain"_mime;
    expectEq$(mime.type(), "text"_str);
    expectEq$(mime.subtype(), "plain"_str);
    expectEq$(mime.str(), "text/plain"_str);
    return Ok();
}

} // namespace Karm::Mime::Tests
