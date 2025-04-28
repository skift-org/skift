module;

#include <karm-base/rc.h>
#include <karm-io/aton.h>
#include <karm-io/expr.h>
#include <karm-io/funcs.h>

export module Karm.Http:response;

import :body;
import :code;
import :header;

namespace Karm::Http {

export struct Response {
    Version version;
    Code code = Code::OK;
    Header header;
    Opt<Rc<Body>> body;

    struct Writer : Io::Writer {
        virtual Header& header() = 0;
        virtual Res<> writeHeader(Code code) = 0;
    };

    static Res<Response> parse(Io::SScan& s) {
        Response res;

        res.version = try$(Version::parse(s));

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        res.code = try$(parseCode(s));

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        s.skip(Re::untilAndConsume("\r\n"_re));

        try$(res.header.parse(s));

        return Ok(res);
    }

    static Res<Response> read(Io::Reader& r) {
        Io::BufferWriter bw;
        while (true) {
            auto [read, reachedDelim] = try$(Io::readLine(r, bw, "\r\n"_bytes));

            if (not reachedDelim)
                return Error::invalidInput("input stream ended with incomplete http header");

            if (read == 0)
                break;
        }

        Io::SScan scan{bw.bytes().cast<char>()};
        return parse(scan);
    }

    Res<Opt<Buf<Byte>>> readBody(Io::Reader& r) {
        auto contentLengthValue = header.tryGet("Content-Length"s);
        if (not contentLengthValue)
            return Ok(NONE);

        auto contentLength = try$(Io::atou(contentLengthValue.unwrap().str()));

        Io::BufferWriter bodyBytes;
        auto read = try$(Io::copy(r, bodyBytes, contentLength));

        if (read < contentLength)
            return Error::invalidInput("read body length is smaller than Content-Length header value");

        return Ok(bodyBytes.take());
    }
};

} // namespace Karm::Http
