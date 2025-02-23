#include <karm-base/ctype.h>
#include <karm-io/aton.h>

#include "url.h"

namespace Karm::Mime {

static auto const RE_COMPONENT =
    Re::alpha() &
    Re::zeroOrMore(
        Re::alnum() | '+'_re | '-'_re | '.'_re
    );

static auto const RE_SCHEME = RE_COMPONENT & ':'_re;

Url Url::parse(Io::SScan& s, Opt<Url> baseUrl) {
    Url url;

    if (s.ahead(RE_SCHEME)) {
        url.scheme = s.token(RE_COMPONENT);
        s.skip(':');
    }

    if (s.skip("//")) {
        auto maybeHost = s.token(RE_COMPONENT);

        if (s.skip('@')) {
            url.userInfo = maybeHost;
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

    if (not baseUrl)
        return url;

    auto resolvedRef = resolveReference(baseUrl.unwrap(), url);
    return resolvedRef.unwrapOr(url);
}

Url Url::parse(Str str, Opt<Url> origin) {
    Io::SScan s{str};
    return parse(s, origin);
}

bool Url::isUrl(Str str) {
    Io::SScan s{str};

    return s.skip(RE_COMPONENT) and
           s.skip(':');
}

static Path _mergePaths(Url const& baseUrl, Url const& referenceUrl) {
    if (baseUrl.host and baseUrl.path.len() == 0) {
        Path path = referenceUrl.path;
        path.rooted = true;
        return path;
    }

    return baseUrl.path.parent(1).join(referenceUrl.path);
}

// https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.2
Res<Url> Url::resolveReference(Url const& baseUrl, Url const& referenceUrl, bool strict) {
    if (baseUrl.isRelative())
        return Error::invalidInput("base url must not be a relative url");

    bool undefineReferenceSchema = not strict and referenceUrl.scheme == baseUrl.scheme;
    if (referenceUrl.scheme and not undefineReferenceSchema) {
        Url targetUrl = referenceUrl;
        targetUrl.path.normalize();
        return Ok(targetUrl);
    }

    if (referenceUrl.host) {
        Url targetUrl = referenceUrl;
        targetUrl.scheme = baseUrl.scheme;
        targetUrl.path.normalize();
        return Ok(targetUrl);
    }

    Url targetUrl = baseUrl;
    if (referenceUrl.path.len() == 0) {
        if (referenceUrl.query)
            targetUrl.query = referenceUrl.query;
    } else {
        if (referenceUrl.path.rooted)
            targetUrl.path = referenceUrl.path;
        else
            targetUrl.path = _mergePaths(baseUrl, referenceUrl);
        targetUrl.query = referenceUrl.query;
    }

    targetUrl.fragment = referenceUrl.fragment;
    targetUrl.path.normalize();

    return Ok(targetUrl);
}

Url Url::join(Path const& other) const {
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

bool Url::isParentOf(Url const& other) const {
    bool same = scheme == other.scheme and
                host == other.host and
                port == other.port;

    return same and path.isParentOf(other.path);
}

Res<> Url::unparse(Io::TextWriter& writer) const {
    if (scheme.len() > 0)
        try$(Io::format(writer, "{}:", scheme));

    if (userInfo.len() > 0 or host.len() > 0)
        try$(writer.writeStr("//"s));

    if (userInfo.len() > 0)
        try$(Io::format(writer, "{}@", userInfo));

    if (host.len() > 0)
        try$(writer.writeStr(host.str()));

    if (port)
        try$(Io::format(writer, ":{}", port.unwrap()));

    try$(path.unparse(writer));

    if (query.len() > 0)
        try$(Io::format(writer, "?{}", query));

    if (fragment.len() > 0)
        try$(Io::format(writer, "#{}", fragment));

    return Ok();
}

String Url::str() const {
    Io::StringWriter writer;
    unparse(writer).unwrap("unparse error");
    return writer.str();
}

Url parseUrlOrPath(Str str, Opt<Url> baseUrl) {
    if (Url::isUrl(str)) {
        return Url::parse(str, baseUrl);
    }

    Url url = baseUrl.unwrapOr(""_url);
    url.path = url.path.join(Path::parse(str));

    return url;
}

} // namespace Karm::Mime
