#pragma once

#include <vaev-dom/document.h>
#include <vaev-dom/element.h>
#include <vaev-dom/text.h>

#include "lexer.h"

namespace Vaev::Html {

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

struct Parser : public Sink {
    enum struct Mode {
#define ITER(NAME) NAME,
        FOREACH_INSERTION_MODE(ITER)
#undef ITER
    };

    bool _scriptingEnabled = false;
    bool _framesetOk = true;

    Mode _insertionMode = Mode::INITIAL;
    Mode _originalInsertionMode = Mode::INITIAL;

    Lexer _lexer;
    Strong<Dom::Document> _document;
    Vec<Strong<Dom::Element>> _openElements;
    Opt<Strong<Dom::Element>> _headElement;
    Opt<Strong<Dom::Element>> _formElement;

    Parser(Strong<Dom::Document> document)
        : _document(document) {
        _lexer.bind(*this);
    }

    void _raise(Str msg = "parse-error");

    void _handleInitialMode(Token const &t);

    void _handleBeforeHtml(Token const &t);

    void _handleBeforeHead(Token const &t);

    void _handleInHead(Token const &t);

    void _handleInHeadNoScript(Token const &t);

    void _handleAfterHead(Token const &t);

    void _handleInBody(Token const &t);

    void _handleText(Token const &t);

    void _switchTo(Mode mode);

    void _acceptIn(Mode mode, Token const &t);

    void accept(Token const &t) override;

    void write(Str str) {
        for (auto r : iterRunes(str))
            _lexer.consume(r);
    }
};

static inline Str toStr(Parser::Mode mode) {
    switch (mode) {
#define ITER(NAME)           \
    case Parser::Mode::NAME: \
        return #NAME;
        FOREACH_INSERTION_MODE(ITER)
#undef ITER
    default:
        panic("invalid mode");
    }
}

} // namespace Vaev::Html
