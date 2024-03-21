#include <web-dom/comment.h>
#include <web-dom/document-type.h>

#include "builder.h"

namespace Web::Html {

// 13.2.6 Tree construction
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

// 13.2.2 Parse errors
// https://html.spec.whatwg.org/multipage/parsing.html#parse-errors

void Builder::_raise(Str msg) {
    logError("{}: {}", toStr(_mode), msg);
}

void Builder::_switchTo(Mode mode) {
    _mode = mode;
}

// https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
Strong<Dom::Element> Builder::_createElementFor(Token const &t) {
    auto el = makeStrong<Dom::Element>(t.name.unwrap());
    // NOSPEC: Keep it simple for the POC
    return el;
}

// 13.2.6.4 The rules for parsing tokens in HTML content

// 13.2.6.4.1 The "initial" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode

Dom::QuirkMode Builder::_whichQuirkMode(Token const &) {
    // NOSPEC: We assume no quirk mode
    return Dom::QuirkMode::NO;
}

void Builder::_handleInitialMode(Token const &t) {
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    } else if (t.type == Token::COMMENT) {
        _document->appendChild(makeStrong<Dom::Comment>(t.data.unwrap()));
    } else if (t.type == Token::DOCTYPE) {
        _document->appendChild(makeStrong<Dom::DocumentType>(
            tryOr(t.name, ""),
            tryOr(t.publicIdent, ""),
            tryOr(t.systemIdent, "")));
        _document->quirkMode = _whichQuirkMode(t);
        _switchTo(Mode::BEFORE_HTML);
    } else {
        _raise();
        _switchTo(Mode::BEFORE_HTML);
        accept(t);
    }
}

// https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
void Builder::_handleBeforeHtml(Token const &t) {
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    } else if (t.type == Token::DOCTYPE) {
        // ignore
        _raise();
    } else if (t.type == Token::COMMENT) {
        _document->appendChild(makeStrong<Dom::Comment>(t.data.unwrap()));
    } else if (t.type == Token::START_TAG and t.name == "html") {
        auto el = _createElementFor(t);
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
    } else if (t.type == Token::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    } else {
        auto el = makeStrong<Dom::Element>("html");
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
        accept(t);
    }
}

// https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
void Builder::_handleBeforeHead(Token const &t) {
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // Ignore the token.
    } else if (t.type == Token::COMMENT) {
        // Insert a comment.
        _insertAComment();
    } else if (t.type == Token::DOCTYPE) {
        // Parse error. Ignore the token.
        _raise();
    } else if (t.type == Token::START_TAG and t.name == "html") {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    } else if (t.type == Token::START_TAG and t.name == "head") {
        auto head = _createElementFor(t);
        _insertAnHtmlElement(head);
        _headElement = head;
    } else if (t.type == Token::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    } else {
        Token headToken;
        headToken.type = Token::START_TAG;
        headToken.name = String{"head"};
        auto head = _createElementFor(headToken);
        _insertAnHtmlElement(head);
        _headElement = head;
        _switchTo(Mode::IN_HEAD);
        accept(t);
    }
}

// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
void Builder::_acceptInHead(Token const &t) {
    if (t.type == Token::DOCTYPE) {
        _raise();
    } else if (t.type == Token::COMMENT) {
        _insertAComment();
    } else if (t.type == Token::CHARACTER and
               (t.rune == '\t' or
                t.rune == '\n' or
                t.rune == '\f' or
                t.rune == ' ')) {
        // Ignore the token.
    } else if (t.type == Token::START_TAG) {

    } else if (t.type == Token::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    }
}

void Builder::_acceptIn(Mode mode, Token const &t) {
    logDebug("{}: {}", toStr(mode), t);

    switch (mode) {

    case Mode::INITIAL:
        _handleInitialMode(t);
        break;

    case Mode::BEFORE_HTML:
        _handleBeforeHtml(t);
        break;

    case Mode::BEFORE_HEAD:
        _handleBeforeHead(t);
        break;

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
    case Mode::IN_HEAD: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
    case Mode::IN_HEAD_NOSCRIPT: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
    case Mode::AFTER_HEAD: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
    case Mode::IN_BODY: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intext
    case Mode::TEXT: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
    case Mode::IN_TABLE: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intabletext
    case Mode::IN_TABLE_TEXT: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incaption
    case Mode::IN_CAPTION: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incolumngroup
    case Mode::IN_COLUMN_GROUP: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intablebody
    case Mode::IN_TABLE_BODY: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inrow
    case Mode::IN_ROW: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incell
    case Mode::IN_CELL: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselect
    case Mode::IN_SELECT: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselectintable
    case Mode::IN_SELECT_IN_TABLE: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intemplate
    case Mode::IN_TEMPLATE: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#the-after-body-insertion-mode
    case Mode::AFTER_BODY: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#the-in-frameset-insertion-mode
    case Mode::IN_FRAMESET: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#the-after-frameset-insertion-mode
    case Mode::AFTER_FRAMESET: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-body-insertion-mode
    case Mode::AFTER_AFTER_BODY: {
        break;
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-frameset-insertion-mode
    case Mode::AFTER_AFTER_FRAMESET: {
        break;
    }

    default:
        break;
    }
}

void Builder::accept(Token const &t) {
    _acceptIn(_mode, t);
}

} // namespace Web::Html
