#include "args.h"

namespace Karm::Cli {

// MARK: Tokenizer -------------------------------------------------------------

void tokenize(Str arg, Vec<Token>& out) {
    if (arg == "--"s) {
        out.pushBack(Token::EXTRA);
    } else if (arg == "-"s) {
        out.pushBack("-"s);
    } else if (startWith(arg, "--"s) == Match::PARTIAL) {
        out.emplaceBack(Token::OPTION, next(arg, 2));
    } else if (startWith(arg, "-"s) == Match::PARTIAL) {
        Str flags = next(arg, 1);
        for (auto r : iterRunes(flags))
            out.emplaceBack(r);
    } else {
        out.pushBack(arg);
    }
}

void tokenize(Slice<Str> args, Vec<Token>& out) {
    for (auto& arg : args)
        tokenize(arg, out);
}

void tokenize(int argc, char** argv, Vec<Token>& out) {
    for (int i = 0; i < argc; ++i)
        tokenize(Str::fromNullterminated(argv[i]), out);
}

// MARK: Values ----------------------------------------------------------------

Res<> ValueParser<bool>::usage(Io::TextWriter& w) {
    return w.writeStr("true|false"s);
}

Res<bool> ValueParser<bool>::parse(Cursor<Token>&) {
    return Ok(true);
}

Res<> ValueParser<isize>::usage(Io::TextWriter& w) {
    return w.writeStr("integer"s);
}

Res<isize> ValueParser<isize>::parse(Cursor<Token>& c) {
    if (c.ended() or c->kind != Token::OPERAND)
        return Error::other("missing value");

    auto value = c.next().value;

    auto result = Io::atoi(value);
    if (not result)
        return Error::other("expected integer");

    return Ok(result.unwrap());
}

Res<> ValueParser<Str>::usage(Io::TextWriter& w) {
    return w.writeStr("string"s);
}

Res<Str> ValueParser<Str>::parse(Cursor<Token>& c) {
    if (c.ended() or c->kind != Token::OPERAND)
        return Error::other("missing value");

    return Ok(c.next().value);
}

} // namespace Karm::Cli
