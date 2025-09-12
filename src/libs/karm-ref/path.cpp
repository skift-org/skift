module;

#include <karm-core/macros.h>

export module Karm.Ref:path;

// https://url.spec.whatwg.org/

import Karm.Core;

namespace Karm::Ref {

// MARK: Path ------------------------------------------------------------------

export Str suffixOf(Str str) {
    auto dotIndex = lastIndexOf(str, '.');
    if (not dotIndex.has())
        return "";
    return next(str, *dotIndex + 1);
}

export struct Path {
    static constexpr auto SEP = '/';

    bool rooted = false;
    Vec<String> _parts;

    static Path parse(Io::SScan& s, bool inUrl = false, bool stopAtWhitespace = false) {
        Path path;

        if (s.skip(SEP)) {
            path.rooted = true;
        }

        s.begin();
        while (not s.ended()) {
            if (inUrl and (s.peek() == '?' or s.peek() == '#'))
                break;

            if (stopAtWhitespace and isAsciiSpace(s.peek()))
                break;

            if (s.peek() == SEP) {
                path._parts.pushBack(s.end());
                s.next();
                s.begin();
            } else {
                s.next();
            }
        }

        auto last = s.end();
        if (last.len() > 0)
            path._parts.pushBack(last);

        return path;
    }

    static Path parse(Str str, bool inUrl = false, bool stopAtWhitespace = false) {
        Io::SScan s{str};
        return parse(s, inUrl, stopAtWhitespace);
    }

    void normalize() {
        Vec<String> parts;
        for (auto const& part : _parts) {
            if (part == ".")
                continue;

            if (part == "..") {
                if (parts.len() > 0) {
                    parts.popBack();
                } else if (not rooted) {
                    parts.pushBack(part);
                }
            } else
                parts.pushBack(part);
        }

        _parts = parts;
    }

    Str basename() const {
        if (not _parts.len())
            return {};

        return last(_parts);
    }

    Path join(Path const& other) const {
        if (other.rooted)
            return other;

        Path path = *this;
        path._parts.pushBack(other._parts);
        path.normalize();
        return path;
    }

    Path join(Str other) const  {
        return join(parse(other));
    }

    void append(Str part) {
        _parts.pushBack(part);
    }

    Path parent(usize n = 0) const {
        Path path = *this;
        if (path._parts.len() >= n) {
            for (usize i = 0; i < n; i++)
                path._parts.popBack();
        }
        return path;
    }

    bool isParentOf(Path const& other) const {
        if (len() > other.len())
            return false;

        for (usize i = 0; i < len(); i++) {
            if (_parts[i] != other._parts[i])
                return false;
        }

        return true;
    }


    Res<> unparse(Io::TextWriter& writer) const {
        if (not rooted and len() == 0)
            try$(writer.writeRune('.'));

        if (rooted and len() == 0)
            try$(writer.writeRune(SEP));

        bool first = not rooted;

        for (auto const& part : _parts) {
            if (not first)
                try$(writer.writeRune(SEP));
            try$(writer.writeStr(part.str()));
            first = false;
        }

        return Ok();
    }

    String str() const {
        Io::StringWriter writer;
        unparse(writer).unwrap("unparse error");
        return writer.str();
    }


    auto iter() const {
        return Karm::iter(_parts);
    }

    Str operator[](usize i) const {
        return _parts[i];
    }

    usize len() const {
        return _parts.len();
    }

    auto operator<=>(Path const&) const = default;

    Str suffix() const {
        if (not _parts.len())
            return "";
        auto dotIndex = lastIndexOf(last(_parts), '.');
        if (not dotIndex.has())
            return "";
        return next(last(_parts), *dotIndex + 1);
    }
};

} // namespace Karm::Ref

export auto operator""_path(char const* str, Karm::usize len) {
    return Karm::Ref::Path::parse({str, len});
}

export auto operator/(Karm::Ref::Path const& path, Karm::Ref::Path const& other) {
    return path.join(other);
}

export auto operator/(Karm::Ref::Path const& path, Karm::Str other) {
    return path.join(other);
}

export template <>
struct Karm::Io::Formatter<Karm::Ref::Path> {
    Res<> format(Io::TextWriter& writer, Karm::Ref::Path const& path) {
        return path.unparse(writer);
    }
};
