#pragma once

#include <web-dom/document.h>
#include <web-dom/element.h>
#include <web-dom/text.h>

#include "lexer.h"

namespace Web::Html {

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

struct Builder {
    // 13.2.6 Tree construction
    // https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

    enum struct Mode {
#define ITER(NAME) NAME,
        FOREACH_INSERTION_MODE(ITER)
#undef ITER
    };

    Mode _mode = Mode::INITIAL;
    Lexer _lexer;
    Strong<Dom::Document> _document;
    Vec<Strong<Dom::Element>> _openElements;
    Opt<Strong<Dom::Element>> _headElement;
    Opt<Strong<Dom::Element>> _formElement;

    Builder(Strong<Dom::Document> document)
        : _document(document) {
    }

    // 13.2.2 Parse errors
    // https://html.spec.whatwg.org/multipage/parsing.html#parse-errors

    void _raise(Str msg = "parse-error");

    // 13.2.6.1 Creating and inserting nodes
    // https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes

    void _apropriatePlaceForInsertingANode();

    Strong<Dom::Element> _createElementFor(Token const &t);

    void _insertAnElementAtTheAdjustedInsertionLocation();

    void _insertAForeignElement(Token const &t);

    void _insertAnHtmlElement(Strong<Dom::Element>);

    void _insertACharacter();

    void _insertAComment();

    //////////////////////////////////////////

    void _switchTo(Mode mode);

    // 13.2.6.4 The rules for parsing tokens in HTML content

    // 13.2.6.4.1 The "initial" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode
    Dom::QuirkMode _whichQuirkMode(Token const &);

    void _handleInitialMode(Token const &t);

    // 13.2.6.4.2 The "before html" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
    void _handleBeforeHtml(Token const &t);

    // 13.2.6.4.3 The "before head" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-beforehead
    void _handleBeforeHead(Token const &t);

    // 13.2.6.4.4 The "in head" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
    void _acceptInHead(Token const &t);

    void _acceptIn(Mode mode, Token const &t);

    void accept(Token const &t);
};

static inline Str toStr(Builder::Mode mode) {
    switch (mode) {
#define ITER(NAME)            \
    case Builder::Mode::NAME: \
        return #NAME;
        FOREACH_INSERTION_MODE(ITER)
#undef ITER
    default:
        panic("invalid mode");
    }
}

} // namespace Web::Html
