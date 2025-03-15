#include <karm-test/macros.h>

import Karm.Http;

namespace Karm::Http::Tests {

test$("parse-unparse-http-request-no-header") {
    auto rawRequest =
        "GET / HTTP/1.1\r\n"
        "\r\n"s;

    Io::SScan s{rawRequest};
    auto request = try$(Request::parse(s));

    expectEq$(request.path, ""_path);
    expectEq$(request.method, GET);

    auto expectedVersion = Version{1u, 1u};
    expectEq$(request.version, expectedVersion);

    Io::StringWriter sw;
    try$(request.unparse(sw));

    expectEq$(rawRequest, sw.take());

    return Ok();
}

test$("parse-unparse-http-request-with-header") {
    auto rawRequest =
        "POST / HTTP/1.2\r\n"
        "Host: odoo.com\r\n"
        "User-Agent: PM\r\n"
        "Accept: */*\r\n"
        "\r\n"s;

    Io::SScan s{rawRequest};
    auto request = try$(Request::parse(s));

    expectEq$(request.path, ""_path);
    expectEq$(request.method, POST);

    auto expectedVersion = Version{1u, 2u};
    expectEq$(request.version.major, expectedVersion.major);
    expectEq$(request.version.minor, expectedVersion.minor);

    Io::StringWriter sw;
    try$(request.unparse(sw));
    auto unparsedReq = sw.take();

    // the order of headers does not matter, so we cannot compare the strings
    expect$(contains(unparsedReq, "Host: odoo.com\r\n"s));
    expect$(contains(unparsedReq, "User-Agent: PM\r\n"s));
    expect$(contains(unparsedReq, "Accept: */*\r\n"s));

    auto unparsedReqLen = unparsedReq.len();
    expectEq$(unparsedReqLen, rawRequest.len());
    expectEq$(Slice(unparsedReq.buf() + unparsedReqLen - 4, unparsedReq.buf() + unparsedReqLen), "\r\n\r\n"s);

    return Ok();
}

} // namespace Karm::Http::Tests
