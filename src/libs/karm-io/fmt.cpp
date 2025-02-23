#include <karm-io/expr.h>

#ifndef __ck_freestanding__
#    include <karm-math/funcs.h>
#endif

#include "aton.h"
#include "emit.h"
#include "fmt.h"

namespace Karm::Io {

// MARK: Format ----------------------------------------------------------------

Res<> _format(Io::TextWriter& writer, Str format, _Args& args) {
    Io::SScan scan{format};
    usize index = 0;

    while (not scan.ended()) {
        Rune c = scan.next();

        if (c == '{') {
            scan.skip(':');
            scan.begin();
            while (scan.peek() != '}') {
                scan.next();
            }
            scan.next();
            Io::SScan inner{scan.end()};
            try$(args.format(inner, writer, index));
            index++;
        } else if (c == '\n') {
            // normalize newlines
            try$(writer.writeStr(Str{Sys::LINE_ENDING}));
        } else {
            try$(writer.writeRune(c));
        }
    }

    return Ok();
};

// MARK: Change case -----------------------------------------------------------

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

// MARK: Case functions --------------------------------------------------------

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

// MARK: Public API ------------------------------------------------------------

Res<String> toDefaultCase(Str str) {
    return Ok(str);
}

Res<String> toCamelCase(Str str) {
    return _changeCase(str, _toCamelCase);
}

Res<String> toCapitalCase(Str str) {
    return _changeCase(str, _toTitleCase);
}

Res<String> toConstantCase(Str str) {
    return _changeCase(str, _toConstantCase);
}

Res<String> toDotCase(Str str) {
    return _changeCase(str, _toDotCase);
}

Res<String> toHeaderCase(Str str) {
    return _changeCase(str, _toHeaderCase);
}

Res<String> toNoCase(Str str) {
    return _changeCase(str, _toNoCase);
}

Res<String> toParamCase(Str str) {
    return _changeCase(str, _toParamCase);
}

Res<String> toPascalCase(Str str) {
    return _changeCase(str, _toPascalCase);
}

Res<String> toPathCase(Str str) {
    return _changeCase(str, _toPathCase);
}

Res<String> toSentenceCase(Str str) {
    return _changeCase(str, _toSentenceCase);
}

Res<String> toSnakeCase(Str str) {
    return _changeCase(str, _toSnakeCase);
}

Res<String> toTitleCase(Str str) {
    return _changeCase(str, _toTitleCase);
}

Res<String> toSwapCase(Str str) {
    return _changeCase(str, _toSwapCase);
}

Res<String> toLowerCase(Str str) {
    return _changeCase(str, _toLowerCase);
}

Res<String> toLowerFirstCase(Str str) {
    return _changeCase(str, _toLowerFirstCase);
}

Res<String> toUpperCase(Str str) {
    return _changeCase(str, _toUpperCase);
}

Res<String> toUpperFirstCase(Str str) {
    return _changeCase(str, _toUpperFirstCase);
}

Res<String> toSpongeCase(Str str) {
    return _changeCase(str, _toSpongeCase);
}

Res<String> changeCase(Str str, Case toCase) {
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

// MARK: Number formatting -----------------------------------------------------

Str NumberFormatter::formatPrefix() {
    if (base == 16)
        return "0x";

    if (base == 8)
        return "0o";

    if (base == 2)
        return "0b";

    return "";
}

void NumberFormatter::parse(Str str) {
    Io::SScan scan{str};
    parse(scan);
}

void NumberFormatter::parse(Io::SScan& s) {
    if (s.skip('#'))
        prefix = true;

    if (s.skip('0'))
        fillChar = '0';

    width = atoi(s).unwrapOr(0);

    if (s.skip('.')) {
        if (s.skip('0'))
            trailingZeros = true;
        precision = atoi(s).unwrapOrDefault(6);
    }

    if (s.ended())
        return;
    Rune c = s.next();
    switch (c) {
    case 'b':
        base = 2;
        break;

    case 'o':
        base = 8;
        break;

    case 'd':
        base = 10;
        break;

    case 'x':
        base = 16;
        break;

    case 'p':
        prefix = true;
        base = 16;
        fillChar = '0';
        width = sizeof(usize) * 2;
        break;

    case 'c':
        isChar = true;
        break;

    default:
        break;
    }
}

Res<> NumberFormatter::formatUnsigned(Io::TextWriter& writer, usize val) {
    auto digit = [](usize v) {
        if (v < 10)
            return '0' + v;
        return 'a' + (v - 10);
    };

    InlineVec<char, 128> buf;

    do {
        buf.pushBack(digit(val % base));
        val /= base;
    } while (val != 0 and buf.len() < buf.cap());

    while (width > buf.len())
        buf.pushBack(fillChar);

    reverse(mutSub(buf));

    if (prefix)
        try$(writer.writeStr(formatPrefix()));
    try$(writer.writeStr(Str{buf}));

    return Ok();
}

Res<> NumberFormatter::formatSigned(Io::TextWriter& writer, isize val) {
    if (val < 0) {
        try$(writer.writeRune('-'));
        val = -val;
    }
    try$(formatUnsigned(writer, val));
    return Ok();
}

#ifndef __ck_freestanding__
Res<> NumberFormatter::formatFloat(Io::TextWriter& writer, f64 val) {
    NumberFormatter formatter;
    isize ipart = (isize)val;
    try$(formatter.formatSigned(writer, ipart));
    f64 fpart = val - (f64)ipart;
    u64 ifpart = (u64)(fpart * Math::pow(10uz, precision));
    if ((ifpart != 0 or trailingZeros) and precision > 0) {
        try$(writer.writeRune('.'));
        formatter.width = precision;
        formatter.fillChar = '0';
        try$(formatter.formatUnsigned(writer, ifpart));
    }
    return Ok();
}
#endif

Res<> NumberFormatter::formatRune(Io::TextWriter& writer, Rune val) {
    if (not prefix)
        return writer.writeRune(val);

    if (val == '\'')
        return writer.writeStr("\\'"s);

    if (val == '\"')
        return writer.writeStr("\\\""s);

    if (val == '\?')
        return writer.writeStr("\\?"s);

    if (val == '\\')
        return writer.writeStr("\\\\"s);

    if (val == '\a')
        return writer.writeStr("\\a"s);

    if (val == '\b')
        return writer.writeStr("\\b"s);

    if (val == '\f')
        return writer.writeStr("\\f"s);

    if (val == '\n')
        return writer.writeStr("\\n"s);

    if (val == '\r')
        return writer.writeStr("\\r"s);

    if (val == '\t')
        return writer.writeStr("\\t"s);

    if (val == '\v')
        return writer.writeStr("\\v"s);

    if (not isAsciiPrint(val))
        return Io::format(writer, "\\u{x}", val);

    return writer.writeRune(val);
}

} // namespace Karm::Io
