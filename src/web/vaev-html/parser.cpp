#include <vaev-dom/comment.h>
#include <vaev-dom/document-type.h>
#include <vaev-mathml/tags.h>
#include <vaev-svg/tags.h>

#include "parser.h"
#include "tags.h"

namespace Vaev::Html {

// 13.2.2 MARK: Parse errors
// https://html.spec.whatwg.org/multipage/parsing.html#parse-errors

void Parser::_raise(Str msg) {
    logError("{}: {}", _insertionMode, msg);
}

// 13.2.4.2 MARK: The stack of open elements
// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements

// https://html.spec.whatwg.org/multipage/parsing.html#special
bool isSpecial(TagName const &tag) {
    static constexpr Array SPECIAL{
#define SPECIAL(NAME) NAME,
#include "defs/special.inc"
#undef SPECIAL
    };

    return contains(SPECIAL, tag);
}

// 13.2.4.3 MARK: The list of active formatting elements
// https://html.spec.whatwg.org/multipage/parsing.html#list-of-active-formatting-elements

void reconstructActiveFormattingElements(Parser &) {
    // TODO
}

// 13.2.5 MARK: Tokenization
// https://html.spec.whatwg.org/multipage/parsing.html#tokenization

// https://html.spec.whatwg.org/multipage/parsing.html#acknowledge-self-closing-flag
void acknowledgeSelfClosingFlag(Token const &) {
    logDebug("acknowledgeSelfClosingFlag not implemented");
}

// 13.2.6 MARK: Tree construction
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

// 13.2.6.1 MARK: Creating and inserting nodes
// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes

struct AdjustedInsertionLocation {
    Strong<Dom::Element> parent;

    // https://html.spec.whatwg.org/multipage/parsing.html#insert-an-element-at-the-adjusted-insertion-location
    void insert(Strong<Dom::Node> node) {
        // NOSPEC
        parent->appendChild(node);
    }

