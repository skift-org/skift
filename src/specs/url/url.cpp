#include "url.h"

namespace Url {

/* --- Path ----------------------------------------------------------------- */

Path Path::parse(Io::SScan &s, bool inUrl) {
    Path path;

    if (s.skip(SEP)) {
        path.rooted = true;
    }

    s.begin();
    while (not s.ended() and (inUrl or (s.curr() != '?' and s.curr() != '#'))) {
        if (s.curr() == SEP) {
            path._parts.pushBack(s.end());
            s.next();
            s.begin();
        } else {
            s.next();
        }
    }

    auto last = s.end();
    if (last.len() > 0) {
        path._parts.pushBack(last);
    }

    return path;
}

Path Path::parse(Str str, bool inUrl) {
    Io::SScan s{str};
    return parse(s, inUrl);
}

void Path::normalize() {
    Vec<String> parts;
    for (auto part : iter()) {
        if (part == ".") {
            continue;
        }

        if (part == "..") {
            if (parts.len() > 0) {
                parts.popBack();
            } else if (not rooted) {
                parts.pushBack(part);
            }
        } else {
            parts.pushBack(part);
        }
    }

    _parts = parts;
}

Str Path::basename() const {
    if (not _parts.len())
        return Str{};

    return _parts.peekBack();
}

Path Path::join(Path const &other) const {
    if (other.rooted) {
        return other;
    }

    Path path = *this;
    path._parts.pushBack(other._parts);
    path.normalize();
    return path;
}

Path Path::join(Str other) const {
    return join(parse(other));
}

Path Path::parent(usize n) const {
    Path path = *this;
    if (path._parts.len() >= n) {
        for (usize i = 0; i < n; i++)
            path._parts.popBack();
    }
    return path;
}

Res<usize> Path::write(Io::TextWriter &writer) const {
    usize written = 0;

    if (not rooted)
        written += try$(writer.writeRune('.'));

    if (rooted and len() == 0)
        written += try$(writer.writeRune(SEP));

    for (auto part : iter())
        written += try$(Fmt::format(writer, "{c}{}", SEP, part));

    return Ok(written);
}

Res<String> Path::str() const {
    Io::StringWriter writer;
    try$(write(writer));
    return Ok(writer.str());
}

/* --- Url ------------------------------------------------------------------ */

static auto const COMPONENT = Re::chain(
    Re::alpha(),
    Re::zeroOrMore(
        Re::either(
            Re::alnum(),
            Re::single('+', '.', '-'))));

Url Url::parse(Io::SScan &s) {
    Url url;

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

Url Url::parse(Str str) {
    Io::SScan s{str};
    return parse(s);
}

bool Url::isUrl(Str str) {
    Io::SScan s{str};

    return s.skip(COMPONENT) and
           s.skip(':');
}

Str Url::basename() const {
    return path.basename();
}

Url Url::parent(usize n) const {
    Url url = *this;
    url.path = url.path.parent(n);
    return url;
}

Res<usize> Url::write(Io::TextWriter &writer) const {
    usize written = 0;

    if (scheme.len() > 0)
        written += try$(Fmt::format(writer, "{}:", scheme));

    if (authority.len() > 0 or host.len() > 0)
        written += try$(writer.writeStr("//"));

    if (authority.len() > 0)
        written += try$(Fmt::format(writer, "{}@", authority));

    if (host.len() > 0)
        written += try$(writer.writeStr(host));

    if (port)
        written += try$(Fmt::format(writer, ":{}", port.unwrap()));

    written += try$(path.write(writer));

    if (query.len() > 0)
        written += try$(Fmt::format(writer, "?{}", query));

    if (fragment.len() > 0)
        written += try$(Fmt::format(writer, "#{}", fragment));

    return Ok(written);
}

Res<String> Url::str() const {
    Io::StringWriter writer;
    try$(write(writer));
    return Ok(writer.str());
}

Res<Url> parseUrlOrPath(Str str) {
    if (Url::isUrl(str)) {
        return Ok(Url::parse(str));
    }

    Url url;
    url.scheme = "file";
    url.path = Path::parse(str);
    return Ok(url);
}

} // namespace Url
