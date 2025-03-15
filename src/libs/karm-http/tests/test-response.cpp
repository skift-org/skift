#include <karm-test/macros.h>

import Karm.Http;

namespace Karm::Http::Tests {

test$("read-http-response-good-body") {
    auto rawResponse =
        "HTTP/1.1 200 OK\r\n"
        "Server: Apache\r\n"
        "Content-Length: 3\r\n"
        "\r\n"
        "foo"s;

    Io::BufReader br{bytes(rawResponse)};

    auto response = try$(Response::read(br));

    expectEq$(response.code, Code{200});

    auto expectedVersion = Version{1u, 1u};
    expectEq$(response.version, expectedVersion);

    expectEq$(response.header.len(), 2u);
    expectEq$(response.header.get("Server"s), "Apache"s);
    expectEq$(response.header.get("Content-Length"s), "3"s);

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

    auto response = try$(Response::read(br));

    expectEq$(response.code, Code{500});

    auto expectedVersion = Version{1u, 2u};
    expectEq$(response.version, expectedVersion);

    expectEq$(response.header.len(), 1u);
    expectEq$(response.header.get("Content-Length"s), "100"s);

    auto body = response.readBody(br);
    expectNot$(body);

    return Ok();
}

test$("read-http-response-body-empty-body") {
    auto rawResponse =
        "HTTP/1.1 404 Not Found\r\n"
        "\r\n"s;

    Io::BufReader br{bytes(rawResponse)};

    auto response = try$(Response::read(br));

    expectEq$(response.code, Http::NOT_FOUND);

    expectEq$(response.header.len(), 0u);

    auto body = try$(response.readBody(br));
    expectNot$(body);

    return Ok();
}

} // namespace Karm::Http::Tests
