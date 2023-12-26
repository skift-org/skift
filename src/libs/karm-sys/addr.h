#pragma once

#include <karm-base/array.h>
#include <karm-base/endian.h>
#include <karm-fmt/fmt.h>

namespace Karm::Sys {

struct SocketAddr;

union Ip4 {
    struct {
        u8 a, b, c, d;
    };

    Array<u8, 4> bytes;

    Be<u32> _raw;

    Ip4(u8 a, u8 b, u8 c, u8 d)
        : a(a), b(b), c(c), d(d) {}

    Ip4(Array<u8, 4> bytes)
        : bytes(bytes) {}

    static auto unspecified() -> Ip4 {
        return {0, 0, 0, 0};
    }

    static SocketAddr unspecified(u16 port);

    static auto localhost() {
        return Ip4{{127, 0, 0, 1}};
    }

    static SocketAddr localhost(u16 port);

    static Res<Ip4> parse(Io::SScan &s) {
        auto addr = unspecified();

        for (auto i = 0; i < 4; ++i) {
            auto n = s.nextUint(10);
            if (not n)
                return Error::invalidInput("invalid ip address");

            if (n.unwrap() > 255)
                return Error::invalidInput("invalid ip address");

            addr.bytes[i] = n.unwrap();

            if (i < 3) {
                if (not s.skip('.'))
                    return Error::invalidInput("invalid ip address");
            }
        }

        return Ok(addr);
    }

    static Res<Ip4> parse(Str str) {
        auto s = Io::SScan(str);
        return parse(s);
    }
};

union Ip6 {
    struct {
        u16 a, b, c, d, e, f, g, h;
    };

    Be<u64> _raw;

    Array<u16, 8> words;

    Ip6(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h)
        : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) {}

    Ip6(Array<u16, 8> words)
        : words(words) {}

    static auto unspecified() {
        return Ip6{0, 0, 0, 0, 0, 0, 0, 0};
    }

    static SocketAddr unspecified(u16 port);

    static auto localhost() {
        return Ip6{0, 0, 0, 0, 0, 0, 0, 1};
    }

    static SocketAddr localhost(u16 port);

    static Res<Ip6> parse(Io::SScan &s) {
        auto addr = unspecified();

        for (auto i = 0; i < 8; i++) {
            addr.words[i] = try$(s.nextUint(16));

            if (i < 7) {
                if (not s.skip(':')) {
                    return Error::invalidInput("invalid IP address");
                }
            }
        }

        return Ok(addr);
    }

    static Res<Ip6> parse(Str str) {
        auto s = Io::SScan(str);
        return parse(s);
    }
};

struct Ip : public Var<Ip4, Ip6> {
    using Var<Ip4, Ip6>::Var;

    static Res<Ip> parse(Io::SScan &s) {
        auto saved = s;
        auto maybeV6 = Ip6::parse(s);

        if (maybeV6)
            return Ok(maybeV6.unwrap());

        s = saved;
        auto maybeV4 = Ip4::parse(s);

        if (maybeV4)
            return Ok(maybeV4.unwrap());

        return Error::invalidInput("invalid IP address");
    }

    static Res<Ip> parse(Str str) {
        auto s = Io::SScan(str);
        return parse(s);
    }
};

struct SocketAddr {
    Ip addr;
    u16 port;

    SocketAddr(Ip addr, u16 port)
        : addr(addr), port(port) {}

    static Res<SocketAddr> parse(Io::SScan &s) {
        auto addr = try$(Ip::parse(s));

        if (not s.skip(':')) {
            return Error::invalidInput("invalid socket address");
        }

        auto port = try$(s.nextUint(10));

        if (port > 65535) {
            return Error::invalidInput("invalid socket address");
        }

        return Ok(SocketAddr(addr, port));
    }
};

inline SocketAddr Ip4::unspecified(u16 port) {
    return SocketAddr{Ip4::unspecified(), port};
}

inline SocketAddr Ip4::localhost(u16 port) {
    return SocketAddr{Ip4::localhost(), port};
}

inline SocketAddr Ip6::unspecified(u16 port) {
    return SocketAddr{Ip6::unspecified(), port};
}

inline SocketAddr Ip6::localhost(u16 port) {
    return SocketAddr{Ip6::localhost(), port};
}

} // namespace Karm::Sys

template <>
struct Karm::Fmt::Formatter<Karm::Sys::Ip4> {
    Res<usize> format(Io::TextWriter &writer, Karm::Sys::Ip4 addr) {
        return Fmt::format(writer, "{}.{}.{}.{}", addr.a, addr.b, addr.c, addr.d);
    }
};

template <>
struct Karm::Fmt::Formatter<Karm::Sys::Ip6> {
    Res<usize> format(Io::TextWriter &writer, Karm::Sys::Ip6 addr) {
        return Fmt::format(writer, "{}:{}:{}:{}:{}:{}:{}:{}", addr.a, addr.b, addr.c, addr.d, addr.e, addr.f, addr.g, addr.h);
    }
};

template <>
struct Karm::Fmt::Formatter<Karm::Sys::Ip> {
    Res<usize> format(Io::TextWriter &writer, Karm::Sys::Ip addr) {
        return addr.visit([&](auto addr) {
            return Fmt::format(writer, "{}", addr);
        });
    }
};

template <>
struct Karm::Fmt::Formatter<Karm::Sys::SocketAddr> {
    Res<usize> format(Io::TextWriter &writer, Karm::Sys::SocketAddr addr) {
        return Fmt::format(writer, "{}:{}", addr.addr, addr.port);
    }
};
