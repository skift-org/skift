export module Karm.Core:base.ctype;

import :base.rune;

namespace Karm {

export template <typename T>
concept CType = requires(T c, Rune r) {
    { c(r) } -> Meta::Same<bool>;
};

// MARK: Check Identity --------------------------------------------------------

export constexpr bool isAscii(Rune rune) {
    return rune <= 0x7F;
}

export constexpr bool isAsciiDigit(Rune rune) {
    return rune >= '0' and rune <= '9';
}

export constexpr bool isAsciiLower(Rune rune) {
    return rune >= 'a' and rune <= 'z';
}

export constexpr bool isAsciiUpper(Rune rune) {
    return rune >= 'A' and rune <= 'Z';
}

export constexpr bool isAsciiAlpha(Rune rune) {
    return isAsciiLower(rune) or isAsciiUpper(rune);
}

export constexpr bool isAsciiAlphaNum(Rune rune) {
    return isAsciiAlpha(rune) or isAsciiDigit(rune);
}

export constexpr bool isAsciiBinDigit(Rune rune) {
    return rune == '0' or rune == '1';
}

export constexpr bool isAsciiOctDigit(Rune rune) {
    return rune >= '0' and rune <= '7';
}

export constexpr bool isAsciiDecDigit(Rune rune) {
    return rune >= '0' and rune <= '9';
}

export constexpr bool isAsciiHexDigit(Rune rune) {
    return isAsciiDecDigit(rune) or (rune >= 'a' and rune <= 'f') or (rune >= 'A' and rune <= 'F');
}

export constexpr bool isAsciiBlank(Rune rune) {
    return rune == ' ' or rune == '\t';
}

export constexpr bool isAsciiSpace(Rune rune) {
    return rune == ' ' or (rune >= '\t' and rune <= '\r');
}

export constexpr bool isAsciiPunct(Rune rune) {
    return (rune >= '!' and rune <= '/') or (rune >= ':' and rune <= '@') or
           (rune >= '[' and rune <= '`') or (rune >= '{' and rune <= '~');
}

export constexpr bool isAsciiGraph(Rune rune) {
    return rune >= '!' and rune <= '~';
}

export constexpr bool isAsciiPrint(Rune rune) {
    return rune >= ' ' and rune <= '~';
}

export constexpr bool isAsciiC0Control(Rune rune) {
    return rune <= 0x20;
}

export constexpr bool isAsciiControl(Rune rune) {
    return isAsciiC0Control(rune) or rune == 0x7F;
}

export constexpr bool isUnicode(Rune rune) {
    return rune <= 0x10FFFF;
}

export constexpr bool isUnicodeControl(Rune rune) {
    return isAsciiC0Control(rune) or (rune >= 0x7E and rune <= 0x9F);
}

export constexpr bool isUnicodeSurrogate(Rune rune) {
    return rune >= 0xD800 and rune <= 0xDFFF;
}

export constexpr bool isUnicodeScalar(Rune rune) {
    return isUnicode(rune) and not isUnicodeSurrogate(rune);
}

export constexpr bool isUnicodeNonCharacter(Rune rune) {
    return isUnicode(rune) and
           ((rune >= 0xFDD0 and rune <= 0xFDEF) or
            ((rune & 0xFFFE) == 0xFFFE) or
            ((rune & 0xFFFF) == 0xFFFF));
}

// MARK: Rune Conversion -------------------------------------------------------

export constexpr Rune toAsciiLower(Rune rune) {
    if (isAsciiUpper(rune))
        return rune + 0x20;

    return rune;
}

export constexpr Rune toAsciiUpper(Rune rune) {
    if (isAsciiLower(rune))
        return rune - 0x20;

    return rune;
}

// MARK: Rune Parsing ----------------------------------------------------------

export constexpr Rune parseAsciiDecDigit(Rune rune) {
    if (not isAsciiDecDigit(rune)) [[unlikely]]
        panic("invalid decimal digit");

    return rune - '0';
}

export constexpr Rune parseAsciiHexDigit(Rune rune) {
    rune = toAsciiLower(rune);

    if (isAsciiDecDigit(rune))
        return rune - '0';

    if (rune >= 'a' and rune <= 'f')
        return rune - 'a' + 10;

    panic("invalid hexadecimal digit");
}

} // namespace Karm
