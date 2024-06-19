#pragma once

// https://url.spec.whatwg.org/

#include <karm-io/expr.h>
#include <karm-io/fmt.h>

#include "path.h"

namespace Karm::Mime {

struct Url {
    String scheme;
    String authority;
    String host;
    Opt<usize> port;
    Path path;
    String query;
    String fragment;

    static Url parse(Io::SScan &s);

    static Url parse(Str str);

    static bool isUrl(Str str);

    bool rooted() const {
        return path.rooted;
    }

    Url join(Path const &other) const;

    Url join(Str other) const;

    Str basename() const;

    void append(Str part) {
        path.append(part);
    }

    Url parent(usize n = 0) const;

    bool isParentOf(Url const &other) const;

    Res<usize> write(Io::TextWriter &writer) const;

    String str() const;

    auto iter() const {
        return path.iter();
    }

    auto len() const {
        return path.len();
    }

    auto operator<=>(Url const &) const = default;
};

Res<Url> parseUrlOrPath(Str str);

} // namespace Karm::Mime

inline auto operator""_url(char const *str, usize len) {
    return Karm::Mime::Url::parse({str, len});
}

inline auto operator/(Karm::Mime::Url const &url, Str path) {
    return url.join(path);
}

inline auto operator/(Karm::Mime::Url const &url, Karm::Mime::Path const &path) {
    return url.join(path);
}

template <>
struct Karm::Io::Formatter<Karm::Mime::Url> {
    Res<usize> format(Io::TextWriter &writer, Karm::Mime::Url const &url) {
        return url.write(writer);
    }
};
