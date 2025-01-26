#include <karm-base/ctype.h>

#include "url.h"

namespace Karm::Mime {

// MARK: Path ------------------------------------------------------------------

Path Path::parse(Io::SScan& s, bool inUrl, bool stopAtWhitespace) {
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

Path Path::parse(Str str, bool inUrl, bool stopAtWhitespace) {
    Io::SScan s{str};
    return parse(s, inUrl, stopAtWhitespace);
}

void Path::normalize() {
    Vec<String> parts;
    for (auto part : iter()) {
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

Str Path::basename() const {
    if (not _parts.len())
        return {};

    return last(_parts);
}

Path Path::join(Path const& other) const {
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

bool Path::isParentOf(Path const& other) const {
    if (len() > other.len())
        return false;

    for (usize i = 0; i < len(); i++) {
        if (_parts[i] != other._parts[i])
            return false;
    }

    return true;
}

Res<usize> Path::write(Io::TextWriter& writer) const {
    usize written = 0;

    if (not rooted and len() == 0)
        written += try$(writer.writeRune('.'));

    if (rooted and len() == 0)
        written += try$(writer.writeRune(SEP));

    bool first = not rooted;

    for (auto part : iter()) {
        if (not first)
            try$(writer.writeRune(SEP));
        written += try$(writer.writeStr(part.str()));
        first = false;
    }

    return Ok(written);
}

String Path::str() const {
    Io::StringWriter writer;
    write(writer).unwrap();
    return writer.str();
}

} // namespace Karm::Mime
