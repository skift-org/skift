#include <karm-mime/mime.h>
#include <karm-test/macros.h>

namespace Karm::Mime::Tests {

test$(parseMime) {
    auto mime = "text/plain"_mime;
    expectEq$(mime.type(), Str{"text"});
    expectEq$(mime.subtype(), Str{"plain"});
    expectEq$(mime.str(), Str{"text/plain"});
    return Ok();
}

} // namespace Karm::Mime::Tests
