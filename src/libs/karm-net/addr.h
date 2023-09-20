#pragma once

#include <karm-base/res.h>
#include <karm-base/var.h>
#include <karm-fmt/fmt.h>
#include <karm-io/sscan.h>

namespace Karm::Net {

union Ip4 {
    struct {
        u8 a, b, c, d;
    };

    Array<u8, 4> bytes;

    Ip4(u8 a, u8 b, u8 c, u8 d)
        : a(a), b(b), c(c), d(d) {}

    Ip4(Array<u8, 4> bytes)
        : bytes(bytes) {}

    static auto unspecified() -> Ip4 {
        return {0, 0, 0, 0};
    }

    static auto localhost() {
        return Ip4{{127, 0, 0, 1}};
    }

    static Res<Ip4> parse(Io::SScan &s) {
        auto addr = unspecified();

        for (auto i = 0; i < 4; ++i) {
            auto n = s.nextUint(10);
            if (not n) {
                return Error::invalidInput("invalid ip address");
            }

            if (n.unwrap() > 255) {
                return Error::invalidInput("invalid IP address");
            }

            addr.bytes[i] = n.unwrap();

            if (i < 3) {
                if (not s.skip('.')) {
                    return Error::invalidInput("invalid IP address");
                }
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

    Array<u16, 8> words;

    Ip6(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h)
        : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) {}

    Ip6(Array<u16, 8> words)
        : words(words) {}

    static auto unspecified() {
        return Ip6{0, 0, 0, 0, 0, 0, 0, 0};
    }

    static auto localhost() {
        return Ip6{0, 0, 0, 0, 0, 0, 0, 1};
    }

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
        auto maybeV6 = Ip6::parse(s);

        if (maybeV6) {
            return Ok(Ip(maybeV6.unwrap()));
        }

        auto maybeV4 = Ip4::parse(s);

        if (maybeV4) {
            return Ok(Ip(maybeV4.unwrap()));
        }

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

} // namespace Karm::Net

template <>
struct Karm::Fmt::Formatter<Net::Ip4> {
    Res<usize> format(Io::TextWriter &writer, Net::Ip4 addr) {
        return Fmt::format(writer, "{}.{}.{}.{}", addr.a, addr.b, addr.c, addr.d);
    }
};

template <>
struct Karm::Fmt::Formatter<Net::Ip6> {
    Res<usize> format(Io::TextWriter &writer, Net::Ip6 addr) {
        return Fmt::format(writer, "{}:{}:{}:{}:{}:{}:{}:{}", addr.a, addr.b, addr.c, addr.d, addr.e, addr.f, addr.g, addr.h);
    }
};

template <>
struct Karm::Fmt::Formatter<Net::Ip> {
    Res<usize> format(Io::TextWriter &writer, Net::Ip addr) {
        return addr.visit([&](auto addr) {
            return Fmt::format(writer, "{}", addr);
        });
    }
};

template <>
struct Karm::Fmt::Formatter<Net::SocketAddr> {
    Res<usize> format(Io::TextWriter &writer, Net::SocketAddr addr) {
        return Fmt::format(writer, "{}:{}", addr.addr, addr.port);
    }
};
