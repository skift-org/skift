#pragma once

#include <karm-base/ctype.h>
#include <karm-base/func.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>

#include "dom.h"

namespace Vaev::Markup {

// MARK: Lexer -----------------------------------------------------------------

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
};

struct HtmlSink {
    virtual ~HtmlSink() = default;
    virtual void accept(HtmlToken const& token) = 0;
};

struct HtmlLexer {
    enum struct State {
#define STATE(NAME) NAME,
#include "defs/ns-html-states.inc"
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
    StringBuilder _builder, _commentBuilder;
    StringBuilder _temp;
    StringBuilder peekerForSingleState;

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

// MARK: Parser ----------------------------------------------------------------

#define FOREACH_INSERTION_MODE(MODE) \
    MODE(INITIAL)                    \
    MODE(BEFORE_HTML)                \
    MODE(BEFORE_HEAD)                \
    MODE(IN_HEAD)                    \
    MODE(IN_HEAD_NOSCRIPT)           \
    MODE(AFTER_HEAD)                 \
    MODE(IN_BODY)                    \
    MODE(TEXT)                       \
    MODE(IN_TABLE)                   \
    MODE(IN_TABLE_TEXT)              \
    MODE(IN_CAPTION)                 \
    MODE(IN_COLUMN_GROUP)            \
    MODE(IN_TABLE_BODY)              \
    MODE(IN_ROW)                     \
    MODE(IN_CELL)                    \
    MODE(IN_SELECT)                  \
    MODE(IN_SELECT_IN_TABLE)         \
    MODE(IN_TEMPLATE)                \
    MODE(AFTER_BODY)                 \
    MODE(IN_FRAMESET)                \
    MODE(AFTER_FRAMESET)             \
    MODE(AFTER_AFTER_BODY)           \
    MODE(AFTER_AFTER_FRAMESET)

struct HtmlParser : public HtmlSink {
    enum struct Mode {
#define ITER(NAME) NAME,
        FOREACH_INSERTION_MODE(ITER)
#undef ITER

            _LEN,
    };

    bool _scriptingEnabled = false;
    bool _framesetOk = true;
    bool _fosterParenting = false;

    Mode _insertionMode = Mode::INITIAL;
    Mode _originalInsertionMode = Mode::INITIAL;

    HtmlLexer _lexer;
    Rc<Document> _document;
    Vec<Rc<Element>> _openElements;
    Opt<Rc<Element>> _headElement;
    Opt<Rc<Element>> _formElement;

    Vec<HtmlToken> _pendingTableCharacterTokens;

    HtmlParser(Rc<Document> document)
        : _document(document) {
        _lexer.bind(*this);
    }

    void _raise(Str msg = "parse-error");

    // MARK: Utilities

    // FIXME: remaining elements from https://html.spec.whatwg.org/#has-an-element-in-scope
    static constexpr Array BASIC_SCOPE_DELIMITERS = {
        Html::APPLET, Html::CAPTION, Html::HTML, Html::TABLE, Html::TD,
        Html::TH, Html::MARQUEE, Html::OBJECT, Html::TEMPLATE
    };

    bool _hasElementInLambdaScope(TagName tag, Func<bool(TagName)> inScopeList) {
        if (_openElements.len() == 0)
            panic("html element should always be in scope");

        for (usize i = _openElements.len() - 1; i >= 0; --i) {
            auto& el = _openElements[i];
            if (el->tagName == tag)
                return true;
            else if (inScopeList(el->tagName))
                return false;
        }

        unreachable();
    }

    // https://html.spec.whatwg.org/#has-an-element-in-scope
    bool _hasElementInScope(TagName tag) {
        return _hasElementInLambdaScope(tag, [](TagName tag) -> bool {
            return contains(BASIC_SCOPE_DELIMITERS, tag);
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-button-scope
    bool _hasElementInButtonScope(TagName tag) {
        return _hasElementInLambdaScope(tag, [](TagName tag) -> bool {
            return tag == Html::BUTTON or contains(BASIC_SCOPE_DELIMITERS, tag);
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-list-item-scope
    bool _hasElementInListItemScope(TagName tag) {
        return _hasElementInLambdaScope(tag, [](TagName tag) -> bool {
            return tag == Html::OL or tag == Html::UL or contains(BASIC_SCOPE_DELIMITERS, tag);
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-table-scope
    bool _hasElementInTableScope(TagName tag) {
        return _hasElementInLambdaScope(tag, [](TagName tag) -> bool {
            return tag == Html::HTML or tag == Html::TABLE or tag == Html::TEMPLATE;
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-select-scope
    bool _hasElementInSelectScope(TagName tag) {
        return _hasElementInLambdaScope(tag, [](TagName tag) -> bool {
            return tag != Html::OPTGROUP and tag != Html::OPTION;
        });
    }

    // MARK: Modes

    void _handleInitialMode(HtmlToken const& t);

    void _handleBeforeHtml(HtmlToken const& t);

    void _handleBeforeHead(HtmlToken const& t);

    void _handleInHead(HtmlToken const& t);

    void _handleInHeadNoScript(HtmlToken const& t);

    void _handleAfterHead(HtmlToken const& t);

    void _handleInBody(HtmlToken const& t);

    void _handleText(HtmlToken const& t);

    void _handleInTable(HtmlToken const& t);

    void _handleInTableText(HtmlToken const& t);

    void _handleInTableBody(HtmlToken const& t);

    void _handleInTableRow(HtmlToken const& t);

    void _handleInCell(HtmlToken const& t);

    void _handleAfterBody(HtmlToken const& t);

    void _switchTo(Mode mode);

    void _acceptIn(Mode mode, HtmlToken const& t);

    void accept(HtmlToken const& t) override;

    void write(Str str) {
        for (auto r : iterRunes(str))
            _lexer.consume(r);
        // NOTE: '\3' (End of Text) is used here as a placeholder so we are directed to the EOF case
        _lexer.consume('\3', true);
    }
};

#undef FOREACH_INSERTION_MODE

} // namespace Vaev::Markup
