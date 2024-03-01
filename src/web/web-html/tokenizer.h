#pragma once

#include <karm-base/ctype.h>
#include <karm-base/func.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>

namespace Web::Html {

#define FOREACH_TOKEN(ITER) \
    ITER(NIL)               \
    ITER(DOCTYPE)           \
    ITER(START_TAG)         \
    ITER(END_TAG)           \
    ITER(COMMENT)           \
    ITER(CHARACTER)         \
    ITER(END_OF_FILE)

struct Token {
    enum Type {
#define ITER(NAME) NAME,
        FOREACH_TOKEN(ITER)
#undef ITER
    };

    struct Attr {
        Opt<String> name;
        Opt<String> value;
    };

    Type type = NIL;
    Opt<String> name = NONE;
    Opt<Rune> rune = NONE;
    Opt<String> data = NONE;
    Opt<String> publicIdent = NONE;
    Opt<String> systemIdent = NONE;
    Vec<Attr> attrs;
    bool forceQuirks{false};
    bool selfClosing{false};
};

struct Sink {
    virtual ~Sink() = default;
    virtual void accept(Token const &token) = 0;
};

struct Tokenizer {
    enum struct State {
#define STATE(NAME) NAME,
#include "states.inc"
#undef STATE
    };

    State _state = State::DATA;
    State _returnState = State::NIL;

    Opt<Token> _token;
    Opt<Token> _last;

    Rune _currChar = 0;
    StringBuilder _builder;
    StringBuilder _temp;
    StringBuilder _wait;

    Sink &_sink;

    Tokenizer(Sink &sink)
        : _sink(sink) {}

    enum Match {
        NO,
        MAYBE,
        YES,
    };

    void _begin(Token::Type type) {
        _token = Token{};
        _token->type = type;
    }

    Token &_ensure() {
        if (not _token)
            _raise("unexpected-token");
        return *_token;
    }

    Token &_ensure(Token::Type type) {
        auto &token = _ensure();
        if (token.type != type)
            _raise("unexpected-token");
        return token;
    }

    void _emit() {
        _sink.accept(_ensure());
        _last = std::move(_token);
    }

    void _emit(Rune rune) {
        _begin(Token::CHARACTER);
        _ensure().rune = rune;
        _emit();
    }

    void _emitEof() {
        _begin(Token::END_OF_FILE);
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

    Match _await(Rune r, Str what) {
        if (startWith(what, _wait.str())) {
            _wait.append(r);

            if (not startWith(what, _wait.str())) {
                _wait.clear();
                return Match::NO;
            }

            if (_wait.str().len() == what.len()) {
                _wait.clear();
                return Match::YES;
            }

            return Match::MAYBE;
        }

        return Match::NO;
    }

    void _reconsumeIn(State state, Rune rune) {
        _switchTo(state);
        consume(rune);
    }

    void _switchTo(State state) {
        _state = state;
    }

    void _raise(Str msg) {
        logError("tokenizer error: {}", msg);
    }

    bool _isAppropriateEndTagToken() {
        if (not _last or not _token)
            return false;
        return _last->name == _token->name;
    }

    void _flushCodePointsConsumedAsACharacterReference() {
        debug("flushing code points consumed as a character reference");
    }

    void consume(Rune rune, bool isEof = false);
};

static inline Str toStr(Token::Type type) {
    switch (type) {
#define ITER(NAME)          \
    case Token::Type::NAME: \
        return #NAME;
        FOREACH_TOKEN(ITER)
#undef ITER
    default:
        panic("invalid token type");
    }
}

static inline Str toStr(Tokenizer::State state) {
    switch (state) {
#define STATE(NAME)              \
    case Tokenizer::State::NAME: \
        return #NAME;
#include "states.inc"
#undef STATE
    default:
        panic("invalid state");
    }
}

} // namespace Web::Html

template <>
struct Karm::Io::Formatter<Web::Html::Token> {
    Res<usize> format(Io::TextWriter &writer, Web::Html::Token const &val) {
        usize written = try$(writer.writeRune('('));
        written += try$(writer.writeStr(try$(Io::toParamCase(Web::Html::toStr(val.type)))));

        if (val.name)
            written += try$(Io::format(writer, " name={}", val.name.unwrap()));

        if (val.rune)
            written += try$(Io::format(writer, " rune={}", val.rune.unwrap()));

        if (val.data)
            written += try$(Io::format(writer, " data='{}'", val.data.unwrap()));

        if (val.publicIdent)
            written += try$(Io::format(writer, " publicIdent='{}'", val.publicIdent.unwrap()));

        if (val.systemIdent)
            written += try$(Io::format(writer, " systemIdent='{}'", val.systemIdent.unwrap()));

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
