#pragma once

#include <karm-base/string.h>
#include <karm-fmt/fmt.h>

namespace Karm::Sys {

struct Path {
    static constexpr char SEPARATOR = '/';

    String _str;

    Path() = default;

    Path(char const *str) : _str(str) {}

    Path(Str path) : _str(path) {}

    Path(String path) : _str(path) {}

    Ordr cmp(Path const &other) const {
        return ::cmp(_str, other._str);
    }

    Str str() const {
        return _str;
    }

    bool isAbsolute() const {
        return _str[0] == SEPARATOR;
    }

    bool isRelative() const {
        return not isAbsolute();
    }

    Path parent() const {
        for (size_t i = _str.len(); i > 0; --i) {
            if (_str[i - 1] == SEPARATOR) {
                Str p = sub(_str, 0, i - 1);
                return Path(p);
            }
        }

        return Path();
    }

    Path parent(size_t index) {
        Path p = *this;
        for (size_t i = 0; i < index; ++i) {
            p = p.parent();
        }
        return p;
    }

    bool hasParent() const {
        return parent().str().len() > 0;
    }
};

} // namespace Karm::Sys

template <>
struct Karm::Fmt::Formatter<Sys::Path> {
    Res<size_t> format(Io::_TextWriter &writer, Sys::Path path) {
        return Fmt::format(writer, "{}", path.str());
    }
};
