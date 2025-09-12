module;

#include <karm-core/macros.h>

export module Karm.Core:io.case_;

import :base.res;
import :io.fmt;
import :io.expr;
import :io.text;

namespace Karm::Io {

static auto const RE_SEP =
    Re::single(' ', '\t', '.', '_', '/', '-');

using CaseFn = auto(Rune, usize, usize) -> Rune;

Res<usize> _changeCase(SScan& s, Io::TextWriter& w, CaseFn fn) {
    bool wasLower = false;

    usize si = 0, wi = 0;
    usize written = 0;

    s.eat(RE_SEP);

    while (not s.ended()) {
        if (s.eat(RE_SEP)) {
            auto sep = fn(' ', si, wi);
            if (sep)
                try$(w.writeRune(sep));
            wi = 0;
            wasLower = false;
        } else {
            if (wasLower and isAsciiUpper(s.peek())) {
                auto sep = fn(' ', si, wi);
                if (sep)
                    try$(w.writeRune(sep));
                wi = 0;
            }

            wasLower = isAsciiLower(s.peek()) and isAsciiAlpha(s.peek());
            try$(w.writeRune(fn(s.next(), si, wi)));
            si++;
            wi++;
        }
    }

    return Ok(written);
}

Res<String> _changeCase(Str str, CaseFn fn) {
    Io::StringWriter writer;
    Io::SScan scan{str};
    try$(_changeCase(scan, writer, fn));
    return Ok(writer.take());
}

// si: string index
// wi: word index

Rune _toNoCase(Rune rune, usize, usize) {
    return toAsciiLower(rune);
}

Rune _toPascalCase(Rune rune, usize, usize wi) {
    if (rune == ' ')
        return '\0';

    if (wi == 0)
        return toAsciiUpper(rune);

    return toAsciiLower(rune);
}

Rune _toCamelCase(Rune rune, usize si, usize wi) {
    if (si == 0)
        return toAsciiLower(rune);

    return _toPascalCase(rune, si, wi);
}

Rune _toTitleCase(Rune rune, usize, usize wi) {
    if (wi == 0)
        return toAsciiUpper(rune);
    return toAsciiLower(rune);
}

Rune _toConstantCase(Rune rune, usize, usize) {
    if (rune == ' ')
        return '_';
    return toAsciiUpper(rune);
}

Rune _toDotCase(Rune rune, usize, usize) {
    if (rune == ' ')
        return '.';
    return rune;
}

Rune _toHeaderCase(Rune rune, usize, usize wi) {
    if (rune == ' ')
        return '-';
    if (wi == 0)
        return toAsciiUpper(rune);
    return toAsciiLower(rune);
}

Rune _toParamCase(Rune rune, usize, usize) {
    if (rune == ' ')
        return '-';
    return toAsciiLower(rune);
}

Rune _toPathCase(Rune rune, usize, usize) {
    if (rune == ' ')
        return '/';
    return rune;
}

Rune _toSentenceCase(Rune rune, usize si, usize) {
    if (si == 0)
        return toAsciiUpper(rune);
    return toAsciiLower(rune);
}

Rune _toSnakeCase(Rune rune, usize, usize) {
    if (rune == ' ')
        return '_';
    return toAsciiLower(rune);
}

Rune _toSwapCase(Rune rune, usize, usize) {
    if (isAsciiLower(rune))
        return toAsciiUpper(rune);
    if (isAsciiUpper(rune))
        return toAsciiLower(rune);
    return rune;
}

Rune _toLowerCase(Rune rune, usize, usize) {
    return toAsciiLower(rune);
}

Rune _toLowerFirstCase(Rune rune, usize si, usize) {
    if (si == 0)
        return toAsciiLower(rune);

    return rune;
}

Rune _toUpperCase(Rune rune, usize, usize) {
    return toAsciiUpper(rune);
}

Rune _toUpperFirstCase(Rune rune, usize si, usize) {
    if (si == 0)
        return toAsciiUpper(rune);
    return rune;
}

Rune _toSpongeCase(Rune rune, usize si, usize) {
    if (si % 2 == 0)
        return toAsciiUpper(rune);
    return toAsciiLower(rune);
}

export enum struct Case {
    DEFAULT,
    CAMEL,
    CAPITAL,
    CONSTANT,
    DOT,
    HEADER,
    NO,
    PARAM,
    PASCAL,
    PATH,
    SENTENCE,
    SNAKE,
    TITLE,
    SWAP,
    LOWER,
    LOWER_FIRST,
    UPPER,
    UPPER_FIRST,
    SPONGE,
};

export Res<String> toDefaultCase(Str str) {
    return Ok(str);
}

export Res<String> toCamelCase(Str str) {
    return _changeCase(str, _toCamelCase);
}

export Res<String> toCapitalCase(Str str) {
    return _changeCase(str, _toTitleCase);
}

export Res<String> toConstantCase(Str str) {
    return _changeCase(str, _toConstantCase);
}

export Res<String> toDotCase(Str str) {
    return _changeCase(str, _toDotCase);
}

export Res<String> toHeaderCase(Str str) {
    return _changeCase(str, _toHeaderCase);
}

export Res<String> toNoCase(Str str) {
    return _changeCase(str, _toNoCase);
}

export Res<String> toParamCase(Str str) {
    return _changeCase(str, _toParamCase);
}

export Res<String> toPascalCase(Str str) {
    return _changeCase(str, _toPascalCase);
}

export Res<String> toPathCase(Str str) {
    return _changeCase(str, _toPathCase);
}

export Res<String> toSentenceCase(Str str) {
    return _changeCase(str, _toSentenceCase);
}

export Res<String> toSnakeCase(Str str) {
    return _changeCase(str, _toSnakeCase);
}

export Res<String> toTitleCase(Str str) {
    return _changeCase(str, _toTitleCase);
}

export Res<String> toSwapCase(Str str) {
    return _changeCase(str, _toSwapCase);
}

export Res<String> toLowerCase(Str str) {
    return _changeCase(str, _toLowerCase);
}

export Res<String> toLowerFirstCase(Str str) {
    return _changeCase(str, _toLowerFirstCase);
}

export Res<String> toUpperCase(Str str) {
    return _changeCase(str, _toUpperCase);
}

export Res<String> toUpperFirstCase(Str str) {
    return _changeCase(str, _toUpperFirstCase);
}

export Res<String> toSpongeCase(Str str) {
    return _changeCase(str, _toSpongeCase);
}

export Res<String> changeCase(Str str, Case toCase) {
    switch (toCase) {
    case Case::CAMEL:
        return toCamelCase(str);

    case Case::CAPITAL:
        return toCapitalCase(str);

    case Case::CONSTANT:
        return toConstantCase(str);

    case Case::DOT:
        return toDotCase(str);

    case Case::HEADER:
        return toHeaderCase(str);

    case Case::NO:
        return toNoCase(str);

    case Case::PARAM:
        return toParamCase(str);

    case Case::PASCAL:
        return toPascalCase(str);

    case Case::PATH:
        return toPathCase(str);

    case Case::SENTENCE:
        return toSentenceCase(str);

    case Case::SNAKE:
        return toSnakeCase(str);

    case Case::TITLE:
        return toTitleCase(str);

    case Case::SWAP:
        return toSwapCase(str);

    case Case::LOWER:
        return toLowerCase(str);

    case Case::LOWER_FIRST:
        return toLowerFirstCase(str);

    case Case::UPPER:
        return toUpperCase(str);

    case Case::UPPER_FIRST:
        return toUpperFirstCase(str);

    case Case::SPONGE:
        return toSpongeCase(str);

    default:
        return toDefaultCase(str);
    }
}

template <typename T>
struct Cased {
    T _inner;
    Case _case;
};

export auto cased(auto inner, Case cased) {
    return Cased<decltype(inner)>{inner, cased};
}

export template <typename T>
struct Formatter<Cased<T>> {
    Formatter<T> _innerFmt{};

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& writer, Cased<T> val) {
        Io::StringWriter sw;
        try$(_innerFmt.format(sw, val._inner));
        String result = try$(changeCase(sw.str(), val._case));
        try$(writer.writeStr(result.str()));
        return Ok();
    }
};

} // namespace Karm::Io
