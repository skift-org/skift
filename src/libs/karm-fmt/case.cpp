#include <karm-io/expr.h>

#include "case.h"

namespace Karm::Fmt {

Res<String> toDefaultCase(Str str) {
    return Ok(str);
}

Res<String> toCamelCase(Str str) {
    auto s = try$(toPascalCase(str));

    if (s.len() > 0) {
        s[0] = tolower(s[0]);
    }

    return Ok(s);
}

Res<String> toCapitalCase(Str str) {
    return toTitleCase(str);
}

Res<String> toConstantCase(Str str) {
    auto s = try$(toNoCase(str));

    for (auto &c : s) {
        if (c == ' ') {
            c = '_';
        } else {
            c = toupper(c);
        }
    }

    return Ok(s);
}

Res<String> toDotCase(Str str) {
    auto s = try$(toNoCase(str));

    for (auto &c : s) {
        if (c == ' ') {
            c = '.';
        }
    }

    return Ok(s);
}

Res<String> toHeaderCase(Str str) {
    auto s = try$(toTitleCase(str));

    for (auto &c : s) {
        if (c == ' ') {
            c = '-';
        }
    }

    return Ok(s);
}

static auto sep() {
    return Re::either(Re::blank(), Re::single('.', '_', '/', '-'));
}

Res<String> toNoCase(Str str) {
    Io::StringWriter writer;
    Io::SScan scan{str};
    bool wasLower = false;

    scan.eat(sep());

    while (not scan.ended()) {
        if (scan.skip(sep())) {
            try$(writer.writeRune(' '));
            wasLower = false;
        } else {
            if (wasLower and isupper(scan.curr())) {
                try$(writer.writeRune(' '));
            }

            wasLower = islower(scan.curr()) and isalpha(scan.curr());
            try$(writer.writeRune(tolower(scan.next())));
        }
    }

    return Ok(writer.take());
}

Res<String> toParamCase(Str str) {
    auto s = try$(toNoCase(str));

    for (auto &c : s) {
        if (c == ' ') {
            c = '-';
        }
    }

    return Ok(s);
}

Res<String> toPascalCase(Str str) {
    bool first = true;
    Io::StringWriter writer;
    Io::SScan scan{str};

    scan.eat(sep());

    while (not scan.ended()) {
        if (scan.skip(sep())) {
            first = true;
        } else {
            if (first) {
                try$(writer.writeRune(toupper(scan.next())));
                first = false;
            } else {
                try$(writer.writeRune(tolower(scan.next())));
            }
        }
    }

    return Ok(writer.take());
}

Res<String> toPathCase(Str str) {
    auto s = try$(toNoCase(str));

    for (auto &c : s) {
        if (c == ' ') {
            c = '/';
        }
    }

    return Ok(s);
}

Res<String> toSentenceCase(Str str) {
    auto s = try$(toNoCase(str));

    if (s.len() > 0) {
        s[0] = toupper(s[0]);
    }

    return Ok(s);
}

Res<String> toSnakeCase(Str str) {
    auto s = try$(toNoCase(str));

    for (auto &c : s) {
        if (c == ' ') {
            c = '_';
        }
    }

    return Ok(s);
}

Res<String> toTitleCase(Str str) {
    auto s = try$(toNoCase(str));

    for (usize i = 0; i < s.len(); i++) {
        if (i == 0 or s[i - 1] == ' ') {
            s[i] = toupper(s[i]);
        }
    }

    return Ok(s);
}

Res<String> toSwapCase(Str str) {
    Io::StringWriter writer;
    Io::SScan scan{str};

    while (not scan.ended()) {
        auto c = scan.next();

        if (islower(c)) {
            try$(writer.writeRune(toupper(c)));
        } else if (isupper(c)) {
            try$(writer.writeRune(tolower(c)));
        } else {
            try$(writer.writeRune(c));
        }
    }

    return Ok(writer.take());
}

Res<String> toLowerCase(Str str) {
    Io::StringWriter writer;
    Io::SScan scan{str};

    while (not scan.ended()) {
        auto c = scan.next();

        if (isupper(c)) {
            try$(writer.writeRune(tolower(c)));
        } else {
            try$(writer.writeRune(c));
        }
    }

    return Ok(writer.take());
}

Res<String> toLowerFirstCase(Str str) {
    String s = str;

    if (s.len() > 0) {
        s[0] = tolower(s[0]);
    }

    return Ok(s);
}

Res<String> toUpperCase(Str str) {
    Io::StringWriter writer;
    Io::SScan scan{str};

    while (not scan.ended()) {
        auto c = scan.next();

        if (islower(c)) {
            try$(writer.writeRune(toupper(c)));
        } else {
            try$(writer.writeRune(c));
        }
    }

    return Ok(writer.take());
}

Res<String> toUpperFirstCase(Str str) {
    String s = str;

    if (s.len() > 0) {
        s[0] = toupper(s[0]);
    }

    return Ok(s);
}

Res<String> toSpongeCase(Str str) {
    String s = str;
    for (usize i = 0; i < s.len(); i++) {
        if (i % 2 == 0) {
            s[i] = toupper(s[i]);
        } else {
            s[i] = tolower(s[i]);
        }
    }
    return Ok(s);
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

} // namespace Karm::Fmt
