#pragma once

#include <karm-base/panic.h>
#include <karm-base/string.h>
#include <karm-fmt/fmt.h>

namespace Karm::Sys {

struct Path {
    static constexpr char SEPARATOR = '/';

    String _str;

    Path() = default;

    Path(char const *str)
        : _str(str) {}

    Path(Str path)
        : _str(path) {}

    Path(String path)
        : _str(path) {}

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

    bool hasParent() {
        for (usize i = _str.len(); i > 0; --i) {
            if (_str[i - 1] == SEPARATOR) {
                return true;
            }
        }

        return false;
    }

    auto iterParts() const {
        return iterSplit(_str, SEPARATOR)
            .filter([](Str s) {
                return s.len() > 0;
            });
    }

    Path up() const {
        for (usize i = _str.len(); i > 0; --i) {
            if (_str[i - 1] == SEPARATOR) {
                Str p = sub(_str, 0, i - 1);
                return Path(p);
            }
        }

        return {"/"};
    }

    Path up(usize index) const {
        Path p = *this;
        for (usize i = 0; i < index; ++i) {
            p = p.up();
        }
        return p;
    }

    Path down(usize index) const {
        Text::Scan scan{_str};
        scan.begin();
        scan.skip('/');

        while (index != 0 && !scan.ended()) {
            while (!scan.skip('/') && !scan.ended()) {
                scan.next();
            }
            index--;
        }

        return scan.end();
    }

    bool hasParent() const {
        return up().str().len() > 0;
    }
};

} // namespace Karm::Sys

template <>
struct Karm::Fmt::Formatter<Sys::Path> {
    Res<usize> format(Io::_TextWriter &writer, Sys::Path path) {
        return Fmt::format(writer, "{}", path.str());
    }
};
