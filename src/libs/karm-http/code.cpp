module;

#include <karm-io/aton.h>
#include <karm-io/fmt.h>

export module Karm.Http:code;

namespace Karm::Http {

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

export enum struct Code : u16 {
#define ITER(CODE, NAME) NAME = CODE,
    FOREACH_CODE(ITER)
#undef ITER
};

using enum Code;

export enum struct CodeClass : u8 {
    UNKNOWN = 0,
    INFORMATIONAL = 1,
    SUCCESS = 2,
    REDIRECTION = 3,
    CLIENT_ERROR = 4,
    SERVER_ERROR = 5,
};

export CodeClass codeClass(Code code) {
    u16 value = static_cast<u16>(code);
    if (value < 100 or value > 599)
        return CodeClass::UNKNOWN;
    return static_cast<CodeClass>(value);
}

export Res<Code> parseCode(Io::SScan& s) {
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

export Str toStr(Code code) {
    switch (code) {
#define ITER(CODE, NAME) \
    case Code::NAME:     \
        return #NAME;
        FOREACH_CODE(ITER)
#undef ITER
    }
    return "UNKNOWN";
}

} // namespace Karm::Http

template <>
struct Karm::Io::Formatter<Karm::Http::Code> {
    Res<> format(Io::TextWriter& writer, Karm::Http::Code code) {
        return writer.writeStr(Karm::Http::toStr(code));
    }
};
