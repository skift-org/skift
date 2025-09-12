module;

#include <karm-core/macros.h>

export module Karm.Ref:url;

// https://url.spec.whatwg.org/
import Karm.Core;
import Karm.Crypto;

import :mime;
import :path;

namespace Karm::Ref {

auto const RE_COMPONENT =
    Re::alpha() &
    Re::zeroOrMore(
        Re::alnum() | '+'_re | '-'_re | '.'_re
    );

auto const RE_SCHEME = RE_COMPONENT & ':'_re;

export struct Blob {
    Mime type = "application/octet-stream"_mime;
    Vec<u8> data{};

    usize len() { return data.len(); }

    bool operator==(Blob const&) const = default;
};

export struct Url {
    Symbol scheme = ""_sym;
    String userInfo;
    Symbol host = ""_sym;
    Opt<usize> port;
    Path path;
    String query;
    String fragment;
    Res<Rc<Blob>> blob = NONE;

    static Res<Rc<Blob>> _parseData(Io::SScan& s) {
        s.begin();
        while (not s.ahead(";base64,") and
               not s.ahead(",") and
               not s.ended()) {
            s.next();
        }

        Mime mime = s.end();
        bool base64 = s.skip(";base64");
        if (not s.skip(','))
            return Ok(makeRc<Blob>());

        Io::BufferWriter buf;
        if (base64) {
            try$(Crypto::base64Decode(s, buf));
        } else {
            Io::TextEncoder enc{buf};
            try$(enc.writeStr(s.remStr()));
        }
        return Ok(makeRc<Blob>(mime, buf.take()));
    }

    static Url parse(Io::SScan& s, Opt<Url> baseUrl = NONE) {
        Url url;

        if (s.ahead(RE_SCHEME)) {
            url.scheme = Symbol::from(s.token(RE_COMPONENT));
            s.skip(':');
        }

        if (url.scheme == "data") {
            url.blob = _parseData(s);
            return url;
        }

        if (s.skip("//")) {
            auto maybeHost = s.token(RE_COMPONENT);

            if (s.skip('@')) {
                url.userInfo = maybeHost;
                maybeHost = s.token(RE_COMPONENT);
            }

            url.host = Symbol::from(maybeHost);

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

    static Url parse(Str str, Opt<Url> baseUrl = NONE) {
        Io::SScan s{str};
        return parse(s, baseUrl);
    }

    static bool isUrl(Str str) {
        Io::SScan s{str};

        return s.skip(RE_COMPONENT) and
               s.skip(':');
    }

    bool rooted() const {
        return path.rooted;
    }

    Url join(Path const& other) const {
        Url url = *this;
        url.path = url.path.join(other);
        return url;
    }

    Url join(Str other) const {
        return join(Path::parse(other));
    }

    Str basename() const {
        return path.basename();
    }

    void append(Str part) {
        path.append(part);
    }

    Url parent(usize n = 0) const {
        Url url = *this;
        url.path = url.path.parent(n);
        return url;
    }

    bool isParentOf(Url const& other) const {
        bool same = scheme == other.scheme and
                    host == other.host and
                    port == other.port;

        return same and path.isParentOf(other.path);
    }

    Res<> unparse(Io::TextWriter& writer) const {
        if (scheme)
            try$(Io::format(writer, "{}:", scheme));

        if (userInfo or host)
            try$(writer.writeStr("//"s));

        if (userInfo)
            try$(Io::format(writer, "{}@", userInfo));

        if (host)
            try$(writer.writeStr(host.str()));

        if (port)
            try$(Io::format(writer, ":{}", port.unwrap()));

        try$(path.unparse(writer));

        if (query)
            try$(Io::format(writer, "?{}", query));

        if (fragment)
            try$(Io::format(writer, "#{}", fragment));

        return Ok();
    }

    String str() const {
        Io::StringWriter writer;
        unparse(writer).unwrap("unparse error");
        return writer.str();
    }

    auto iter() const {
        return path.iter();
    }

    auto len() const {
        return path.len();
    }

    bool operator==(Url const&) const = default;

    bool isRelative() const {
        return not scheme;
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
    static Res<Url> resolveReference(Url const& baseUrl, Url const& referenceUrl, bool strict = true) {
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
};

export Url parseUrlOrPath(Str str, Opt<Url> baseUrl = NONE) {
    if (Url::isUrl(str)) {
        return Url::parse(str, baseUrl);
    }

    Url url = baseUrl.unwrapOr(Url::parse(""));
    url.path = url.path.join(Path::parse(str));

    return url;
}

} // namespace Karm::Ref

export Karm::Ref::Url operator""_url(char const* str, Karm::usize len) {
    return Karm::Ref::Url::parse({str, len});
}

export Karm::Ref::Url operator/(Karm::Ref::Url const& url, Karm::Str path) {
    return url.join(path);
}

export Karm::Ref::Url operator/(Karm::Ref::Url const& url, Karm::Ref::Path const& path) {
    return url.join(path);
}

export template <>
struct Karm::Io::Formatter<Karm::Ref::Url> {
    Karm::Res<> format(Karm::Io::TextWriter& writer, Karm::Ref::Url const& url) {
        return url.unparse(writer);
    }
};
