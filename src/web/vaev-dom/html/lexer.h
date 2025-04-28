#pragma once

#include <karm-io/emit.h>

namespace Vaev::Dom {

#define FOREACH_TOKEN(TOKEN) \
    TOKEN(NIL)               \
    TOKEN(DOCTYPE)           \
    TOKEN(START_TAG)         \
    TOKEN(END_TAG)           \
    TOKEN(COMMENT)           \
    TOKEN(CHARACTER)         \
    TOKEN(END_OF_FILE)

struct HtmlToken {
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
    String name = ""s;
    Rune rune = '\0';
    String data = ""s;
    String publicIdent = ""s;
    String systemIdent = ""s;
    Vec<Attr> attrs = {};
    bool forceQuirks{false};
    bool selfClosing{false};

    void repr(Io::Emit& e) const {
        e("({}", type);
        if (name)
            e(" name={}", name);
        if (rune)
            e(" rune='{#c}'", rune);
        if (data)
            e(" data={#}", data);
        if (publicIdent)
            e(" publicIdent={#}", publicIdent);
        if (systemIdent)
            e(" systemIdent={#}", systemIdent);
        if (attrs.len() > 0) {
            e.indentNewline();
            for (auto& attr : attrs)
                e("({} {#})", attr.name, attr.value);
            e.deindent();
        }
        if (forceQuirks)
            e(" forceQuirks");
        if (selfClosing)
            e(" selfClosing");
        e(")");
    }

    bool hasAttribute(Str name) const {
        for (auto& attr : attrs) {
            if (attr.name == name)
                return true;
        }
        return false;
    }
};

struct HtmlSink {
    virtual ~HtmlSink() = default;
    virtual void accept(HtmlToken const& token) = 0;
};

struct HtmlLexer {
    enum struct State {
#define STATE(NAME) NAME,
#include "../defs/ns-html-states.inc"
#undef STATE

        _LEN,
    };

    using enum State;

    State _state = State::DATA;
    State _returnState = State::NIL;

    Opt<HtmlToken> _token;
    Opt<HtmlToken> _last;
    HtmlSink* _sink = nullptr;

    Rune _currChar = 0;
    StringBuilder _builder;
    StringBuilder _temp;
    StringBuilder _peek;

    Opt<usize> matchedCharReferenceNoSemiColon;

    HtmlToken& _begin(HtmlToken::Type type) {
        _token = HtmlToken{.type = type};
        return *_token;
    }

    HtmlToken& _ensure() {
        if (not _token)
            panic("unexpected-token");
        return *_token;
    }

    HtmlToken& _ensure(HtmlToken::Type type) {
        auto& token = _ensure();
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
        _begin(HtmlToken::CHARACTER).rune = rune;
        _emit();
    }

    void _beginAttribute() {
        _ensure().attrs.emplaceBack();
    }

    HtmlToken::Attr& _lastAttr() {
        auto& token = _ensure();
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
        return _last.unwrap().name == _token.unwrap().name;
    }

    void _flushCodePointsConsumedAsACharacterReference() {
        for (auto codePoint : iterRunes(_temp.str())) {
            if (_consumedAsPartOfAnAttribute()) {
                _builder.append(codePoint);
            } else {
                _emit(codePoint);
            }
        }
    }

    bool _consumedAsPartOfAnAttribute() {
        return _returnState == State::ATTRIBUTE_VALUE_DOUBLE_QUOTED ||
               _returnState == State::ATTRIBUTE_VALUE_SINGLE_QUOTED ||
               _returnState == State::ATTRIBUTE_VALUE_UNQUOTED;
    }

    void bind(HtmlSink& sink) {
        if (_sink)
            panic("sink already bound");
        _sink = &sink;
    }

    void consume(Rune rune, bool isEof = false);
};

#undef FOREACH_TOKEN

} // namespace Vaev::Dom
