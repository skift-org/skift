#pragma once

#include <karm-io/expr.h>
#include <karm-io/sscan.h>

namespace Karm::Mime {

struct Mime {
    String _buf;

    Mime(Str buf) : _buf(buf) {}

    Str type() {
        Io::SScan s(_buf);

        s.begin();
        s.skip(Re::until(Re::single('/')));
        return s.end();
    }

    Str subtype() {
        Io::SScan s(_buf);

        s.skip(type());
        s.skip('/');

        s.begin();
        s.skip(Re::until(Re::single(';')));
        return s.end();
    }
};

} // namespace Karm::Mime

inline auto operator""_mime(char const *buf, usize len) {
    return Karm::Mime::Mime(Str(buf, len));
}
