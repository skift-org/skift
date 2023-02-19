#pragma once

#include <karm-base/res.h>
#include <karm-base/var.h>
#include <karm-fmt/fmt.h>
#include <karm-text/scan.h>

namespace Karm::Net {

union IpAddrV4 {
    struct {
        uint8_t a, b, c, d;
    };

    Array<uint8_t, 4> bytes;

    IpAddrV4(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
        : a(a), b(b), c(c), d(d) {}

    IpAddrV4(Array<uint8_t, 4> bytes)
        : bytes(bytes) {}

    static auto unspecified() -> IpAddrV4 {
        return {0, 0, 0, 0};
    }

    static auto localhost() {
        return IpAddrV4{{127, 0, 0, 1}};
    }

    static Res<IpAddrV4> parse(Text::Scan &s) {
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

    static Res<IpAddrV4> parse(Str str) {
        auto s = Text::Scan(str);
        return parse(s);
    }
};

union IpAddrV6 {
    struct {
        uint16_t a, b, c, d, e, f, g, h;
    };

    Array<uint16_t, 8> words;

    IpAddrV6(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t e, uint16_t f, uint16_t g, uint16_t h)
        : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) {}

    IpAddrV6(Array<uint16_t, 8> words)
        : words(words) {}

    static auto unspecified() {
        return IpAddrV6{0, 0, 0, 0, 0, 0, 0, 0};
    }

    static auto localhost() {
        return IpAddrV6{0, 0, 0, 0, 0, 0, 0, 1};
    }

    static Res<IpAddrV6> parse(Text::Scan &s) {
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

    static Res<IpAddrV6> parse(Str str) {
        auto s = Text::Scan(str);
        return parse(s);
    }
};

struct IpAddr : public Var<IpAddrV4, IpAddrV6> {
    using Var<IpAddrV4, IpAddrV6>::Var;

    static Res<IpAddr> parse(Text::Scan &s) {
        auto maybeV6 = IpAddrV6::parse(s);

        if (maybeV6) {
            return Ok(IpAddr(maybeV6.unwrap()));
        }

        auto maybeV4 = IpAddrV4::parse(s);

        if (maybeV4) {
            return Ok(IpAddr(maybeV4.unwrap()));
        }

        return Error::invalidInput("invalid IP address");
    }

    static Res<IpAddr> parse(Str str) {
        auto s = Text::Scan(str);
        return parse(s);
    }
};

struct SocketAddr {
    IpAddr addr;
    uint16_t port;

    SocketAddr(IpAddr addr, uint16_t port)
        : addr(addr), port(port) {}

    static Res<SocketAddr> parse(Text::Scan &s) {
        auto addr = try$(IpAddr::parse(s));

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
struct Karm::Fmt::Formatter<Net::IpAddrV4> {
    Res<size_t> format(Io::_TextWriter &writer, Net::IpAddrV4 addr) {
        return Fmt::format(writer, "{}.{}.{}.{}", addr.a, addr.b, addr.c, addr.d);
    }
};

template <>
struct Karm::Fmt::Formatter<Net::IpAddrV6> {
    Res<size_t> format(Io::_TextWriter &writer, Net::IpAddrV6 addr) {
        return Fmt::format(writer, "{}:{}:{}:{}:{}:{}:{}:{}", addr.a, addr.b, addr.c, addr.d, addr.e, addr.f, addr.g, addr.h);
    }
};

template <>
struct Karm::Fmt::Formatter<Net::IpAddr> {
    Res<size_t> format(Io::_TextWriter &writer, Net::IpAddr addr) {
        return addr.visit([&](auto addr) {
            return Fmt::format(writer, "{}", addr);
        });
    }
};

template <>
struct Karm::Fmt::Formatter<Net::SocketAddr> {
    Res<size_t> format(Io::_TextWriter &writer, Net::SocketAddr addr) {
        return Fmt::format(writer, "{}:{}", addr.addr, addr.port);
    }
};
