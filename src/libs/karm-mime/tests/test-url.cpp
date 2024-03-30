#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-test/macros.h>

namespace Karm::Mime::Tests {

test$(urlParentOf) {
    expect$("http://example.com/"_url.isParentOf("http://example.com/"_url));
    expect$("http://example.com"_url.isParentOf("http://example.com/a"_url));
    expect$("http://example.com"_url.isParentOf("http://example.com/a/b"_url));

    expectNot$("http://example.com/a"_url.isParentOf("http://example.com"_url));
    expectNot$("http://example.com/a/b"_url.isParentOf("http://example.com"_url));

    return Ok();
}

} // namespace Karm::Mime::Tests
