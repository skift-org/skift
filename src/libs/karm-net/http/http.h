#pragma once

#include <karm-base/distinct.h>
#include <karm-base/map.h>
#include <karm-io/aton.h>
#include <karm-io/fmt.h>
#include <karm-io/funcs.h>
#include <karm-mime/url.h>

namespace Karm::Net::Http {

#define FOREACH_CODE(CODE)                     \
    CODE(0, UNKNOWN)                           \
    CODE(100, CONTINUE)                        \
    CODE(101, SWITCHING_PROTOCOLS)             \
    CODE(102, PROCESSING)                      \
    CODE(103, EARLY_HINTS)                     \
    CODE(200, OK)                              \
    CODE(201, CREATED)                         \
    CODE(202, ACCEPTED)                        \
    CODE(203, NON_AUTHORITATIVE_INFORMATION)   \
    CODE(204, NO_CONTENT)                      \
    CODE(205, RESET_CONTENT)                   \
    CODE(206, PARTIAL_CONTENT)                 \
    CODE(207, MULTI_STATUS)                    \
    CODE(208, ALREADY_REPORTED)                \
    CODE(226, IM_USED)                         \
    CODE(300, MULTIPLE_CHOICES)                \
    CODE(301, MOVED_PERMANENTLY)               \
    CODE(302, FOUND)                           \
    CODE(303, SEE_OTHER)                       \
    CODE(304, NOT_MODIFIED)                    \
    CODE(305, USE_PROXY)                       \
    CODE(306, UNUSED)                          \
    CODE(307, TEMPORARY_REDIRECT)              \
    CODE(308, PERMANENT_REDIRECT)              \
    CODE(400, BAD_REQUEST)                     \
    CODE(401, UNAUTHORIZED)                    \
    CODE(402, PAYMENT_REQUIRED)                \
    CODE(403, FORBIDDEN)                       \
    CODE(404, NOT_FOUND)                       \
    CODE(405, METHOD_NOT_ALLOWED)              \
    CODE(406, NOT_ACCEPTABLE)                  \
    CODE(407, PROXY_AUTHENTICATION_REQUIRED)   \
    CODE(408, REQUEST_TIMEOUT)                 \
    CODE(409, CONFLICT)                        \
    CODE(410, GONE)                            \
    CODE(411, LENGTH_REQUIRED)                 \
    CODE(412, PRECONDITION_FAILED)             \
    CODE(413, PAYLOAD_TOO_LARGE)               \
    CODE(414, URI_TOO_LONG)                    \
    CODE(415, UNSUPPORTED_MEDIA_TYPE)          \
    CODE(416, RANGE_NOT_SATISFIABLE)           \
    CODE(417, EXPECTATION_FAILED)              \
    CODE(418, IM_A_TEAPOT)                     \
    CODE(421, MISDIRECTED_REQUEST)             \
    CODE(422, UNPROCESSABLE_ENTITY)            \
    CODE(423, LOCKED)                          \
    CODE(424, FAILED_DEPENDENCY)               \
    CODE(425, TOO_EARLY)                       \
    CODE(426, UPGRADE_REQUIRED)                \
    CODE(428, PRECONDITION_REQUIRED)           \
    CODE(429, TOO_MANY_REQUESTS)               \
    CODE(431, REQUEST_HEADER_FIELDS_TOO_LARGE) \
    CODE(451, UNAVAILABLE_FOR_LEGAL_REASONS)   \
    CODE(500, INTERNAL_SERVER_ERROR)           \
    CODE(501, NOT_IMPLEMENTED)                 \
    CODE(502, BAD_GATEWAY)                     \
    CODE(503, SERVICE_UNAVAILABLE)             \
    CODE(504, GATEWAY_TIMEOUT)                 \
    CODE(505, HTTP_VERSION_NOT_SUPPORTED)      \
    CODE(506, VARIANT_ALSO_NEGOTIATES)         \
    CODE(507, INSUFFICIENT_STORAGE)            \
    CODE(508, LOOP_DETECTED)                   \
    CODE(510, NOT_EXTENDED)                    \
    CODE(511, NETWORK_AUTHENTICATION_REQUIRED)

enum struct Code : u16 {
#define ITER(CODE, NAME) NAME = CODE,
    FOREACH_CODE(ITER)
#undef ITER
};

enum struct CodeClass : u8 {
    UNKNOWN = 0,
    INFORMATIONAL = 1,
    SUCCESS = 2,
    REDIRECTION = 3,
    CLIENT_ERROR = 4,
    SERVER_ERROR = 5,
};

static inline CodeClass codeClass(Code code) {
    u16 value = static_cast<u16>(code);
    if (value < 100 or value > 599)
        return CodeClass::UNKNOWN;
    return static_cast<CodeClass>(value);
}

static inline Res<Code> parseCode(Io::SScan& s) {
    auto code = try$(atou(s));
    switch (code) {
#define ITER(CODE, NAME) \
    case CODE:           \
        return Ok(Code::NAME);
        FOREACH_CODE(ITER)
#undef ITER
    default:
        return Error::invalidData("Invalid code");
    }
}

static inline Str toStr(Code code) {
    switch (code) {
#define ITER(CODE, NAME) \
    case Code::NAME:     \
        return #NAME;
        FOREACH_CODE(ITER)
#undef ITER
    }
    return "UNKNOWN";
}

#define FOREACH_METHOD(METHOD) \
    METHOD(GET)                \
    METHOD(HEAD)               \
    METHOD(POST)               \
    METHOD(PUT)                \
    METHOD(DELETE)             \
    METHOD(CONNECT)            \
    METHOD(OPTIONS)            \
    METHOD(TRACE)              \
    METHOD(PATCH)

enum struct Method : u8 {
#define ITER(NAME) NAME,
    FOREACH_METHOD(ITER)
#undef ITER
};

static inline Res<Method> parseMethod(Io::SScan& s) {
#define ITER(NAME)     \
    if (s.skip(#NAME)) \
        return Ok(Method::NAME);
    FOREACH_METHOD(ITER)
#undef ITER
    return Error::invalidData("Expected method");
}

static inline Str toStr(Method method) {
    switch (method) {
#define ITER(NAME)     \
    case Method::NAME: \
        return #NAME;
        FOREACH_METHOD(ITER)
#undef ITER
    }
    return "UNKNOWN";
}

struct Version {
    u8 major;
    u8 minor;

    static Res<Version> parse(Io::SScan& s) {
        if (not s.skip("HTTP/"))
            return Error::invalidData("Expected \"HTTP/\"");
        Version v;
        v.major = try$(atou(s));
        s.skip('.');
        v.minor = try$(atou(s));
        return Ok(v);
    }

    Res<> unparse(Io::TextWriter& w) {
        try$(Io::format(w, "HTTP/{}.{}", major, minor));
        return Ok();
    }
};

struct Header {
    Map<String, String> headers;

    void add(Str const& key, Str value) {
        headers.put(key, std::move(value));
    }

    Res<> _parse(Io::SScan& s) {
        while (not s.ended()) {
            Str key, value;

            auto RE_ENDLINE =
                Re::zeroOrMore(' '_re) & "\r\n"_re;

            auto RE_SEPARATOR =
                Re::separator(':'_re);

            auto RE_KEY_VALUE =
                Re::token(
                    key,
                    Re::until(RE_SEPARATOR)
                ) &
                RE_SEPARATOR &
                Re::token(
                    value,
                    Re::until(RE_ENDLINE)
                ) &
                RE_ENDLINE;

            if (s.skip("\r\n"))
                break;

            if (not s.skip(RE_KEY_VALUE))
                return Error::invalidData("Expected header");

            headers.put(key, value);
        }

        return Ok();
    }

    Res<> _unparse(Io::TextWriter& w) {
        for (auto& [key, value] : headers.iter()) {
            try$(Io::format(w, "{}: {}\r\n", key, value));
        }

        try$(w.writeStr("\r\n"s));

        return Ok();
    }
};

struct Request : public Header {
    Method method;
    Mime::Path path;
    Version version;

    static Res<Request> parse(Io::SScan& s) {
        Request req;

        req.method = try$(parseMethod(s));

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        req.path = Mime::Path::parse(s, true, true);
        req.path.rooted = true;
        req.path.normalize();
        req.path.rooted = false;

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        req.version = try$(Version::parse(s));

        if (not s.skip("\r\n"))
            return Error::invalidData("Expected \"\\r\\n\"");

        try$(req._parse(s));

        return Ok(req);
    }

    Res<> unparse(Io::TextWriter& w) {
        // Start line

        path.rooted = true;
        try$(Io::format(w, "{} {} ", toStr(method), path));
        path.rooted = false;

        try$(version.unparse(w));
        try$(w.writeStr("\r\n"s));

        // Headers and empty line
        try$(_unparse(w));

        return Ok();
    }
};

struct Response : public Header {
    Version version;
    Code code;

    static Res<Response> parse(Io::SScan& s) {
        Response res;

        res.version = try$(Version::parse(s));

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        res.code = try$(parseCode(s));

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        s.skip(Re::untilAndConsume("\r\n"_re));

        try$(res._parse(s));

        return Ok(res);
    }

    static Res<Response> read(Io::Reader& r) {
        Io::BufferWriter bw;
        while (true) {
            auto [read, reachedDelim] = try$(Io::readLine(
                r, bw, bytes("\r\n"s)
            ));

            if (not reachedDelim)
                return Error::invalidInput("input stream ended with incomplete http header");

            if (read == 0)
                break;
        }

        Io::SScan scan{bw.bytes().cast<char>()};
        return parse(scan);
    }

    Res<Opt<Buf<Byte>>> readBody(Io::Reader& r) {
        auto contentLengthValue = headers.tryGet("Content-Length"s);
        if (not contentLengthValue)
            return Ok(NONE);

        auto contentLength = try$(Io::atou(contentLengthValue.unwrap().str()));

        Io::BufferWriter bodyBytes;
        auto read = try$(Io::copy(r, bodyBytes, contentLength));

        if (read != contentLength)
            return Error::invalidInput("read body size is different from Content-Length header value");

        return Ok(bodyBytes.take());
    }
};

} // namespace Karm::Net::Http

template <>
struct Karm::Io::Formatter<Karm::Net::Http::Code> {
    Res<> format(Io::TextWriter& writer, Karm::Net::Http::Code code) {
        return writer.writeStr(Karm::Net::Http::toStr(code));
    }
};

template <>
struct Karm::Io::Formatter<Karm::Net::Http::Method> {
    Res<> format(Io::TextWriter& writer, Karm::Net::Http::Method method) {
        return writer.writeStr(Karm::Net::Http::toStr(method));
    }
};

template <>
struct Karm::Io::Formatter<Karm::Net::Http::Version> {
    Res<> format(Io::TextWriter& writer, Karm::Net::Http::Version version) {
        return Io::format(writer, "HTTP/{}.{}", version.major, version.minor);
    }
};
