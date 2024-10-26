#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-test/macros.h>

namespace Karm::Mime::Tests {

test$("karm-mime-url-parse") {
    auto url = "http://example.com:1234/home"_url;

    expectEq$(url.scheme, "http"s);
    expectEq$(url.authority, ""s);
    expectEq$(url.host, "example.com"s);
    expectEq$(url.port, 1234uz);
    expectEq$(url.path.str(), "/home"s);
    expectEq$(url.query, ""s);
    expectEq$(url.fragment, ""s);

    auto url2 = "http://example.com:1234/home?query#fragment"_url;

    expectEq$(url2.scheme, "http"s);
    expectEq$(url2.authority, ""s);
    expectEq$(url2.host, "example.com"s);
    expectEq$(url2.port, 1234uz);
    expectEq$(url2.path.str(), "/home"s);
    expectEq$(url2.query, "query"s);
    expectEq$(url2.fragment, "fragment"s);

    auto url3 = "ftp://user@example.com:1234/home?query#fragment"_url;

    expectEq$(url3.scheme, "ftp"s);
    expectEq$(url3.authority, "user"s);
    expectEq$(url3.host, "example.com"s);
    expectEq$(url3.port, 1234uz);
    expectEq$(url3.path.str(), "/home"s);
    expectEq$(url3.query, "query"s);
    expectEq$(url3.fragment, "fragment"s);

    auto url4 = "./home"_url;

    expectEq$(url4.scheme, ""s);
    expectEq$(url4.authority, ""s);
    expectEq$(url4.host, ""s);
    expectEq$(url4.port, NONE);
    expectEq$(url4.path.str(), "./home"s);
    expectEq$(url4.query, ""s);
    expectEq$(url4.fragment, ""s);

    return Ok();
}

test$("karm-mime-url-parent-of") {
    expect$("http://example.com/"_url.isParentOf("http://example.com/"_url));
    expect$("http://example.com"_url.isParentOf("http://example.com/a"_url));
    expect$("http://example.com"_url.isParentOf("http://example.com/a/b"_url));

    expectNot$("http://example.com/a"_url.isParentOf("http://example.com"_url));
    expectNot$("http://example.com/a/b"_url.isParentOf("http://example.com"_url));

    return Ok();
}

} // namespace Karm::Mime::Tests
