#include <karm-net/http/http.h>
#include <karm-test/macros.h>

namespace Karm::Print::Tests {

test$("read-http-response-good-body") {
    auto rawResponse =
        "HTTP/1.1 200 OK\r\n"
        "Server: Apache\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "foo"s;

    Io::BufReader br{bytes(rawResponse)};

    auto response = try$(Net::Http::Response::read(br));

    expectEq$(response.code, Net::Http::Code{200});

    auto expectedVersion = Net::Http::Version{1u, 1u};
    expectEq$(response.version.major, expectedVersion.major);
    expectEq$(response.version.minor, expectedVersion.minor);

    expectEq$(response.headers.len(), 2u);
    expectEq$(response.headers.get("Server"s), "Apache"s);
    expectEq$(response.headers.get("Content-Length"s), "3"s);

    auto body = try$(response.readBody(br));

    expectEq$(body.unwrap(), bytes("foo"s));

    return Ok();
}

test$("read-http-response-body-content-length-mismatch") {
    auto rawResponse =
        "HTTP/1.2 500 Internal Server Error\r\n"
        "Content-Length: 100\r\n"
        "\r\n"
        "foo"s;

    Io::BufReader br{bytes(rawResponse)};

    auto response = try$(Net::Http::Response::read(br));

    expectEq$(response.code, Net::Http::Code{500});

    auto expectedVersion = Net::Http::Version{1u, 2u};
    expectEq$(response.version.major, expectedVersion.major);
    expectEq$(response.version.minor, expectedVersion.minor);

    expectEq$(response.headers.len(), 1u);
    expectEq$(response.headers.get("Content-Length"s), "100"s);

    auto body = response.readBody(br);
    expectNot$(body);

    return Ok();
}

test$("read-http-response-body-empty-body") {
    auto rawResponse =
        "HTTP/1.1 404 Not Found\r\n"
        "\r\n"s;

    Io::BufReader br{bytes(rawResponse)};

    auto response = try$(Net::Http::Response::read(br));

    expectEq$(response.code, Net::Http::Code{404});

    expectEq$(response.headers.len(), 0u);

    auto body = try$(response.readBody(br));
    expectNot$(body);

    return Ok();
}

test$("parse-unparse-http-request-no-header") {
    auto rawRequest =
        "GET / HTTP/1.1\r\n"
        "\r\n"s;

    Io::SScan s{rawRequest};
    auto request = try$(Net::Http::Request::parse(s));

    expectEq$(request.path, ""_path);
    expectEq$(request.method, Net::Http::Method::GET);

    auto expectedVersion = Net::Http::Version{1u, 1u};
    expectEq$(request.version.major, expectedVersion.major);
    expectEq$(request.version.minor, expectedVersion.minor);

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
    auto request = try$(Net::Http::Request::parse(s));

    expectEq$(request.path, ""_path);
    expectEq$(request.method, Net::Http::Method::POST);

    auto expectedVersion = Net::Http::Version{1u, 2u};
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

} // namespace Karm::Print::Tests