    Opt<Strong<Dom::Node>> lastChild() {
        // NOSPEC
        if (not parent->hasChildren())
            return NONE;
        return parent->lastChild();
    }
};

// https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
AdjustedInsertionLocation apropriatePlaceForInsertingANode(Parser &b, Opt<Strong<Dom::Element>> overrideTarget = NONE) {
    // 1. If there was an override target specified, then let target be
    //    the override target.
    //
    //    Otherwise, let target be the current node.
    auto target = overrideTarget
                      ? *overrideTarget
                      : last(b._openElements);

    // 2. Determine the adjusted insertion location using the first
    //    matching steps from the following list:

    //    If foster parenting is enabled and target is a table, tbody, tfoot, thead, or tr element

    //    NOTE: Foster parenting happens when content is misnested in tables.

    //    1. Let last template be the last template element in the stack of open elements, if any.

    //    2. Let last table be the last table element in the stack of open elements, if any.

    //    3. If there is a last template and either there is no last table,
    //       or there is one, but last template is lower (more recently added)
    //       than last table in the stack of open elements,
    //       then: let adjusted insertion location be inside last template's
    //       template contents, after its last child (if any), and abort these steps.

    //    4. If there is no last table, then let adjusted insertion location be
    //       inside the first element in the stack of open elements (the html element),
    //       after its last child (if any), and abort these steps. (fragment case)

    //    5. If last table has a parent node, then let adjusted insertion location
    //       be inside last table's parent node, immediately before last table,
    //       and abort these steps.

    //    6. Let previous element be the element immediately above last table
    //       in the stack of open elements.

    //    7. Let adjusted insertion location be inside previous element,
    //       after its last child (if any).

    //  Otherwise: Let adjusted insertion location be inside target,
    //             after its last child (if any).

    // 3. If the adjusted insertion location is inside a template element,
    //    let it instead be inside the template element's template contents,
    //    after its last child (if any).

    // 4. Return the adjusted insertion location.
    return {target};
}

// https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
Strong<Dom::Element> createElementFor(Token const &t, Ns ns) {
    // NOSPEC: Keep it simple for the POC

    // 1. If the active speculative HTML parser is not null, then return the
    //    result of creating a speculative mock element given given namespace,
    //    the tag name of the given token, and the attributes of the given token.

    // 2. Otherwise, optionally create a speculative mock element given given
    //    namespace, the tag name of the given token, and the attributes of
    //    the given token.

    // 3. Let document be intended parent's node document.

    // 4. Let local name be the tag name of the token

    // 5. Let is be the value of the "is" attribute in the given token, if
    //    such an attribute exists, or null otherwise.

    // 6. Let definition be the result of looking up a custom element
    //    definition given document, given namespace, local name, and is.

    // 7. If definition is non-null and the parser was not created as part
    //    of the HTML fragment parsing algorithm, then let will execute
    //    script be true. Otherwise, let it be false.

    // NOSPEC: We don't support scripting so we don't need to worry about this
    bool willExecuteScript = false;

    // 8. If will execute script is true, then:

    if (willExecuteScript) {
        // 1. Increment document's throw-on-dynamic-markup-insertion counter.

        // 2. If the JavaScript execution context stack is empty,
        //    then perform a microtask checkpoint.

        // 3. Push a new element queue onto document's relevant agent's
        //    custom element reactions stack.
    }

    // 9. Let element be the result of creating an element given document,
    //    localName, given namespace, null, and is. If will execute script
    //    is true, set the synchronous custom elements flag; otherwise,
    //    leave it unset.
    auto el = makeStrong<Dom::Element>(TagName::make(t.name, ns));

    // 10. Append each attribute in the given token to element.
    for (auto &[name, value] : t.attrs) {
        el->setAttribute(AttrName::make(name, ns), value);
    }

    // 11. If will execute script is true, then:
    if (willExecuteScript) {
        // 1. Let queue be the result of popping from document's relevant
        //    agent's custom element reactions stack. (This will be the
        //    same element queue as was pushed above.)

        // 2. Invoke custom element reactions in queue.

        // 3. Decrement document's throw-on-dynamic-markup-insertion counter.
    }

    // 12. If element has an xmlns attribute in the XMLNS namespace whose
    //     value is not exactly the same as the element's namespace, that
    //     is a parse error. Similarly, if element has an xmlns:xlink
    //     attribute in the XMLNS namespace whose value is not the XLink
    //     Namespace, that is a parse error.

    // 13. If element is a resettable element, invoke its reset algorithm.
    //     (This initializes the element's value and checkedness based on the element's attributes.)

    // 14. If element is a form-associated element and not a form-associated
    //     custom element, the form element pointer is not null, there is no
    //     template element on the stack of open elements, element is either
    //     not listed or doesn't have a form attribute, and the intended parent
    //     is in the same tree as the element pointed to by the form element pointer,

    //     Then associate element with the form element pointed to by the form
    //     element pointer and set element's parser inserted flag.

    // 15. Return element.
    return el;
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-foreign-element

static Strong<Dom::Element> insertAForeignElement(Parser &b, Token const &t, Ns ns, bool onlyAddToElementStack = false) {
    // 1. Let the adjusted insertion location be the appropriate place for inserting a node.
    auto location = apropriatePlaceForInsertingANode(b);

    // 2. Let element be the result of creating an element for the token in the
    // given namespace, with the intended parent being the element in which the
    // adjusted insertion location finds itself.
    auto el = createElementFor(t, ns);

    // 3. If onlyAddToElementStack is false, then run insert an element at the adjusted insertion location with element.
    if (not onlyAddToElementStack) {
        location.insert(el);
    }

    // 4. Push element onto the stack of open elements so that it is the new current node.
    b._openElements.pushBack(el);

    // 5. Return element.
    return el;
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-an-html-element
static Strong<Dom::Element> insertHtmlElement(Parser &b, Token const &t) {
    return insertAForeignElement(b, t, Vaev::HTML, false);
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
static void insertACharacter(Parser &b, Rune c) {
    // 2. Let the adjusted insertion location be the appropriate place for inserting a node.
    auto location = apropriatePlaceForInsertingANode(b);

    // 3. If the adjusted insertion location is inside a Document node, then ignore the token.
    if (location.parent->nodeType() == Dom::NodeType::DOCUMENT)
        return;

    // 4. If there is a Text node immediately before the adjusted insertion
    //    location, then append data to that Text node's data.
    auto lastChild = location.lastChild();
    if (lastChild and (*lastChild)->nodeType() == Dom::NodeType::TEXT) {
        auto text = (*(*lastChild).cast<Dom::Text>());
        text->appendData(c);
    }

    // Otherwise, create a new Text node whose data is data and whose node
    //            document is the same as that of the element in which the
    //            adjusted insertion location finds itself, and insert the
    //            newly created node at the adjusted insertion location.
    else {
        auto text = makeStrong<Dom::Text>(""s);
        text->appendData(c);

        location.insert(text);
    }
}

static void insertACharacter(Parser &b, Token const &t) {
    // 1. Let data be the characters passed to the algorithm, or, if no characters were explicitly specified, the character of the character token being processed.
    insertACharacter(b, t.rune);
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-comment
static void insertAComment(Parser &b, Token const &t) {
    // 1. Let data be the data given in the comment token being processed.

    // 2. If position was specified, then let the adjusted insertion
    //    location be position. Otherwise, let adjusted insertion location
    //    be the appropriate place for inserting a node.

    // TODO: If position was
    auto location = apropriatePlaceForInsertingANode(b);

    // 3. Create a Comment node whose data attribute is set to data and
    //    whose node document is the same as that of the node in which
    //    the adjusted insertion location finds itself.
    auto comment = makeStrong<Dom::Comment>(t.data);

    // 4. Insert the newly created node at the adjusted insertion location.
    location.insert(comment);
}

// 13.2.6.2 MARK: Parsing elements that contain only text
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-elements-that-contain-only-text

static void parseRawTextElement(Parser &b, Token const &t) {
    insertHtmlElement(b, t);
    b._lexer._switchTo(Lexer::RAWTEXT);
    b._originalInsertionMode = b._insertionMode;
    b._switchTo(Parser::Mode::TEXT);
}

static void parseRcDataElement(Parser &b, Token const &t) {
    insertHtmlElement(b, t);
    b._lexer._switchTo(Lexer::RCDATA);
    b._originalInsertionMode = b._insertionMode;
    b._switchTo(Parser::Mode::TEXT);
}

// 13.2.6.3 MARK: Closing elements that have implied end tags
// https://html.spec.whatwg.org/multipage/parsing.html#generate-implied-end-tags
static constexpr Array IMPLIED_END_TAGS = {
    Html::DD, Html::DT, Html::LI, Html::OPTION, Html::OPTGROUP, Html::P, Html::RB, Html::RP, Html::RT, Html::RTC
};

static void generateImpliedEndTags(Parser &b, Str except = ""s) {
    while (contains(IMPLIED_END_TAGS, last(b._openElements)->tagName) and
           last(b._openElements)->tagName.name() != except) {
        b._openElements.popBack();
    }
}

// 13.2.6.4 MARK: The rules for parsing tokens in HTML content

// 13.2.6.4.1 MARK: The "initial" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode

static Dom::QuirkMode _whichQuirkMode(Token const &) {
    // NOSPEC: We assume no quirk mode
    return Dom::QuirkMode::NO;
}

void Parser::_handleInitialMode(Token const &t) {
    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    }

    // A comment token
    else if (t.type == Token::COMMENT) {
        _document->appendChild(makeStrong<Dom::Comment>(t.data));
    }

    // A DOCTYPE token
    else if (t.type == Token::DOCTYPE) {
        _document->appendChild(makeStrong<Dom::DocumentType>(
            t.name,
            t.publicIdent,
            t.systemIdent
        ));
        _document->quirkMode = _whichQuirkMode(t);
        _switchTo(Mode::BEFORE_HTML);
    }

    // Anything else
    else {
        _raise();
        _switchTo(Mode::BEFORE_HTML);
        accept(t);
    }
}

// 13.2.6.4.2 MARK: The "before html" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
void Parser::_handleBeforeHtml(Token const &t) {
    // A DOCTYPE token
    if (t.type == Token::DOCTYPE) {
        // ignore
        _raise();
    }

    // A comment token
    else if (t.type == Token::COMMENT) {
        _document->appendChild(makeStrong<Dom::Comment>(t.data));
    }

    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    }

    // A start tag whose tag name is "html"
    else if (t.type == Token::START_TAG and t.name == "html") {
        auto el = createElementFor(t, Vaev::HTML);
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
    }

    // Any other end tag
    else if (t.type == Token::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    }

    // An end tag whose tag name is one of: "head", "body", "html", "br"
    // Anything else
    else {
        auto el = makeStrong<Dom::Element>(Html::HTML);
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
        accept(t);
    }
}

// 13.2.6.4.3 MARK: The "before head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
void Parser::_handleBeforeHead(Token const &t) {
    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // Ignore the token.
    }

    // A comment token
    else if (t.type == Token::COMMENT) {
        // Insert a comment.
        insertAComment(*this, t);
    }

    // A comment token
    else if (t.type == Token::DOCTYPE) {
        // Parse error. Ignore the token.
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == Token::START_TAG and t.name == "html") {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is "head"
    else if (t.type == Token::START_TAG and t.name == "head") {
        _headElement = insertHtmlElement(*this, t);
        _switchTo(Mode::IN_HEAD);
    }

    // Anything else
    else if (t.type == Token::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    }

    // An end tag whose tag name is one of: "head", "body", "html", "br"
    // Anything else
    else {
        Token headToken;
        headToken.type = Token::START_TAG;
        headToken.name = String{"head"};
        _headElement = insertHtmlElement(*this, headToken);
        _switchTo(Mode::IN_HEAD);
        accept(t);
    }
}

// 13.2.6.4.4 MARK: The "in head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
void Parser::_handleInHead(Token const &t) {
    auto anythingElse = [&] {
        _openElements.popBack();
        _switchTo(Mode::AFTER_HEAD);
        accept(t);
    };

    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        insertACharacter(*this, t);
    }

    // A comment token
    else if (t.type == Token::COMMENT) {
        insertAComment(*this, t);
    }

    // A DOCTYPE token
    else if (t.type == Token::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == Token::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is one of: "base", "basefont", "bgsound", "link"
    else if (t.type == Token::START_TAG and (t.name == "base" or t.name == "basefont" or t.name == "bgsound" or t.name == "link")) {
        insertHtmlElement(*this, t);
        _openElements.popBack();
        // TODO: Acknowledge the token's self-closing flag, if it is set.
    }

    // A start tag whose tag name is "meta"
    else if (t.type == Token::START_TAG and (t.name == "meta")) {
        insertHtmlElement(*this, t);
        _openElements.popBack();
        // TODO: Acknowledge the token's self-closing flag, if it is set.

        // TODO: Handle handle speculative parsing
    }

    // A start tag whose tag name is "title"
    else if (t.type == Token::START_TAG and (t.name == "title")) {
        parseRcDataElement(*this, t);
    }

    // A start tag whose tag name is "noscript", if the scripting flag is enabled
    // A start tag whose tag name is one of: "noframes", "style"
    else if (t.type == Token::START_TAG and ((t.name == "noscript" and _scriptingEnabled) or t.name == "noframe" or t.name == "style")) {
        parseRawTextElement(*this, t);
    }

    // A start tag whose tag name is "noscript", if the scripting flag is disabled
    else if (t.type == Token::START_TAG and (t.name == "noscript" and not _scriptingEnabled)) {
        insertHtmlElement(*this, t);
        _switchTo(Mode::IN_HEAD_NOSCRIPT);
    }

    // A start tag whose tag name is "script"
    else if (t.type == Token::START_TAG and (t.name == "script")) {
        // 1. Let the adjusted insertion location be the appropriate place for inserting a node.
        auto localtion = apropriatePlaceForInsertingANode(*this);

        // 2. Create an element for the token in the HTML namespace, with
        //    the intended parent being the element in which the adjusted
        //    insertion location finds itself.
        auto el = createElementFor(t, Vaev::HTML);

        // 3. Set the element's parser document to the Document, and set
        //    the element's force async to false.

        // NOSPEC: We don't support async scripts

        // NOTE: This ensures that, if the script is external,
        //       any document.write() calls in the script will execute
        //       in-line, instead of blowing the document away, as would
        //       happen in most other cases. It also prevents the script
        //       from executing until the end tag is seen.

        // 4. If the parser was created as part of the HTML fragment
        //    parsing algorithm, then set the script element's already
        //    started to true. (fragment case)

        // NOSPEC: We don't support fragments

        // 5. If the parser was invoked via the document.write() or
        //    document.writeln() methods, then optionally set the script
        //    element's already started to true. (For example, the user
        //    agent might use this clause to prevent execution of
        //    cross-origin scripts inserted via document.write() under
        //    slow network conditions, or when the page has already taken
        //    a long time to load.)

        // NOSPEC: We don't support document.write()

        // 6. Insert the newly created element at the adjusted insertion location.
        localtion.insert(el);

        // 7. Push the element onto the stack of open elements so that it is the new current node.
        _openElements.pushBack(el);

        // 8. Switch the tokenizer to the script data state.
        _lexer._switchTo(Lexer::SCRIPT_DATA);

        // 9. Let the original insertion mode be the current insertion mode.
        _originalInsertionMode = _insertionMode;

        // 10. Switch the insertion mode to "text".
        _switchTo(Mode::TEXT);
    } else if (t.type == Token::END_TAG and (t.name == "head")) {
        _openElements.popBack();
        _switchTo(Mode::AFTER_HEAD);
    } else if (t.type == Token::END_TAG and (t.name == "body" or t.name == "html" or t.name == "br")) {
        anythingElse();
    } else if (t.type == Token::START_TAG and (t.name == "template")) {
        // NOSPEC: We don't support templates
    } else if (t.type == Token::END_TAG and (t.name == "template")) {
        // NOSPEC: We don't support templates
    } else if ((t.type == Token::START_TAG and (t.name == "head")) or t.type == Token::END_TAG) {
        // ignore
        _raise();
    } else {
        anythingElse();
    }
}

// 13.2.6.4.5 MARK: The "in head noscript" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
void Parser::_handleInHeadNoScript(Token const &t) {
    auto anythingElse = [&] {
        _raise();
        _openElements.popBack();
        _switchTo(Mode::IN_HEAD);
        accept(t);
    };

    // A DOCTYPE token
    if (t.type == Token::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == Token::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // An end tag whose tag name is "noscript"
    else if (t.type == Token::END_TAG and (t.name == "noscript")) {
        _openElements.popBack();
        _switchTo(Mode::IN_HEAD);
    }

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION,
    //   - U+000A LINE FEED (LF),
    //   - U+000C FORM FEED (FF),
    //   - U+000D CARRIAGE RETURN (CR),
    //   - U+0020 SPACE
    // A comment token
    // A start tag whose tag name is one of: "basefont", "bgsound", "link", "meta", "noframes", "style"
    else if (
        (t.type == Token::CHARACTER and
         (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == ' ')) or
        t.type == Token::COMMENT or
        (t.type == Token::START_TAG and
         (t.name == "basefont" or t.name == "bgsound" or t.name == "link" or t.name == "meta" or t.name == "noframes" or t.name == "style"))
    ) {
        _acceptIn(Mode::IN_HEAD, t);
    }

    // An end tag whose tag name is "br"
    else if (t.type == Token::END_TAG and (t.name == "br")) {
        anythingElse();
    }

    // A start tag whose tag name is one of: "head", "noscript"
    // Any other end tag
    else if (
        (t.type == Token::START_TAG and (t.name == "head" or t.name == "noscript")) or
        t.type == Token::END_TAG
    ) {
        // ignore
        _raise();
    }

    // Anything else
    else {
        anythingElse();
    }
}

// 13.2.6.4.6 MARK: The "after head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
void Parser::_handleAfterHead(Token const &t) {
    auto anythingElse = [&] {
        Token bodyToken;
        bodyToken.type = Token::START_TAG;
        bodyToken.name = String{"body"};
        insertHtmlElement(*this, bodyToken);
        _switchTo(Mode::IN_BODY);
        accept(t);
    };

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION,
    //   - U+000A LINE FEED (LF),
    //   - U+000C FORM FEED (FF),
    //   - U+000D CARRIAGE RETURN (CR)
    //   - U+0020 SPACE
    if (t.type == Token::CHARACTER and
        (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) {
        insertACharacter(*this, t.rune);
    }

    // A comment token
    else if (t.type == Token::COMMENT) {
        insertAComment(*this, t);
    }

    // A DOCTYPE token
    else if (t.type == Token::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == Token::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is "body"
    else if (t.type == Token::START_TAG and (t.name == "body")) {
        insertHtmlElement(*this, t);
        _framesetOk = false;
        _switchTo(Mode::IN_BODY);
    }

    // A start tag whose tag name is "frameset"
    else if (t.type == Token::START_TAG and (t.name == "frameset")) {
        insertHtmlElement(*this, t);
        _switchTo(Mode::IN_FRAMESET);
    }

    // A start tag whose tag name is one of:
    //   "base", "basefont", "bgsound", "link", "meta",
    //   "noframes", "script", "style", "template", "title"
    else if (
        t.type == Token::START_TAG and
        (t.name == "base" or t.name == "basefont" or
         t.name == "bgsound" or t.name == "link" or
         t.name == "meta" or t.name == "noframes" or
         t.name == "script" or t.name == "style" or
         t.name == "template" or t.name == "title")
    ) {
        _raise();
        _openElements.pushBack(*_headElement);
        _acceptIn(Mode::IN_HEAD, t);
        _openElements.removeAll(*_headElement);
    }

    // An end tag whose tag name is "template"
    else if (t.type == Token::END_TAG and (t.name == "template")) {
        _acceptIn(Mode::IN_HEAD, t);
    }

    // An end tag whose tag name is one of: "body", "html", "br"
    else if (t.type == Token::END_TAG and (t.name == "body" or t.name == "html" or t.name == "br")) {
        anythingElse();
    }

    // A start tag whose tag name is "head"
    else if (t.type == Token::END_TAG or (t.type == Token::START_TAG and t.name == "head")) {
        // ignore
        _raise();
    }

    // Anything else
    else {
        anythingElse();
    }
}

// 13.2.6.4.7 MARK: The "in body" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
void Parser::_handleInBody(Token const &t) {
    // A character token that is U+0000 NULL
    if (t.type == Token::CHARACTER and t.rune == '\0') {
        _raise();
    }

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION
    //   - U+000A LINE FEED (LF)
    //   - U+000C FORM FEED (FF)
    //   - U+000D CARRIAGE RETURN (CR)
    //   - U+0020 SPACE
    else if (t.type == Token::CHARACTER and (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) {
        reconstructActiveFormattingElements(*this);
        insertACharacter(*this, t);
    }

    // Any other character token
    else if (t.type == Token::CHARACTER) {
        reconstructActiveFormattingElements(*this);
        insertACharacter(*this, t);
        _framesetOk = false;
    }

    // A comment token
    else if (t.type == Token::COMMENT) {
        insertAComment(*this, t);
    }

    // A DOCTYPE token
    else if (t.type == Token::DOCTYPE) {
        _raise();
    }

    // TODO: A start tag whose tag name is "html"

    // TODO: A start tag whose tag name is one of: "base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"
    // An end tag whose tag name is "template"

    // TODO: A start tag whose tag name is "body"

    // TODO: A start tag whose tag name is "frameset"

    // TODO: An end-of-file token

    // TODO: An end tag whose tag name is "body"

    // TODO: An end tag whose tag name is "html"

    // TODO: A start tag whose tag name is one of:
    // "address", "article", "aside", "blockquote", "center",
    // "details", "dialog", "dir", "div", "dl", "fieldset",
    // "figcaption", "figure", "footer", "header", "hgroup",
    // "main", "menu", "nav", "ol", "p", "search", "section",
    // "summary", "ul"

    // TODO: A start tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"

    // TODO: A start tag whose tag name is one of: "pre", "listing"

    // TODO: A start tag whose tag name is "form"

    // TODO: A start tag whose tag name is "li"

    // TODO: A start tag whose tag name is one of: "dd", "dt"

    // TODO: A start tag whose tag name is "plaintext"

    // TODO: A start tag whose tag name is "button"

    // TODO: An end tag whose tag name is one of: "address", "article", "aside", "blockquote", "button", "center", "details", "dialog", "dir", "div", "dl", "fieldset", "figcaption", "figure", "footer", "header", "hgroup", "listing", "main", "menu", "nav", "ol", "pre", "search", "section", "summary", "ul"

    // TODO: An end tag whose tag name is "form"

    // TODO: An end tag whose tag name is "p"

    // TODO: An end tag whose tag name is "li"

    // TODO: An end tag whose tag name is one of: "dd", "dt"

    // TODO: An end tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"

    // TODO: An end tag whose tag name is "sarcasm"
    //       This state machine is not equipped to handle sarcasm
    //       So we just ignore it

    // TODO: A start tag whose tag name is "a"

    // TODO: A start tag whose tag name is one of: "b", "big", "code", "em", "font", "i", "s", "small", "strike", "strong", "tt", "u"

    // TODO: A start tag whose tag name is "nobr"

    // TODO: An end tag whose tag name is one of: "a", "b", "big", "code", "em", "font", "i", "nobr", "s", "small", "strike", "strong", "tt", "u"

    // TODO: A start tag whose tag name is one of: "applet", "marquee", "object"

    // TODO: An end tag token whose tag name is one of: "applet", "marquee", "object"

    // TODO: A start tag whose tag name is "table"

    // TODO: An end tag whose tag name is "br"

    // TODO: A start tag whose tag name is one of: "area", "br", "embed", "img", "keygen", "wbr"

    // TODO: A start tag whose tag name is "input"

    // TODO: A start tag whose tag name is one of: "param", "source", "track"

    // TODO: A start tag whose tag name is "hr"

    // TODO: A start tag whose tag name is "image"

    // TODO: A start tag whose tag name is "textarea"

    // TODO: A start tag whose tag name is "xmp"

    // TODO: A start tag whose tag name is "iframe"

    // TODO: A start tag whose tag name is "noembed"
    // A start tag whose tag name is "noscript", if the scripting flag is enabled

    // TODO: A start tag whose tag name is "select"

    // TODO: A start tag whose tag name is one of: "optgroup", "option"

    // TODO: A start tag whose tag name is one of: "rb", "rtc"

    // TODO: A start tag whose tag name is one of: "rp", "rt"

    // TODO: A start tag whose tag name is "math"

    // TODO: A start tag whose tag name is "svg"

    // TODO: A start tag whose tag name is one of: "caption", "col", "colgroup", "frame", "head", "tbody", "td", "tfoot", "th", "thead", "tr"

    else if (t.type == Token::START_TAG) {
        reconstructActiveFormattingElements(*this);
        insertHtmlElement(*this, t);
    }

    // TODO: Any other end tag
    else if (t.type == Token::END_TAG) {
        // loop:
        while (true) {
            // 1. Initialize node to be the current node (the bottommost node of the stack).
            auto node = last(_openElements);

            // 2. Loop: If node is an HTML element with the same tag name as the token, then:
            if (node->tagName.name() == t.name) {
                // 1. Generate implied end tags, except for HTML elements with the same tag name as the token.
                generateImpliedEndTags(*this, t.name);

                // 2. If node is not the current node, then this is a parse error.
                if (node != last(_openElements)) {
                    _raise();
                }

                // 3. Pop all the nodes from the current node up to node, including node, then stop these steps
                while (last(_openElements) != node) {
                    _openElements.popBack();
                }
                _openElements.popBack();
                break;
            }

            // 3. Otherwise, if node is in the special category,
            //    then this is a parse error; ignore the token, and return.

            // 4. Set node to the previous entry in the stack of open elements.

            // 5. Return to the step labeled loop.
        }
    }
}

// 13.2.6.4.8 MARK: The "text" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incdata
void Parser::_handleText(Token const &t) {
    // A character token
    if (t.type == Token::CHARACTER) {
        insertACharacter(
            *this,
            t.rune == '\0'
                ? 0xFFFD
                : t.rune
        );
    }

    else if (t.type == Token::END_OF_FILE) {
        _raise();

        // TODO: If the current node is a script element, then set its already started to true.

        _openElements.popBack();
        _switchTo(_originalInsertionMode);
    }

    // An end tag whose tag name is "script"
    // else if (t.type == Token::END_TAG and t.name == "script") {
    // }
    // NOSPEC: We handle script end tags like any other end tag

    // Any other end tag
    else if (t.type == Token::END_TAG) {
        this->_openElements.popBack();
        _switchTo(_originalInsertionMode);
    }

    // FIXME: Implement the rest of the rules
}

void Parser::_switchTo(Mode mode) {
    _insertionMode = mode;
}

void Parser::_acceptIn(Mode mode, Token const &t) {
    logDebug("Parsing {} in {}", t, mode);

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

    case Mode::IN_HEAD:
        _handleInHead(t);
        break;

    case Mode::IN_HEAD_NOSCRIPT:
        _handleInHeadNoScript(t);
        break;

    case Mode::AFTER_HEAD:
        _handleAfterHead(t);
        break;

    case Mode::IN_BODY:
        _handleInBody(t);
        break;

    case Mode::TEXT:
        _handleText(t);
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
    case Mode::IN_TABLE: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intabletext
    case Mode::IN_TABLE_TEXT: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incaption
    case Mode::IN_CAPTION: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incolumngroup
    case Mode::IN_COLUMN_GROUP: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intablebody
    case Mode::IN_TABLE_BODY: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inrow
    case Mode::IN_ROW: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incell
    case Mode::IN_CELL: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselect
    case Mode::IN_SELECT: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselectintable
    case Mode::IN_SELECT_IN_TABLE: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intemplate
    case Mode::IN_TEMPLATE: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-body-insertion-mode
    case Mode::AFTER_BODY: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-in-frameset-insertion-mode
    case Mode::IN_FRAMESET: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-frameset-insertion-mode
    case Mode::AFTER_FRAMESET: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-body-insertion-mode
    case Mode::AFTER_AFTER_BODY: {
        break;
    }

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-frameset-insertion-mode
    case Mode::AFTER_AFTER_FRAMESET: {
        break;
    }

    default:
        break;
    }
}

void Parser::accept(Token const &t) {
    _acceptIn(_insertionMode, t);
}

} // namespace Vaev::Html
