#pragma once

#include <web-dom/node.h>

#include "tokenizer.h"

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
    ITER(AFTER_AFTER_FRAMESET)

struct Parser : private Sink {
    enum struct InsertionMode {
#define ITER(NAME) NAME,
        FOREACH_INSERTION_MODE(ITER)
#undef ITER
    };

    InsertionMode _insertionMode;
    Tokenizer _tokenizer{*this};

    Strong<Dom::Document> _document;

    // https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode
    void _handleInitialMode(Token const &t) {
        if (t.type == Token::CHARACTER and
            (t.rune == '\t' or
             t.rune == '\n' or
             t.rune == '\f' or
             t.rune == ' ')) {
            return;
        } else if (t.type == Token::COMMENT) {
            _document->appendChild(makeStrong<Dom::Comment>(t.data));
        }
    }

    void _acceptAs(InsertionMode mode, Token const &t) {
        switch (mode) {

        case InsertionMode::INITIAL:
            _handleInitialMode(t);
            break;

        // https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
        case InsertionMode::BEFORE_HTML: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
        case InsertionMode::BEFORE_HEAD: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
        case InsertionMode::IN_HEAD: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
        case InsertionMode::IN_HEAD_NOSCRIPT: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
        case InsertionMode::AFTER_HEAD: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
        case InsertionMode::IN_BODY: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intext
        case InsertionMode::TEXT: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
        case InsertionMode::IN_TABLE: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intabletext
        case InsertionMode::IN_TABLE_TEXT: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incaption
        case InsertionMode::IN_CAPTION: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incolumngroup
        case InsertionMode::IN_COLUMN_GROUP: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intablebody
        case InsertionMode::IN_TABLE_BODY: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inrow
        case InsertionMode::IN_ROW: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incell
        case InsertionMode::IN_CELL: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselect
        case InsertionMode::IN_SELECT: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselectintable
        case InsertionMode::IN_SELECT_IN_TABLE: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intemplate
        case InsertionMode::IN_TEMPLATE: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#the-after-body-insertion-mode
        case InsertionMode::AFTER_BODY: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#the-in-frameset-insertion-mode
        case InsertionMode::IN_FRAMESET: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#the-after-frameset-insertion-mode
        case InsertionMode::AFTER_FRAMESET: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-body-insertion-mode
        case InsertionMode::AFTER_AFTER_BODY: {
            break;
        }

        // https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-frameset-insertion-mode
        case InsertionMode::AFTER_AFTER_FRAMESET: {
            break;
        }

        default:
            break;
        }
    }

    void accept(Token const &t) override {
        _acceptAs(_insertionMode, t);
    }
};

} // namespace Web::Html
