export module Karm.Http:method;

import Karm.Core;

namespace Karm::Http {

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

export enum struct Method : u8 {
#define ITER(NAME) NAME,
    FOREACH_METHOD(ITER)
#undef ITER
};

using enum Method;

export Res<Method> parseMethod(Io::SScan& s) {
#define ITER(NAME)     \
    if (s.skip(#NAME)) \
        return Ok(Method::NAME);
    FOREACH_METHOD(ITER)
#undef ITER
    return Error::invalidData("Expected method");
}

export Str toStr(Method method) {
    switch (method) {
#define ITER(NAME)     \
    case Method::NAME: \
        return #NAME;
        FOREACH_METHOD(ITER)
#undef ITER
    }
    return "UNKNOWN";
}

} // namespace Karm::Http

template <>
struct Karm::Io::Formatter<Karm::Http::Method> {
    Res<> format(Io::TextWriter& writer, Karm::Http::Method method) {
        return writer.writeStr(Karm::Http::toStr(method));
    }
};
