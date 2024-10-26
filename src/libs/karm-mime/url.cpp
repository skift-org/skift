#include <karm-base/ctype.h>

#include "url.h"

namespace Karm::Mime {

static auto const RE_COMPONENT =
    Re::alpha() &
    Re::zeroOrMore(
        Re::alnum() | '+'_re | '-'_re | '.'_re
    );

Url Url::parse(Io::SScan &s) {
    Url url;

    url.scheme = s.token(RE_COMPONENT);
    s.skip(':');

    if (s.skip("//")) {
        auto maybeHost = s.token(RE_COMPONENT);

        if (s.skip('@')) {
            url.authority = maybeHost;
            maybeHost = s.token(RE_COMPONENT);
        }

        url.host = maybeHost;

        if (s.skip(':')) {
            url.port = atou(s);
        }
    }

    url.path = Path::parse(s, true);

    if (s.skip('?'))
        url.query = s.token(Re::until('#'_re));

    if (s.skip('#'))
        url.fragment = s.token(Re::until(Re::eof()));

    return url;
}

Url Url::parse(Str str) {
    Io::SScan s{str};
    return parse(s);
}

bool Url::isUrl(Str str) {
    Io::SScan s{str};

    return s.skip(RE_COMPONENT) and
           s.skip(':');
}

Url Url::join(Path const &other) const {
    Url url = *this;
    url.path = url.path.join(other);
    return url;
}

Url Url::join(Str other) const {
    return join(Path::parse(other));
}

Str Url::basename() const {
    return path.basename();
}

Url Url::parent(usize n) const {
    Url url = *this;
    url.path = url.path.parent(n);
    return url;
}

bool Url::isParentOf(Url const &other) const {
    bool same = scheme == other.scheme and
                host == other.host and
                port == other.port;

    return same and path.isParentOf(other.path);
}

Res<usize> Url::write(Io::TextWriter &writer) const {
    usize written = 0;

    if (scheme.len() > 0)
        written += try$(Io::format(writer, "{}:", scheme));

    if (authority.len() > 0 or host.len() > 0)
        written += try$(writer.writeStr("//"s));

    if (authority.len() > 0)
        written += try$(Io::format(writer, "{}@", authority));

    if (host.len() > 0)
        written += try$(writer.writeStr(host.str()));

    if (port)
        written += try$(Io::format(writer, ":{}", port.unwrap()));

    written += try$(path.write(writer));

    if (query.len() > 0)
        written += try$(Io::format(writer, "?{}", query));

    if (fragment.len() > 0)
        written += try$(Io::format(writer, "#{}", fragment));

    return Ok(written);
}

String Url::str() const {
    Io::StringWriter writer;
    write(writer).unwrap();
    return writer.str();
}

Res<Url> parseUrlOrPath(Str str) {
    if (Url::isUrl(str)) {
        return Ok(Url::parse(str));
    }

    Url url;
    url.scheme = "file"s;
    url.path = Path::parse(str);
    return Ok(url);
}

} // namespace Karm::Mime
