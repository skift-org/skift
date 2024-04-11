#pragma once

#include <karm-io/expr.h>
#include <karm-io/fmt.h>
#include <karm-io/sscan.h>
#include <mdi/spec.h>

namespace Karm::Mime {

struct Mime {
    String _buf;

    Mime(Str buf)
        : _buf(buf) {}

    Str type() const {
        Io::SScan s(_buf);

        s.begin();
        s.skip(Re::until('/'_re));
        return s.end();
    }

    Str subtype() const {
        Io::SScan s(_buf);

        s.skip(type());
        s.skip('/');

        s.begin();
        s.skip(Re::until(';'_re));
        return s.end();
    }

    bool is(Mime const &other) const {
        return type() == other.type() and subtype() == other.subtype();
    }

    Str str() const {
        return _buf;
    }
};

/// Try to sniff the mime type from a file extension.
Opt<Mime> sniffSuffix(Str suffix);

/// Guess an icon for a given mime type.
Mdi::Icon iconFor(Mime const &mime);

} // namespace Karm::Mime

inline auto operator""_mime(char const *buf, usize len) {
    return Karm::Mime::Mime(Str(buf, len));
}

template <>
struct Karm::Io::Formatter<Karm::Mime::Mime> {
    Res<usize> format(Io::TextWriter &writer, Karm::Mime::Mime const &mime) {
        return writer.writeStr(mime.str());
    }
};
