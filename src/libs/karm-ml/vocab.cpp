module;

#include <karm-logger/logger.h>

export module Karm.Ml:vocab;

import Karm.Core;

namespace Karm::Ml {

export struct BpeVocab {
    using Token = u32;

    struct TokenInfos {
        enum struct Type {
            NORMAL,
            UNKNOWN,
            CONTROL,
            USER_DEFINED,
            UNUSED,
        };

        using enum Type;

        Type type;
        String text;
        Token token;
    };

    Vec<TokenInfos> _infos;
    Regex::Regex _regex = "'s|'t|'re|'ve|'m|'ll|'d| ?\\p{L}+| ?\\p{N}+| ?[^\\s\\p{L}\\p{N}]+|\\s+(?!\\S)|\\s+"_regex;

    static Rune _u8ToUnicode(u8 in) {
        if ((U'!' <= in and in <= U'~') or
            (U'¡' <= in and in <= U'¬') or
            (U'®' <= in and in <= U'ÿ'))
            return in;
        return 256 + in;
    }

    static u8 _unicodeToU8(Rune in) {
        if (in >= 256)
            return in - 256;
        return in;
    }

    Vec<Token> encode(Str) {
        return {};
    }

    String decode(Slice<Token>) {
        return ""s;
    }
};

} // namespace Karm::Ml
