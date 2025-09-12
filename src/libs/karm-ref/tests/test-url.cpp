#include <karm-logger/logger.h>
#include <karm-test/macros.h>

import Karm.Ref;

namespace Karm::Ref::Tests {

test$("karm-ref-url-parse") {
    auto url = "http://example.com:1234/home"_url;

    expectEq$(url.scheme, "http"s);
    expectEq$(url.userInfo, ""s);
    expectEq$(url.host, "example.com"s);
    expectEq$(url.port, 1234uz);
    expectEq$(url.path.str(), "/home"s);
    expectEq$(url.query, ""s);
    expectEq$(url.fragment, ""s);

    auto url2 = "http://example.com:1234/home?query#fragment"_url;

    expectEq$(url2.scheme, "http"s);
    expectEq$(url2.userInfo, ""s);
    expectEq$(url2.host, "example.com"s);
    expectEq$(url2.port, 1234uz);
    expectEq$(url2.path.str(), "/home"s);
    expectEq$(url2.query, "query"s);
    expectEq$(url2.fragment, "fragment"s);

    auto url3 = "ftp://user@example.com:1234/home?query#fragment"_url;

    expectEq$(url3.scheme, "ftp"s);
    expectEq$(url3.userInfo, "user"s);
    expectEq$(url3.host, "example.com"s);
    expectEq$(url3.port, 1234uz);
    expectEq$(url3.path.str(), "/home"s);
    expectEq$(url3.query, "query"s);
    expectEq$(url3.fragment, "fragment"s);

    auto url4 = "./home"_url;

    expectEq$(url4.scheme, ""s);
    expectEq$(url4.userInfo, ""s);
    expectEq$(url4.host, ""s);
    expectEq$(url4.port, NONE);
    expectEq$(url4.path.str(), "./home"s);
    expectEq$(url4.query, ""s);
    expectEq$(url4.fragment, ""s);

    return Ok();
}

test$("karm-ref-url-parent-of") {
    expect$("http://example.com/"_url.isParentOf("http://example.com/"_url));
    expect$("http://example.com"_url.isParentOf("http://example.com/a"_url));
    expect$("http://example.com"_url.isParentOf("http://example.com/a/b"_url));

    expectNot$("http://example.com/a"_url.isParentOf("http://example.com"_url));
    expectNot$("http://example.com/a/b"_url.isParentOf("http://example.com"_url));

    return Ok();
}

test$("karm-ref-url-resolution-reference") {
    auto base = "http://a/b/c/d;p?q"_url;

    // https://datatracker.ietf.org/doc/html/rfc3986#section-5.4.1
    expectEq$(Url::resolveReference(base, "g:h"_url).take(), "g:h"_url);
    expectEq$(Url::resolveReference(base, "g"_url).take(), "http://a/b/c/g"_url);
    expectEq$(Url::resolveReference(base, "./g"_url).take(), "http://a/b/c/g"_url);
    expectEq$(Url::resolveReference(base, "g/"_url).take(), "http://a/b/c/g/"_url);
    expectEq$(Url::resolveReference(base, "//g"_url).take(), "http://g"_url);
    expectEq$(Url::resolveReference(base, "?y"_url).take(), "http://a/b/c/d;p?y"_url);
    expectEq$(Url::resolveReference(base, "g?y"_url).take(), "http://a/b/c/g?y"_url);
    expectEq$(Url::resolveReference(base, "#s"_url).take(), "http://a/b/c/d;p?q#s"_url);
    expectEq$(Url::resolveReference(base, "g#s"_url).take(), "http://a/b/c/g#s"_url);
    expectEq$(Url::resolveReference(base, "g?y#s"_url).take(), "http://a/b/c/g?y#s"_url);
    expectEq$(Url::resolveReference(base, ";x"_url).take(), "http://a/b/c/;x"_url);
    expectEq$(Url::resolveReference(base, "g;x"_url).take(), "http://a/b/c/g;x"_url);
    expectEq$(Url::resolveReference(base, "g;x?y#s"_url).take(), "http://a/b/c/g;x?y#s"_url);
    expectEq$(Url::resolveReference(base, ""_url).take(), "http://a/b/c/d;p?q"_url);
    expectEq$(Url::resolveReference(base, "."_url).take(), "http://a/b/c/"_url);
    expectEq$(Url::resolveReference(base, "./"_url).take(), "http://a/b/c/"_url);
    expectEq$(Url::resolveReference(base, ".."_url).take(), "http://a/b/"_url);
    expectEq$(Url::resolveReference(base, "../"_url).take(), "http://a/b/"_url);
    expectEq$(Url::resolveReference(base, "../g"_url).take(), "http://a/b/g"_url);
    expectEq$(Url::resolveReference(base, "../.."_url).take(), "http://a/"_url);
    expectEq$(Url::resolveReference(base, "../../"_url).take(), "http://a/"_url);
    expectEq$(Url::resolveReference(base, "../../g"_url).take(), "http://a/g"_url);

    // https://datatracker.ietf.org/doc/html/rfc3986#section-5.4.2
    expectEq$(Url::resolveReference(base, "../../../g"_url).take(), "http://a/g"_url);
    expectEq$(Url::resolveReference(base, "../../../../g"_url).take(), "http://a/g"_url);
    expectEq$(Url::resolveReference(base, "/./g"_url).take(), "http://a/g"_url);
    expectEq$(Url::resolveReference(base, "/../g"_url).take(), "http://a/g"_url);
    expectEq$(Url::resolveReference(base, "g."_url).take(), "http://a/b/c/g."_url);
    expectEq$(Url::resolveReference(base, ".g"_url).take(), "http://a/b/c/.g"_url);
    expectEq$(Url::resolveReference(base, "g.."_url).take(), "http://a/b/c/g.."_url);
    expectEq$(Url::resolveReference(base, "..g"_url).take(), "http://a/b/c/..g"_url);
    expectEq$(Url::resolveReference(base, "./../g"_url).take(), "http://a/b/g"_url);
    expectEq$(Url::resolveReference(base, "./g/."_url).take(), "http://a/b/c/g/"_url);
    expectEq$(Url::resolveReference(base, "g/./h"_url).take(), "http://a/b/c/g/h"_url);
    expectEq$(Url::resolveReference(base, "g/../h"_url).take(), "http://a/b/c/h"_url);
    expectEq$(Url::resolveReference(base, "g;x=1/./y"_url).take(), "http://a/b/c/g;x=1/y"_url);
    expectEq$(Url::resolveReference(base, "g;x=1/../y"_url).take(), "http://a/b/c/y"_url);
    expectEq$(Url::resolveReference(base, "g?y/./x"_url).take(), "http://a/b/c/g?y/./x"_url);
    expectEq$(Url::resolveReference(base, "g?y/../x"_url).take(), "http://a/b/c/g?y/../x"_url);
    expectEq$(Url::resolveReference(base, "g#s/./x"_url).take(), "http://a/b/c/g#s/./x"_url);
    expectEq$(Url::resolveReference(base, "g#s/../x"_url).take(), "http://a/b/c/g#s/../x"_url);
    expectEq$(Url::resolveReference(base, "http:g"_url, true).take(), "http:g"_url);
    expectEq$(Url::resolveReference(base, "http:g"_url, false).take(), "http://a/b/c/g"_url);

    return Ok();
}

} // namespace Karm::Ref::Tests
