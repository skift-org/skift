#pragma once

#include <karm-base/ctype.h>
#include <karm-base/func.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>

namespace Vaev::Html {

#define FOREACH_TOKEN(TOKEN) \
    TOKEN(NIL)               \
    TOKEN(DOCTYPE)           \
    TOKEN(START_TAG)         \
    TOKEN(END_TAG)           \
    TOKEN(COMMENT)           \
    TOKEN(CHARACTER)         \
    TOKEN(END_OF_FILE)

struct Token {
    enum Type {

#define ITER(NAME) NAME,
        FOREACH_TOKEN(ITER)
#undef ITER

            _LEN,
    };

    struct Attr {
        String name{};
        String value{};
    };

    Type type = NIL;
    String name;
    Rune rune = '\0';
    String data;
    String publicIdent;
    String systemIdent;
    Vec<Attr> attrs;
    bool forceQuirks{false};
    bool selfClosing{false};
};

struct Sink {
    virtual ~Sink() = default;
    virtual void accept(Token const &token) = 0;
};

struct Lexer {
    enum struct State {
#define STATE(NAME) NAME,
#include "defs/states.inc"
#undef STATE

        _LEN,
    };

    using enum State;

    State _state = State::DATA;
    State _returnState = State::NIL;

    Opt<Token> _token;
    Opt<Token> _last;
    Sink *_sink = nullptr;

    Rune _currChar = 0;
    StringBuilder _builder;
    StringBuilder _temp;

    Token &_begin(Token::Type type) {
        _token = Token{};
        _token->type = type;
        return *_token;
    }

    Token &_ensure() {
        if (not _token)
            panic("unexpected-token");
        return *_token;
    }

    Token &_ensure(Token::Type type) {
        auto &token = _ensure();
        if (token.type != type)
            panic("unexpected-token");
        return token;
    }

    void _emit() {
        if (not _sink)
            panic("no sink");
        _sink->accept(_ensure());
        _last = std::move(_token);
    }

    void _emit(Rune rune) {
        _begin(Token::CHARACTER).rune = rune;
        _emit();
    }

    void _beginAttribute() {
        _ensure().attrs.emplaceBack();
    }

    Token::Attr &_lastAttr() {
        auto &token = _ensure();
        if (token.attrs.len() == 0)
            panic("_beginAttribute miss match");
        return last(token.attrs);
    }

    void _reconsumeIn(State state, Rune rune) {
        _switchTo(state);
        consume(rune);
    }

    void _switchTo(State state) {
        _state = state;
    }

    void _raise(Str msg);

    bool _isAppropriateEndTagToken() {
        if (not _last or not _token)
            return false;
        return _last->name == _token->name;
    }

    void _flushCodePointsConsumedAsACharacterReference() {
        debug("flushing code points consumed as a character reference");
    }

    void bind(Sink &sink) {
        if (_sink)
            panic("sink already bound");
        _sink = &sink;
    }

    void consume(Rune rune, bool isEof = false);
};

#undef FOREACH_TOKEN

} // namespace Vaev::Html

template <>
struct Karm::Io::Formatter<Vaev::Html::Token> {
    Res<usize> format(Io::TextWriter &writer, Vaev::Html::Token const &val) {
        usize written = try$(Io::format(writer, "({}", val.type));

        if (val.name)
            written += try$(Io::format(writer, " name={}", val.name));

        if (val.rune)
            written += try$(Io::format(writer, " rune='{#c}'", val.rune));

        if (val.data)
            written += try$(Io::format(writer, " data='{}'", val.data));

        if (val.publicIdent)
            written += try$(Io::format(writer, " publicIdent='{}'", val.publicIdent));

        if (val.systemIdent)
            written += try$(Io::format(writer, " systemIdent='{}'", val.systemIdent));

        if (val.attrs.len() > 0)
            for (auto &attr : val.attrs) {
                written += try$(Io::format(writer, " attr:{}='{}'", attr.name, attr.value));
            }

        if (val.forceQuirks)
            written += try$(Io::format(writer, " forceQuirks"));

        if (val.selfClosing)
            written += try$(Io::format(writer, " selfClosing"));

        written += try$(writer.writeRune(')'));
        return Ok(written);
    }
};
