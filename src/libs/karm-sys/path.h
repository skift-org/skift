#pragma once

#include <karm-base/panic.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-fmt/fmt.h>
#include <karm-text/expr.h>
#include <karm-text/scan.h>

namespace Karm::Sys {

struct Path : public Vec<String> {
    static constexpr auto SEP = '/';

    bool _rooted = false;
    Vec<String> _parts;

    static Path parse(Text::Scan &s, bool inUrl = false) {
        Path path;

        if (s.skip(SEP)) {
            path._rooted = true;
        }

        s.begin();
        while (not s.ended() and (inUrl or (s.curr() != '?' and s.curr() != '#'))) {
            if (s.curr() == SEP) {
                path._parts.pushBack(s.end());
                s.begin();
            }
            s.next();
        }

        auto last = s.end();
        if (last.len() > 0) {
            path._parts.pushBack(last);
        }

        return path;
    }

    static Path parse(Str str, bool inUrl = false) {
        Text::Scan s{str};
        return parse(s, inUrl);
    }

    bool rooted() const {
        return _rooted;
    }

    auto iter() const {
        return ::iter(_parts);
    }

    Ordr cmp(Path const &other) const {
        return ::cmp(_rooted, other._rooted) | ::cmp(_parts, other._parts);
    }

    auto len() const {
        return _parts.len();
    }

    void normalize() {
        Vec<String> parts;
        for (auto part : iter()) {
            if (Op::eq(part, Str{"."})) {
                continue;
            }

            if (Op::eq(part, "..")) {
                if (parts.len() > 0) {
                    parts.popBack();
                } else if (not rooted()) {
                    parts.pushBack(part);
                }
            } else {
                parts.pushBack(part);
            }
        }

        _parts = parts;
    }

    Path join(Path const &other) const {
        if (other.rooted()) {
            return other;
        }

        Path path = *this;
        path._parts.pushBack(other._parts);
        path.normalize();
        return path;
    }

    Path join(Str other) const {
        return join(parse(other));
    }

    Path parent() const {
        Path path = *this;
        if (path._parts.len() > 0) {
            path._parts.popBack();
        }
        return path;
    }

    Str basename() const {
        if (not _parts.len())
            return Str{};

        return _parts.peekBack();
    }
};

struct Url {
    String scheme;
    String authority;
    String host;
    Opt<usize> port;
    Path path;
    String query;
    String fragment;

    static Url parse(Text::Scan &s) {
        Url url;

        auto const COMPONENT = Re::chain(
            Re::alpha(),
            Re::zeroOrMore(
                Re::either(
                    Re::alnum(),
                    Re::single('+', '.', '-'))));

        url.scheme = s.token(COMPONENT);
        s.skip(':');

        if (s.skip("//")) {
            auto maybeHost = s.token(COMPONENT);

            if (s.skip('@')) {
                url.authority = maybeHost;
                maybeHost = s.token(COMPONENT);
            }

            url.host = maybeHost;

            if (s.skip(':')) {
                url.port = s.nextUint();
            }
        }

        url.path = Path::parse(s, true);

        if (s.skip('?')) {
            url.query = s.token(Re::until(Re::single('#')));
        }

        if (s.skip('#')) {
            url.fragment = s.token(Re::until(Re::eof()));
        }

        return url;
    }
};

} // namespace Karm::Sys

template <>
struct Karm::Fmt::Formatter<Sys::Path> {
    Res<usize> format(Io::_TextWriter &writer, Sys::Path path) {
        usize written = 0;

        if (path.rooted()) {
            written += try$(writer.writeRune(Karm::Sys::Path::SEP));
        }

        for (auto part : path.iter()) {
            written += try$(writer.writeStr(part));
            written += try$(writer.writeRune(Karm::Sys::Path::SEP));
        }

        return Ok(written);
    }
};

template <>
struct Karm::Fmt::Formatter<Sys::Url> {
    Res<usize> format(Io::_TextWriter &writer, Sys::Url url) {
        usize written = 0;

        if (url.scheme.len() > 0) {
            written += try$(writer.writeStr(url.scheme));
            written += try$(writer.writeStr(":"));
        }

        if (url.authority.len() > 0 or url.host.len() > 0) {
            written += try$(writer.writeStr("//"));
        }

        if (url.authority.len() > 0) {
            written += try$(writer.writeStr(url.authority));
            written += try$(writer.writeRune('@'));
        }

        if (url.host.len() > 0) {
            written += try$(writer.writeStr(url.host));
        }

        if (url.port) {
            written += try$(writer.writeStr(":"));
            written += try$(Fmt::format(writer, "{}", url.port));
        }

        written += try$(Fmt::format(writer, "{}", url.path));

        if (url.query.len() > 0) {
            written += try$(writer.writeStr("?"));
            written += try$(writer.writeStr(url.query));
        }

        if (url.fragment.len() > 0) {
            written += try$(writer.writeStr("#"));
            written += try$(writer.writeStr(url.fragment));
        }

        return Ok(written);
    }
};
