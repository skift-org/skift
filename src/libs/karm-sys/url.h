#pragma once

#include <karm-base/panic.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-fmt/fmt.h>
#include <karm-text/expr.h>
#include <karm-text/scan.h>

namespace Karm::Sys {

/* --- Path ----------------------------------------------------------------- */

struct Path {
    static constexpr auto SEP = '/';

    bool rooted = false;
    Vec<String> _parts;

    static Path parse(Text::Scan &s, bool inUrl = false);

    static Path parse(Str str, bool inUrl = false);

    void normalize();

    Str basename() const;

    Path join(Path const &other) const;

    Path join(Str other) const;

    void append(Str part) {
        _parts.pushBack(part);
    }

    Path parent(usize n = 0) const;

    Res<usize> write(Io::TextWriter &writer) const;

    Res<String> str() const;

    Ordr cmp(Path const &other) const {
        return ::cmp(rooted, other.rooted) |
               ::cmp(_parts, other._parts);
    }

    auto iter() const {
        return ::iter(_parts);
    }

    auto len() const {
        return _parts.len();
    }
};

/* --- Url ------------------------------------------------------------------ */

struct Url {
    String scheme;
    String authority;
    String host;
    Opt<usize> port;
    Path path;
    String query;
    String fragment;

    static Url parse(Text::Scan &s);

    static Url parse(Str str);

    static bool isUrl(Str str);

    bool rooted() const {
        return path.rooted;
    }

    Str basename() const;

    void append(Str part) {
        path.append(part);
    }

    Url parent(usize n = 0) const;

    Res<usize> write(Io::TextWriter &writer) const;

    Res<String> str() const;

    Ordr cmp(Url const &other) const {
        return ::cmp(scheme, other.scheme) |
               ::cmp(authority, other.authority) |
               ::cmp(host, other.host) |
               ::cmp(port, other.port) |
               ::cmp(path, other.path) |
               ::cmp(query, other.query) |
               ::cmp(fragment, other.fragment);
    }

    auto iter() const {
        return path.iter();
    }

    auto len() const {
        return path.len();
    }
};

 Res<Url> parseUrlOrPath(Str str) ;

} // namespace Karm::Sys

inline auto operator""_path(char const *str, usize len) {
    return Karm::Sys::Path::parse({str, len});
}

inline auto operator""_url(char const *str, usize len) {
    return Karm::Sys::Url::parse({str, len});
}

template <>
struct Karm::Fmt::Formatter<Sys::Path> {
    Res<usize> format(Io::TextWriter &writer, Sys::Path path) {
        return path.write(writer);
    }
};

template <>
struct Karm::Fmt::Formatter<Sys::Url> {
    Res<usize> format(Io::TextWriter &writer, Sys::Url url) {
        return url.write(writer);
    }
};
