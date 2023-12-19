#pragma once

#include <karm-base/distinct.h>
#include <karm-base/map.h>
#include <karm-base/std.h>
#include <karm-fmt/fmt.h>
#include <url/url.h>

namespace Web::Http {

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

static inline Res<Code> parseCode(Io::SScan &s) {
    auto code = try$(s.nextUint());
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

static inline Res<Method> parseMethod(Io::SScan &s) {
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
};

struct Request {
    Method method;
    Url::Path path;
    Version version;
    Map<Str, Str> headers;

    static Res<Request> parse(Io::SScan &s) {
        Request req;

        req.method = try$(parseMethod(s));

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        req.path = Url::Path::parse(s, true, true);
        req.path.rooted = true;
        req.path.normalize();
        req.path.rooted = false;

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        if (not s.skip("HTTP/"))
            return Error::invalidData("Expected \"HTTP/\"");

        req.version.major = try$(s.nextUint());
        s.skip('.');
        req.version.minor = try$(s.nextUint());

        if (not s.skip("\r\n"))
            return Error::invalidData("Expected \"\\r\\n\"");

        while (not s.ended()) {
            auto key = s.token(Re::until(Re::single(':')));
            s.skip(':');
            auto value = s.token(Re::until(Re::single('\r')));
            req.headers.put(key, value);
            if (s.skip("\r\n\r\n"))
                break;
            s.skip("\r\n");
        }

        return Ok(req);
    }
};

} // namespace Web::Http

template <>
struct Karm::Fmt::Formatter<Web::Http::Code> {
    Res<usize> format(Io::TextWriter &writer, Web::Http::Code code) {
        return writer.writeStr(Web::Http::toStr(code));
    }
};

template <>
struct Karm::Fmt::Formatter<Web::Http::Method> {
    Res<usize> format(Io::TextWriter &writer, Web::Http::Method method) {
        return writer.writeStr(Web::Http::toStr(method));
    }
};

template <>
struct Karm::Fmt::Formatter<Web::Http::Version> {
    Res<usize> format(Io::TextWriter &writer, Web::Http::Version version) {
        return Fmt::format(writer, "HTTP/{}.{}", version.major, version.minor);
    }
};
