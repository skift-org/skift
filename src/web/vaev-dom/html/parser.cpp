#include <karm-logger/logger.h>

#include "../comment.h"
#include "../document-type.h"
#include "parser.h"

namespace Vaev::Dom {

static constexpr bool DEBUG_HTML_PARSER = false;

// 13.2.2 MARK: Parse errors
// https://html.spec.whatwg.org/multipage/parsing.html#parse-errors

void HtmlParser::_raise(Str msg) {
    logError("{}: {}", _insertionMode, msg);
}

// 13.2.4.2 MARK: The stack of open elements
// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements

// https://html.spec.whatwg.org/multipage/parsing.html#special
bool HtmlParser::_isSpecial(TagName const& tag) {
    static constexpr Array SPECIAL{
#define SPECIAL(NAME) NAME,
#include "../defs/ns-html-special.inc"
#undef SPECIAL
    };

    return contains(SPECIAL, tag);
}

// https://html.spec.whatwg.org/multipage/parsing.html#mathml-text-integration-point
bool HtmlParser::_isMathMlTextIntegrationPoint(Element const& el) {
    // A node is a MathML text integration point if it is one of the following elements:
    Array const MATHML_TEXT_INTEGRATION_POINT{
        MathMl::MI,
        MathMl::MO,
        MathMl::MN,
        MathMl::MS,
        MathMl::MTEXT,
    };

    // - A MathML mi element
    // - A MathML mo element
    // - A MathML mn element
    // - A MathML ms element
    // - A MathML mtext element
    return contains(MATHML_TEXT_INTEGRATION_POINT, el.tagName);
}

// https://html.spec.whatwg.org/multipage/parsing.html#html-integration-point
bool HtmlParser::_isHtmlIntegrationPoint(Element const& el) {
    // A node is an HTML integration point if it is one of the following elements:

    // - A MathML annotation-xml element whose start tag token had an attribute with the name "encoding" whose value was an ASCII case-insensitive match for the string "text/html"
    // - A MathML annotation-xml element whose start tag token had an attribute with the name "encoding" whose value was an ASCII case-insensitive match for the string "application/xhtml+xml"
    if (el.tagName == MathMl::ANNOTATION_XML and
        (el.getAttribute(MathMl::ENCODING_ATTR) == "text/html" or
         el.getAttribute(MathMl::ENCODING_ATTR) == "application/xhtml+xml")) {
        return true;
    }

    // - An SVG foreignObject element
    // - An SVG desc element
    // - An SVG title element
    Array const HTML_INTEGRATION_POINT{
        Svg::FOREIGN_OBJECT,
        Svg::DESC,
        Svg::TITLE,
    };

    return contains(HTML_INTEGRATION_POINT, el.tagName);
}

// 13.2.4.3 MARK: The list of active formatting elements
// https://html.spec.whatwg.org/multipage/parsing.html#list-of-active-formatting-elements

void HtmlParser::_reconstructActiveFormattingElements() {
    // TODO
}

// 13.2.5 MARK: Tokenization
// https://html.spec.whatwg.org/multipage/parsing.html#tokenization

// https://html.spec.whatwg.org/multipage/parsing.html#acknowledge-self-closing-flag
void HtmlParser::_acknowledgeSelfClosingFlag(HtmlToken const&) {
    logDebugIf(DEBUG_HTML_PARSER, "acknowledgeSelfClosingFlag not implemented");
}

// 13.2.6 MARK: Tree construction
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

// 13.2.6.1 MARK: Creating and inserting nodes
// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes

// https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
HtmlParser::AdjustedInsertionLocation HtmlParser::_apropriatePlaceForInsertingANode(Gc::Ptr<Element> overrideTarget) {
    // 1. If there was an override target specified, then let target be
    //    the override target.
    //
    //    Otherwise, let target be the current node.
    auto target = overrideTarget
                      ? overrideTarget
                      : _currentElement();

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
Gc::Ref<Element> HtmlParser::_createElementFor(HtmlToken const& t, Ns ns) {
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
    auto tag = TagName::make(t.name, ns);
    logDebugIf(DEBUG_HTML_PARSER, "Creating element: {} {}", t.name, tag);
    auto el = _heap.alloc<Element>(tag);

    // 10. Append each attribute in the given token to element.
    for (auto& [name, value] : t.attrs) {
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
Gc::Ref<Element> HtmlParser::_insertAForeignElement(HtmlToken const& t, Ns ns, bool onlyAddToElementStack) {
    // 1. Let the adjusted insertion location be the appropriate place for inserting a node.
    auto location = _apropriatePlaceForInsertingANode();

    // 2. Let element be the result of creating an element for the token in the
    // given namespace, with the intended parent being the element in which the
    // adjusted insertion location finds itself.
    auto el = _createElementFor(t, ns);

    // 3. If onlyAddToElementStack is false, then run insert an element at the adjusted insertion location with element.
    if (not onlyAddToElementStack) {
        location.insert(el);
    }

    // 4. Push element onto the stack of open elements so that it is the new current node.
    _openElements.pushBack(el);

    // 5. Return element.
    return el;
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-an-html-element
Gc::Ref<Element> HtmlParser::_insertHtmlElement(HtmlToken const& t) {
    return _insertAForeignElement(t, Vaev::HTML, false);
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
void HtmlParser::_insertACharacter(Rune c) {
    // 2. Let the adjusted insertion location be the appropriate place for inserting a node.
    auto location = _apropriatePlaceForInsertingANode();

    // 3. If the adjusted insertion location is inside a Document node, then ignore the token.
    if (location.parent->nodeType() == NodeType::DOCUMENT)
        return;

    // 4. If there is a Text node immediately before the adjusted insertion
    //    location, then append data to that Text node's data.
    auto lastChild = location.lastChild();
    if (lastChild and lastChild->nodeType() == NodeType::TEXT) {
        auto text = lastChild->is<Text>();
        text->appendData(c);
    }

    // Otherwise, create a new Text node whose data is data and whose node
    //            document is the same as that of the element in which the
    //            adjusted insertion location finds itself, and insert the
    //            newly created node at the adjusted insertion location.
    else {
        auto text = _heap.alloc<Text>(""s);
        text->appendData(c);
        location.insert(text);
    }
}

// https://html.spec.whatwg.org/multipage/parsing.html#insert-a-comment
void HtmlParser::_insertAComment(HtmlToken const& t) {
    // 1. Let data be the data given in the comment token being processed.

    // 2. If position was specified, then let the adjusted insertion
    //    location be position. Otherwise, let adjusted insertion location
    //    be the appropriate place for inserting a node.

    // TODO: If position was
    auto location = _apropriatePlaceForInsertingANode();

    // 3. Create a Comment node whose data attribute is set to data and
    //    whose node document is the same as that of the node in which
    //    the adjusted insertion location finds itself.
    auto comment = _heap.alloc<Comment>(t.data);

    // 4. Insert the newly created node at the adjusted insertion location.
    location.insert(comment);
}

// https://html.spec.whatwg.org/multipage/parsing.html#reset-the-insertion-mode-appropriately
void HtmlParser::_resetTheInsertionModeAppropriately() {
    // 1. Let last be false.
    bool _last = false;

    // 2. Let node be the last node in the stack of open elements.
    // 3. Loop: If node is the first node in the stack of open elements, then set last to true, and,
    // if the parser was created as part of the HTML fragment parsing algorithm (fragment case),
    // set node to the context element passed to that algorithm.

    auto nodeIdx = _openElements.len() - 1;
    while (true) {
        auto node = _openElements[nodeIdx];

        if (nodeIdx == 0)
            _last = true;

        // 4. If node is a select element, run these substeps:
        if (node->tagName == Html::SELECT) {
            // 4.1 If last is true, jump to the step below labeled done.
            if (_last) {
                _switchTo(HtmlParser::Mode::IN_SELECT);
                return;
            }

            // 4.2 Let ancestor be node.
            auto ancestorIdx = _openElements.len() - 1;

            // 4.3 Loop: If ancestor is the first node in the stack of open elements, jump to the step below labeled done.
            while (ancestorIdx != 0) {
                // 4.4 Let ancestor be the node before ancestor in the stack of open elements.
                ancestorIdx--;

                // 4.5 If ancestor is a template node, jump to the step below labeled done.
                if (_openElements[ancestorIdx]->tagName == Html::SELECT)
                    break;

                // 4.6 If ancestor is a table node, switch the insertion mode to "in select in table" and return.
                if (_openElements[ancestorIdx]->tagName == Html::TABLE) {
                    _switchTo(HtmlParser::Mode::IN_SELECT_IN_TABLE);
                    return;
                }

                // 4.7 Jump back to the step labeled loop.
            }

            // 4.8 Done: Switch the insertion mode to "in select" and return.]
            _switchTo(HtmlParser::Mode::IN_SELECT);
            return;
        }

        // 5. If node is a td or th element and last is false, then switch the insertion mode to "in cell" and return.
        if ((node->tagName == Html::TD or node->tagName == Html::TH) and not _last) {
            _switchTo(HtmlParser::Mode::IN_CELL);
            return;
        }

        // 6. If node is a tr element, then switch the insertion mode to "in row" and return.
        if (node->tagName == Html::TR) {
            _switchTo(HtmlParser::Mode::IN_ROW);
            return;
        }

        // 7. If node is a tbody, thead, or tfoot element, then switch the insertion mode to "in table body" and return.
        if (node->tagName == Html::TBODY or node->tagName == Html::THEAD or node->tagName == Html::TFOOT) {
            _switchTo(HtmlParser::Mode::IN_TABLE_BODY);
            return;
        }

        // 8. If node is a caption element, then switch the insertion mode to "in caption" and return.
        if (node->tagName == Html::CAPTION) {
            _switchTo(HtmlParser::Mode::IN_CAPTION);
            return;
        }

        // 9. If node is a colgroup element, then switch the insertion mode to "in column group" and return.
        if (node->tagName == Html::COLGROUP) {
            _switchTo(HtmlParser::Mode::IN_COLUMN_GROUP);
            return;
        }

        // 10. If node is a table element, then switch the insertion mode to "in table" and return.
        if (node->tagName == Html::TABLE) {
            _switchTo(HtmlParser::Mode::IN_TABLE);
            return;
        }

        // 11. If node is a template element, then switch the insertion mode to the current template insertion mode and return.

        // 12. If node is a head element and last is false, then switch the insertion mode to "in head" and return.
        if (node->tagName == Html::HEAD and not _last) {
            _switchTo(HtmlParser::Mode::IN_HEAD);
            return;
        }

        // 13. If node is a body element, then switch the insertion mode to "in body" and return.
        if (node->tagName == Html::BODY) {
            _switchTo(HtmlParser::Mode::IN_BODY);
            return;
        }

        // 14. If node is a frameset element, then switch the insertion mode to "in frameset" and return. (fragment case)
        if (node->tagName == Html::FRAMESET) {
            _switchTo(HtmlParser::Mode::IN_FRAMESET);
            return;
        }

        // 15. If node is an html element, run these substeps:
        if (node->tagName == Html::HTML) {
            // 15.1 If the head element pointer is null, switch the insertion mode to "before head" and return. (fragment case)
            if (not _headElement)
                _switchTo(HtmlParser::Mode::BEFORE_HEAD);

            // 15.2 Otherwise, the head element pointer is not null, switch the insertion mode to "after head" and return.
            else
                _switchTo(HtmlParser::Mode::AFTER_HEAD);

            return;
        }

        // 16. If last is true, then switch the insertion mode to "in body" and return. (fragment case)
        if (_last)
            _switchTo(HtmlParser::Mode::IN_BODY);

        // 17. Let node now be the node before node in the stack of open elements.
        nodeIdx--;

        // 18. Return to the step labeled loop.
    }
}

// 13.2.6.2 MARK: Parsing elements that contain only text
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-elements-that-contain-only-text

void HtmlParser::_parseRawTextElement(HtmlToken const& t) {
    _insertHtmlElement(t);
    _lexer._switchTo(HtmlLexer::RAWTEXT);
    _originalInsertionMode = _insertionMode;
    _switchTo(HtmlParser::Mode::TEXT);
}

void HtmlParser::_parseRcDataElement(HtmlToken const& t) {
    _insertHtmlElement(t);
    _lexer._switchTo(HtmlLexer::RCDATA);
    _originalInsertionMode = _insertionMode;
    _switchTo(HtmlParser::Mode::TEXT);
}

// 13.2.6.3 MARK: Closing elements that have implied end tags
// https://html.spec.whatwg.org/multipage/parsing.html#generate-implied-end-tags
static constexpr Array IMPLIED_END_TAGS = {
    Html::DD, Html::DT, Html::LI, Html::OPTION, Html::OPTGROUP, Html::P, Html::RB, Html::RP, Html::RT, Html::RTC
};

static void _generateImpliedEndTags(HtmlParser& b, Opt<TagName> except = NONE) {
    while (contains(IMPLIED_END_TAGS, b._currentElement()->tagName) and
           b._currentElement()->tagName != except) {
        b._openElements.popBack();
    }
}

// 13.2.6.4 MARK: The rules for parsing tokens in HTML content

// 13.2.6.4.1 MARK: The "initial" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode

static QuirkMode _whichQuirkMode(HtmlToken const&) {
    // NOSPEC: We assume no quirk mode
    return QuirkMode::NO;
}

void HtmlParser::_handleInitialMode(HtmlToken const& t) {
    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        _document->appendChild(_heap.alloc<Comment>(t.data));
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _document->appendChild(
            _heap.alloc<DocumentType>(
                t.name,
                t.publicIdent,
                t.systemIdent
            )
        );
        _document->quirkMode = _whichQuirkMode(t);
        _switchTo(Mode::BEFORE_HTML);
    }

    // Anything else
    else {
        _switchTo(Mode::BEFORE_HTML);
        accept(t);
    }
}

// 13.2.6.4.2 MARK: The "before html" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
void HtmlParser::_handleBeforeHtml(HtmlToken const& t) {
    // A DOCTYPE token
    if (t.type == HtmlToken::DOCTYPE) {
        // ignore
        _raise();
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        _document->appendChild(_heap.alloc<Comment>(t.data));
    }

    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // ignore
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and t.name == "html") {
        auto el = _createElementFor(t, Vaev::HTML);
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
    }

    // Any other end tag
    else if (t.type == HtmlToken::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    }

    // An end tag whose tag name is one of: "head", "body", "html", "br"
    // Anything else
    else {
        auto el = _heap.alloc<Element>(Html::HTML);
        _document->appendChild(el);
        _openElements.pushBack(el);
        _switchTo(Mode::BEFORE_HEAD);
        accept(t);
    }
}

// 13.2.6.4.3 MARK: The "before head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
void HtmlParser::_handleBeforeHead(HtmlToken const& t) {
    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        // Ignore the token.
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        // Insert a comment.
        _insertAComment(t);
    }

    // A comment token
    else if (t.type == HtmlToken::DOCTYPE) {
        // Parse error. Ignore the token.
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and t.name == "html") {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is "head"
    else if (t.type == HtmlToken::START_TAG and t.name == "head") {
        _headElement = _insertHtmlElement(t);
        _switchTo(Mode::IN_HEAD);
    }

    // Anything else
    else if (t.type == HtmlToken::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
        // ignore
        _raise();
    }

    // An end tag whose tag name is one of: "head", "body", "html", "br"
    // Anything else
    else {
        HtmlToken headToken;
        headToken.type = HtmlToken::START_TAG;
        headToken.name = String{"head"};
        _headElement = _insertHtmlElement(headToken);
        _switchTo(Mode::IN_HEAD);
        accept(t);
    }
}

// 13.2.6.4.4 MARK: The "in head" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
void HtmlParser::_handleInHead(HtmlToken const& t) {
    auto anythingElse = [&] {
        _openElements.popBack();
        _switchTo(Mode::AFTER_HEAD);
        accept(t);
    };

    // A character token that is one of U+0009 CHARACTER TABULATION,
    // U+000A LINE FEED (LF), U+000C FORM FEED (FF),
    // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or
         t.rune == '\n' or
         t.rune == '\f' or
         t.rune == ' ')) {
        _insertACharacter(t.rune);
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        _insertAComment(t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is one of: "base", "basefont", "bgsound", "link"
    else if (t.type == HtmlToken::START_TAG and (t.name == "base" or t.name == "basefont" or t.name == "bgsound" or t.name == "link")) {
        _insertHtmlElement(t);
        _openElements.popBack();
        _acknowledgeSelfClosingFlag(t);
    }

    // A start tag whose tag name is "meta"
    else if (t.type == HtmlToken::START_TAG and (t.name == "meta")) {
        _insertHtmlElement(t);
        _openElements.popBack();
        _acknowledgeSelfClosingFlag(t);
        // TODO: Handle handle speculative parsing
    }

    // A start tag whose tag name is "title"
    else if (t.type == HtmlToken::START_TAG and (t.name == "title")) {
        _parseRcDataElement(t);
    }

    // A start tag whose tag name is "noscript", if the scripting flag is enabled
    // A start tag whose tag name is one of: "noframes", "style"
    else if (t.type == HtmlToken::START_TAG and ((t.name == "noscript" and _scriptingEnabled) or t.name == "noframe" or t.name == "style")) {
        _parseRawTextElement(t);
    }

    // A start tag whose tag name is "noscript", if the scripting flag is disabled
    else if (t.type == HtmlToken::START_TAG and (t.name == "noscript" and not _scriptingEnabled)) {
        _insertHtmlElement(t);
        _switchTo(Mode::IN_HEAD_NOSCRIPT);
    }

    // A start tag whose tag name is "script"
    else if (t.type == HtmlToken::START_TAG and (t.name == "script")) {
        // 1. Let the adjusted insertion location be the appropriate place for inserting a node.
        auto localtion = _apropriatePlaceForInsertingANode();

        // 2. Create an element for the token in the HTML namespace, with
        //    the intended parent being the element in which the adjusted
        //    insertion location finds itself.
        auto el = _createElementFor(t, Vaev::HTML);

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
        _lexer._switchTo(HtmlLexer::SCRIPT_DATA);

        // 9. Let the original insertion mode be the current insertion mode.
        _originalInsertionMode = _insertionMode;

        // 10. Switch the insertion mode to "text".
        _switchTo(Mode::TEXT);
    } else if (t.type == HtmlToken::END_TAG and (t.name == "head")) {
        _openElements.popBack();
        _switchTo(Mode::AFTER_HEAD);
    } else if (t.type == HtmlToken::END_TAG and (t.name == "body" or t.name == "html" or t.name == "br")) {
        anythingElse();
    } else if (t.type == HtmlToken::START_TAG and (t.name == "template")) {
        // NOSPEC: We don't support templates
    } else if (t.type == HtmlToken::END_TAG and (t.name == "template")) {
        // NOSPEC: We don't support templates
    } else if ((t.type == HtmlToken::START_TAG and (t.name == "head")) or t.type == HtmlToken::END_TAG) {
        // ignore
        _raise();
    } else {
        anythingElse();
    }
}

// 13.2.6.4.5 MARK: The "in head noscript" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
void HtmlParser::_handleInHeadNoScript(HtmlToken const& t) {
    auto anythingElse = [&] {
        _raise();
        _openElements.popBack();
        _switchTo(Mode::IN_HEAD);
        accept(t);
    };

    // A DOCTYPE token
    if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // An end tag whose tag name is "noscript"
    else if (t.type == HtmlToken::END_TAG and (t.name == "noscript")) {
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
        (t.type == HtmlToken::CHARACTER and
         (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == ' ')) or
        t.type == HtmlToken::COMMENT or
        (t.type == HtmlToken::START_TAG and
         (t.name == "basefont" or t.name == "bgsound" or t.name == "link" or t.name == "meta" or t.name == "noframes" or t.name == "style"))
    ) {
        _acceptIn(Mode::IN_HEAD, t);
    }

    // An end tag whose tag name is "br"
    else if (t.type == HtmlToken::END_TAG and (t.name == "br")) {
        anythingElse();
    }

    // A start tag whose tag name is one of: "head", "noscript"
    // Any other end tag
    else if (
        (t.type == HtmlToken::START_TAG and (t.name == "head" or t.name == "noscript")) or
        t.type == HtmlToken::END_TAG
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
void HtmlParser::_handleAfterHead(HtmlToken const& t) {
    auto anythingElse = [&] {
        HtmlToken bodyToken;
        bodyToken.type = HtmlToken::START_TAG;
        bodyToken.name = String{"body"};
        _insertHtmlElement(bodyToken);
        _switchTo(Mode::IN_BODY);
        accept(t);
    };

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION,
    //   - U+000A LINE FEED (LF),
    //   - U+000C FORM FEED (FF),
    //   - U+000D CARRIAGE RETURN (CR)
    //   - U+0020 SPACE
    if (t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) {
        _insertACharacter(t.rune);
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        _insertAComment(t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::START_TAG and (t.name == "html")) {
        _acceptIn(Mode::IN_BODY, t);
    }

    // A start tag whose tag name is "body"
    else if (t.type == HtmlToken::START_TAG and (t.name == "body")) {
        _insertHtmlElement(t);
        _framesetOk = false;
        _switchTo(Mode::IN_BODY);
    }

    // A start tag whose tag name is "frameset"
    else if (t.type == HtmlToken::START_TAG and (t.name == "frameset")) {
        _insertHtmlElement(t);
        _switchTo(Mode::IN_FRAMESET);
    }

    // A start tag whose tag name is one of:
    //   "base", "basefont", "bgsound", "link", "meta",
    //   "noframes", "script", "style", "template", "title"
    else if (
        t.type == HtmlToken::START_TAG and
        (t.name == "base" or t.name == "basefont" or
         t.name == "bgsound" or t.name == "link" or
         t.name == "meta" or t.name == "noframes" or
         t.name == "script" or t.name == "style" or
         t.name == "template" or t.name == "title")
    ) {
        _raise();
        _openElements.pushBack(_headElement.upgrade());
        _acceptIn(Mode::IN_HEAD, t);
        _openElements.removeAll(_headElement);
    }

    // An end tag whose tag name is "template"
    else if (t.type == HtmlToken::END_TAG and (t.name == "template")) {
        _acceptIn(Mode::IN_HEAD, t);
    }

    // An end tag whose tag name is one of: "body", "html", "br"
    else if (t.type == HtmlToken::END_TAG and (t.name == "body" or t.name == "html" or t.name == "br")) {
        anythingElse();
    }

    // A start tag whose tag name is "head"
    else if (t.type == HtmlToken::END_TAG or (t.type == HtmlToken::START_TAG and t.name == "head")) {
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
void HtmlParser::_handleInBody(HtmlToken const& t) {
    auto closePElementIfInButtonScope = [&]() {
        // https://html.spec.whatwg.org/#close-a-p-element
        // If the stack of open elements has a p element in button scope, then close a p element.
        if (_hasElementInButtonScope(Html::P)) {

            // Generate implied end tags, except for p elements.
            _generateImpliedEndTags(*this, Html::P);

            // If the current node is not a p element, then this is a parse error.
            if (_currentElement()->tagName != Html::P)
                _raise();

            // Pop elements from the stack of open elements until a p element has been popped from the stack.
            while (_openElements.len() > 0) {
                auto lastEl = last(_openElements);

                _openElements.popBack();
                if (lastEl->tagName == Html::P)
                    break;
            }
        }
    };

    // A character token that is U+0000 NULL
    if (t.type == HtmlToken::CHARACTER and t.rune == '\0') {
        _raise();
    }

    // A character token that is one of
    //   - U+0009 CHARACTER TABULATION
    //   - U+000A LINE FEED (LF)
    //   - U+000C FORM FEED (FF)
    //   - U+000D CARRIAGE RETURN (CR)
    //   - U+0020 SPACE
    else if (t.type == HtmlToken::CHARACTER and (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) {
        _reconstructActiveFormattingElements();
        _insertACharacter(t.rune);
    }

    // Any other character token
    else if (t.type == HtmlToken::CHARACTER) {
        _reconstructActiveFormattingElements();
        _insertACharacter(t.rune);
        _framesetOk = false;
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        _insertAComment(t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        _raise();
    }

    // TODO: A start tag whose tag name is "html"

    // TODO: A start tag whose tag name is one of: "base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"
    // An end tag whose tag name is "template"

    // TODO: A start tag whose tag name is "body"

    // TODO: A start tag whose tag name is "frameset"

    // TODO: An end-of-file token

    // An end tag whose tag name is "body"
    // An end tag whose tag name is "html"
    else if (t.type == HtmlToken::END_TAG and (t.name == "body" or t.name == "html")) {
        // If the stack of open elements does not have a body element in
        // scope, this is a parse error; ignore the token.
        if (not _hasElementInScope(Html::BODY)) {
            _raise();
            return;
        }

        // Otherwise, if there is a node in the stack of open elements that
        // is not an implied end tag or
        // tbody element, a td element, a tfoot element, a th element, a thead element, a tr element, the body element, or the html
        // then this is a parse error.
        for (auto& el : _openElements) {
            if (not contains(IMPLIED_END_TAGS, el->tagName) and
                el->tagName != Html::TBODY and el->tagName != Html::TD and el->tagName != Html::TFOOT and
                el->tagName != Html::TH and el->tagName != Html::THEAD and el->tagName != Html::TR and
                el->tagName != Html::BODY and el->tagName != Html::HTML) {
                _raise();
                break;
            }
        }

        // Switch the insertion mode to "after body".
        _switchTo(Mode::AFTER_BODY);

        if (t.name == "html")
            accept(t);

    }

    // A start tag whose tag name is one of:
    // "address", "article", "aside", "blockquote", "center",
    // "details", "dialog", "dir", "div", "dl", "fieldset",
    // "figcaption", "figure", "footer", "header", "hgroup",
    // "main", "menu", "nav", "ol", "p", "search", "section",
    // "summary", "ul"
    else if (
        t.type == HtmlToken::START_TAG and
        (t.name == "address" or t.name == "article" or t.name == "aside" or t.name == "blockquote" or
         t.name == "center" or t.name == "details" or t.name == "dialog" or t.name == "dir" or
         t.name == "div" or t.name == "dl" or t.name == "fieldset" or t.name == "figcaption" or
         t.name == "figure" or t.name == "footer" or t.name == "header" or t.name == "hgroup" or
         t.name == "main" or t.name == "menu" or t.name == "nav" or t.name == "ol" or
         t.name == "p" or t.name == "search" or t.name == "section" or t.name == "summary" or t.name == "ul"
        )
    ) {
        // If the stack of open elements has a p element in button scope, then close a p element.
        closePElementIfInButtonScope();

        // Insert an HTML element for the token.
        _insertHtmlElement(t);
    }

    // TODO: A start tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"

    // TODO: A start tag whose tag name is one of: "pre", "listing"

    // TODO: A start tag whose tag name is "form"

    // TODO: A start tag whose tag name is "li"

    // TODO: A start tag whose tag name is one of: "dd", "dt"
    else if (t.type == HtmlToken::START_TAG and (t.name == "dd" or t.name == "dt")) {
        // 1. Set the frameset-ok flag to "not ok".
        _framesetOk = false;

        // 2. Initialize node to be the current node (the bottommost node of the stack).
        usize curr = _openElements.len();

        auto done = [&] {
            // 7. If the stack of open elements has a p element in button scope, then close a p element.

            // 8. Insert an HTML element for the token.
            _insertHtmlElement(t);
        };

        // 3. Loop:
        while (curr > 0) {
            auto tag = _openElements[curr - 1]->tagName;
            // If node is a dd element, then run these substeps:
            if (tag == Html::DD) {
                // 1. Generate implied end tags, except for dd elements.
                _generateImpliedEndTags(*this, Html::DD);

                // 2. If the current node is not a dd element, then this is a parse error.
                if (_currentElement()->tagName != Html::DD) {
                    _raise();
                }

                // 3. Pop elements from the stack of open elements until a dd element has been popped from the stack.
                while (Karm::any(_openElements) and _openElements.popBack()->tagName != Html::DD) {
                    // do nothing
                }

                // 4. Jump to the step labeled done below.
                done();
                return;
            }

            if (tag == Html::DT) {
                // 1. Generate implied end tags, except for dt elements.
                _generateImpliedEndTags(*this, Html::DT);

                // 2. If the current node is not a dt element, then this is a parse error.
                if (_currentElement()->tagName != Html::DT) {
                    _raise();
                }

                // 3. Pop elements from the stack of open elements until a dt element has been popped from the stack.
                while (Karm::any(_openElements) and _openElements.popBack()->tagName != Html::DT) {
                    // do nothing
                }

                // 4. Jump to the step labeled done below.
                done();
                return;
            }

            // 5. If node is in the special category, but is not an address,
            //    div, or p element, then jump to the step labeled done below.
            if (_isSpecial(tag) and not(tag == Html::ADDRESS or tag == Html::DIV or tag == Html::P)) {
                done();
                return;
            }

            // 6. Otherwise, set node to the previous entry in the stack of open
            //    elements and return to the step labeled loop.
            curr--;
        }
    }

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

    // A start tag whose tag name is "table"
    else if (t.type == HtmlToken::START_TAG and t.name == "table") {
        // TODO: If the Document is not set to quirks mode,
        // and the stack of open elements has a p element in button scope, then close a p element.

        // Insert an HTML element for the token.
        _insertHtmlElement(t);

        // Set the frameset-ok flag to "not ok".
        _framesetOk = false;

        // Switch the insertion mode to "in table".
        _switchTo(Mode::IN_TABLE);
    }

    // TODO: An end tag whose tag name is "br"
    // TODO: A start tag whose tag name is one of: "area", "br", "embed", "img", "keygen", "wbr"
    else if (
        t.name == "br" or
        (t.type == HtmlToken::START_TAG and
         (t.name == "area" or t.name == "br" or t.name == "embed" or t.name == "img" or t.name == "keygen" or t.name == "wbr")
        )
    ) {
        if (t.type == HtmlToken::END_TAG) {
            // Parse error.
            _raise();

            // Drop the attributes from the token, and act as described in the next entry; i.e. act as if
            // this was a "br" start tag token with no attributes, rather than the end tag token that it actually is.
            // FIXME: cannot drop attributes since token is const
        }

        // Reconstruct the active formatting elements, if any.
        _reconstructActiveFormattingElements();

        // Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.
        _insertHtmlElement(t);
        _openElements.popBack();

        // TODO: Acknowledge the token's self-closing flag, if it is set.

        // Set the frameset-ok flag to "not ok".
        _framesetOk = false;
    }

    // A start tag whose tag name is "input"
    else if (t.type == HtmlToken::START_TAG and t.name == "input") {
        // TODO: Reconstruct the active formatting elements, if any.

        // Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.
        _insertHtmlElement(t);
        _openElements.popBack();

        // Acknowledge the token's self-closing flag, if it is set.
        _acknowledgeSelfClosingFlag(t);

        // If the token does not have an attribute with the name "type",
        // or if it does, but that attribute's value is not an ASCII case-insensitive match for the string "hidden",
        bool hasHiddenAsTypeAttrValue = false;
        for (auto& [name, value] : t.attrs) {
            if (name == "type") {
                // TODO: ASCII case-insensitive match
                if (value == "hidden") {
                    hasHiddenAsTypeAttrValue = true;
                }

                break;
            }
        }

        if (not hasHiddenAsTypeAttrValue) {
            //  then: set the frameset-ok flag to "not ok".
            _framesetOk = false;
        }
    }

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

    // A start tag whose tag name is "svg"
    else if (t.type == HtmlToken::START_TAG and t.name == "svg") {
        // Reconstruct the active formatting elements, if any.
        _reconstructActiveFormattingElements();

        // TODO: Adjust SVG attributes for the token. (This fixes the case of
        // SVG attributes that are not all lowercase.)

        // TODO: Adjust foreign attributes for the token. (This fixes the use of
        // namespaced attributes, in particular XLink in SVG.)

        // Insert a foreign element for the token, with SVG namespace and false.
        _insertAForeignElement(t, Vaev::SVG, false);

        // If the token has its self-closing flag set, pop the current node
        // off the stack of open elements and acknowledge the token's self-closing flag.
        if (t.selfClosing) {
            _openElements.popBack();
            _acknowledgeSelfClosingFlag(t);
        }
    }

    // TODO: A start tag whose tag name is one of: "caption", "col", "colgroup", "frame", "head", "tbody", "td", "tfoot", "th", "thead", "tr"

    else if (t.type == HtmlToken::START_TAG) {
        _reconstructActiveFormattingElements();
        _insertHtmlElement(t);
    }

    // TODO: Any other end tag
    else if (t.type == HtmlToken::END_TAG) {
        // loop:
        usize curr = _openElements.len();
        while (curr > 0) {
            // 1. Initialize node to be the current node (the bottommost node of the stack).
            auto node = _openElements[curr - 1];

            // 2. Loop: If node is an HTML element with the same tag name as the token, then:
            if (node->tagName.name() == t.name) {
                // 1. Generate implied end tags, except for HTML elements with the same tag name as the token.
                _generateImpliedEndTags(*this, node->tagName);

                // 2. If node is not the current node, then this is a parse error.
                if (node != _currentElement())
                    _raise();

                // 3. Pop all the nodes from the current node up to node, including node, then stop these steps
                while (_currentElement() != node) {
                    _openElements.popBack();
                }
                _openElements.popBack();
                break;
            }

            // 3. Otherwise, if node is in the special category,
            //    then this is a parse error; ignore the token, and return.

            // TODO: Implement the special category

            // 4. Set node to the previous entry in the stack of open elements.
            curr--;

            // 5. Return to the step labeled loop.
        }
    }
}

// 13.2.6.4.8 MARK: The "text" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incdata
void HtmlParser::_handleText(HtmlToken const& t) {
    // A character token
    if (t.type == HtmlToken::CHARACTER) {
        _insertACharacter(
            t.rune == '\0'
                ? 0xFFFD
                : t.rune
        );
    }

    else if (t.type == HtmlToken::END_OF_FILE) {
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
    else if (t.type == HtmlToken::END_TAG) {
        this->_openElements.popBack();
        _switchTo(_originalInsertionMode);
    }

    // FIXME: Implement the rest of the rules
}

void HtmlParser::_inTableModeAnythingElse(HtmlToken const& t) {
    // Parse error.
    _raise();

    // Enable foster parenting,
    _fosterParenting = true;

    // process the token using the rules for the "in body" insertion mode,
    _acceptIn(HtmlParser::Mode::IN_BODY, t);

    // and then disable foster parenting.
    _fosterParenting = false;
}

// 13.2.6.4.9 MARK: The "in table" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
void HtmlParser::_handleInTable(HtmlToken const& t) {
    auto _clearTheStackBackToATableContext = [&]() {
        while (_currentElement()->tagName != Html::TABLE and
               _currentElement()->tagName != Html::TEMPLATE and
               _currentElement()->tagName != Html::HTML) {

            _openElements.popBack();
        }
    };

    // A character token, if the current node is table, tbody, template, tfoot, thead, or tr element
    if (t.type == HtmlToken::CHARACTER and
        (_currentElement()->tagName == Html::TABLE or _currentElement()->tagName == Html::TBODY or
         _currentElement()->tagName == Html::TEMPLATE or _currentElement()->tagName == Html::TFOOT or
         _currentElement()->tagName == Html::THEAD or _currentElement()->tagName == Html::TR
        )) {
        // Let the pending table character tokens be an empty list of tokens.
        _pendingTableCharacterTokens.clear();

        // Let the original insertion mode be the current insertion mode.
        _originalInsertionMode = _insertionMode;

        // Switch the insertion mode to "in table text" and reprocess the token.
        _switchTo(Mode::IN_TABLE_TEXT);
        accept(t);
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        // Insert a comment.
        _insertAComment(t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        // Parse error. Ignore the token.
        _raise();
    }

    // A start tag whose tag name is "caption"
    else if (t.type == HtmlToken::START_TAG and t.name == "caption") {
        // Clear the stack back to a table context. (See below.)
        _clearTheStackBackToATableContext();

        // TODO: Insert a marker at the end of the list of active formatting elements.

        // Insert an HTML element for the token, then switch the insertion mode to "in caption".
        _insertHtmlElement(t);
        _switchTo(Mode::IN_CAPTION);
    }

    // A start tag whose tag name is "colgroup"
    else if (t.type == HtmlToken::START_TAG and t.name == "colgroup") {
        // Clear the stack back to a table context. (See below.)
        _clearTheStackBackToATableContext();

        // Insert an HTML element for the token, then switch the insertion mode to "in column group".
        _insertHtmlElement(t);
        _switchTo(Mode::IN_COLUMN_GROUP);
    }

    // A start tag whose tag name is "col"
    else if (t.type == HtmlToken::START_TAG and t.name == "col") {
        // Clear the stack back to a table context. (See below.)
        _clearTheStackBackToATableContext();

        // Insert an HTML element for a "colgroup" start tag token with no attributes, then switch the insertion mode to "in column group".
        HtmlToken colGroupToken;
        colGroupToken.type = HtmlToken::START_TAG;
        colGroupToken.name = String{"colgroup"};
        _insertAForeignElement(colGroupToken, Vaev::HTML);
        _switchTo(Mode::IN_COLUMN_GROUP);

        // Reprocess the current token.
        accept(t);
    }

    // A start tag whose tag name is one of: "tbody", "tfoot", "thead"
    else if (t.type == HtmlToken::START_TAG and
             (t.name == "tbody" or t.name == "tfoot" or t.name == "thead")) {
        // Clear the stack back to a table context. (See below.)
        _clearTheStackBackToATableContext();

        // Insert an HTML element for the token, then switch the insertion mode to "in table body".
        _insertHtmlElement(t);
        _switchTo(Mode::IN_TABLE_BODY);
    }

    // A start tag whose tag name is one of: "td", "th", "tr"
    else if (t.type == HtmlToken::START_TAG and
             (t.name == "td" or t.name == "th" or t.name == "tr")) {
        // Clear the stack back to a table context. (See below.)
        _clearTheStackBackToATableContext();

        // Insert an HTML element for a "tbody" start tag token with no attributes, then switch the insertion mode to "in table body".
        HtmlToken TableBodyToken;
        TableBodyToken.type = HtmlToken::START_TAG;
        TableBodyToken.name = "tbody"s;
        _insertAForeignElement(TableBodyToken, Vaev::HTML);
        _switchTo(Mode::IN_TABLE_BODY);

        // Reprocess the current token.
        accept(t);
    }

    // A start tag whose tag name is "table"
    else if (t.type == HtmlToken::START_TAG and t.name == "table") {
        // Parse error.
        _raise();

        // If the stack of open elements does not have a table element in table scope, ignore the token.
        if (not _hasElementInTableScope(Html::TABLE))
            return;

        // Otherwise:

        // Pop elements from this stack until a table element has been popped from the stack.
        while (Karm::any(_openElements) and _openElements.popBack()->tagName != Html::TABLE) {
            // do nothing
        }

        // Reset the insertion mode appropriately.
        _resetTheInsertionModeAppropriately();

        // Reprocess the token.
        accept(t);
    }

    // An end tag whose tag name is "table"
    else if (t.type == HtmlToken::END_TAG and t.name == "table") {
        // If the stack of open elements does not have a table element in table scope, this is a parse error;
        // ignore the token.
        if (not _hasElementInTableScope(Html::TABLE)) {
            _raise();
            return;
        }

        // Pop elements from this stack until a table element has been popped from the stack.
        while (Karm::any(_openElements) and _openElements.popBack()->tagName != Html::TABLE) {
            // do nothing
        }

        // Reset the insertion mode appropriately.
        _resetTheInsertionModeAppropriately();
    }

    // An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html", "tbody", "td", "tfoot", "th", "thead", "tr"
    else if (t.type == HtmlToken::END_TAG and
             (t.name == "body" or t.name == "caption" or t.name == "col" or
              t.name == "colgroup" or t.name == "html" or t.name == "tbody" or
              t.name == "td" or t.name == "tfoot" or t.name == "th" or
              t.name == "thead" or t.name == "tr"
             )) {
        // Parse error. Ignore the token.
        _raise();
    }

    // A start tag whose tag name is one of: "style", "script", "template"
    else if (t.type == HtmlToken::START_TAG and
             (t.name == "style" or t.name == "script" or t.name == "template")) {
        // Process the token using the rules for the "in head" insertion mode.
        _acceptIn(Mode::IN_HEAD, t);
    }

    // An end tag whose tag name is "template"
    else if (t.type == HtmlToken::END_TAG and t.name == "template") {
        // Process the token using the rules for the "in head" insertion mode.
        _acceptIn(Mode::IN_HEAD, t);
    }

    // A start tag whose tag name is "input"
    else if (t.type == HtmlToken::START_TAG and t.name == "input") {

        // If the token does not have an attribute with the name "type",
        // or if it does, but that attribute's value is not an ASCII case-insensitive match for the string "hidden",
        bool hasHiddenAsTypeAttrValue = false;
        for (auto& [name, value] : t.attrs) {
            if (name == "type") {
                // TODO: ASCII case-insensitive match
                if (eqCi(value.str(), "hidden"s)) {
                    hasHiddenAsTypeAttrValue = true;
                }

                break;
            }
        }

        // then: act as described in the "anything else" entry below.
        if (hasHiddenAsTypeAttrValue) {
            _inTableModeAnythingElse(t);
            return;
        }

        // Parse error.
        _raise();

        // Insert an HTML element for the token.
        _insertHtmlElement(t);

        // Pop that input element off the stack of open elements.
        _openElements.popBack();

        // Acknowledge the token's self-closing flag, if it is set.
        _acknowledgeSelfClosingFlag(t);
    }

    // A start tag whose tag name is "form"
    else if (t.type == HtmlToken::START_TAG and t.name == "form") {
        // Parse error.
        _raise();

        // If there is a template element on the stack of open elements, or if the form element pointer is not null, ignore the token.
        for (auto& el : _openElements) {
            if (el->tagName == Html::TEMPLATE)
                return;
        }

        if (not _formElement)
            return;

        // Insert an HTML element for the token, and set the form element pointer to point to the element created.
        HtmlToken formToken;
        formToken.type = HtmlToken::START_TAG;
        formToken.name = "form"s;

        _formElement = _insertAForeignElement(formToken, Vaev::HTML);

        // Pop that form element off the stack of open elements.
        _openElements.popBack();
    }

    // An end-of-file token
    else if (t.type == HtmlToken::END_OF_FILE) {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    }

    // Anything else
    else {
        _inTableModeAnythingElse(t);
    }
}

// 13.2.6.4.10 MARK: The "in table text" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intabletext
void HtmlParser::_handleInTableText(HtmlToken const& t) {

    // A character token that is U+0000 NULL
    if (t.type == HtmlToken::CHARACTER and t.rune == '\0') {
        // Parse error. Ignore the token.
        _raise();
    }

    // Any other character token
    else if (t.type == HtmlToken::CHARACTER) {
        // Append the character token to the pending table character tokens list.
        _pendingTableCharacterTokens.pushBack(t);
    }

    else {
        // If any of the tokens in the pending table character tokens list are character tokens that are not ASCII
        // whitespace,
        // then this is a parse error:
        bool hasNonWhitespace = false;
        for (auto const& token : _pendingTableCharacterTokens) {
            if (
                token.rune != '\t' and token.rune != '\n' and
                token.rune != '\f' and token.rune != '\r' and token.rune != ' '
            ) {
                hasNonWhitespace = true;
                break;
            }
        }

        if (hasNonWhitespace) {
            // reprocess the character tokens in the pending table character tokens list using the rules given in
            // the "anything else" entry in the "in table" insertion mode.
            for (auto const& token : _pendingTableCharacterTokens) {
                _inTableModeAnythingElse(token);
            }
        } else {
            // Otherwise, insert the characters given by the pending table character tokens list.
            for (auto const& token : _pendingTableCharacterTokens) {
                _insertACharacter(token.rune);
            }
        }

        // Switch the insertion mode to the original insertion mode and reprocess the token.
        _switchTo(_originalInsertionMode);
        accept(t);
    }
}

// 13.2.6.4.13 MARK: The "in table body" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intbody
void HtmlParser::_handleInTableBody(HtmlToken const& t) {
    auto _clearTheStackBackToATableBodyContext = [&]() {
        while (
            _currentElement()->tagName != Html::TBODY and
            _currentElement()->tagName != Html::TFOOT and
            _currentElement()->tagName != Html::THEAD and
            _currentElement()->tagName != Html::TEMPLATE and
            _currentElement()->tagName != Html::HTML
        ) {
            _openElements.popBack();
        }
    };

    // A start tag whose tag name is "tr"
    if (t.type == HtmlToken::START_TAG and t.name == "tr") {
        // Clear the stack back to a table body context. (See below.)
        _clearTheStackBackToATableBodyContext();

        // Insert an HTML element for the token, then switch the insertion mode to "in row".
        _insertHtmlElement(t);
        _switchTo(Mode::IN_ROW);
    }

    // A start tag whose tag name is one of: "th", "td"
    else if (t.type == HtmlToken::START_TAG and (t.name == "th" or t.name == "td")) {
        _raise();

        // Clear the stack back to a table body context. (See below.)
        _clearTheStackBackToATableBodyContext();

        // Insert an HTML element for a "tr" start tag token with no attributes, then switch the insertion mode to "in row".
        HtmlToken tableRowToken;
        tableRowToken.type = HtmlToken::START_TAG;
        tableRowToken.name = "tr"s;
        _insertAForeignElement(tableRowToken, Vaev::HTML);

        _switchTo(Mode::IN_ROW);

        accept(t);
    }

    else if (t.type == HtmlToken::END_TAG and (t.name == "tbody" or t.name == "tfoot" or t.name == "thead")) {
        // If the stack of open elements does not have an element in table scope that is an HTML element with the same
        // tag name as the token, this is a parse error; ignore the token.

        if (not _hasElementInTableScope(TagName::make(t.name, Vaev::HTML))) {
            _raise();
            return;
        }

        // Clear the stack back to a table body context. (See below.)
        _clearTheStackBackToATableBodyContext();

        // Pop the current node from the stack of open elements. Switch the insertion mode to "in table".
        _openElements.popBack();
        _switchTo(Mode::IN_TABLE);
    }

    else if (
        (t.type == HtmlToken::START_TAG and
         (t.name == "caption" or t.name == "col" or t.name == "colgroup" or
          t.name == "tbody" or t.name == "tfoot" or t.name == "thead")) or
        (t.type == HtmlToken::END_TAG and t.name == "table")
    ) {

        // If the stack of open elements does not have a tbody, thead, or tfoot element in table scope,
        // TODO: consider refactor so _hasElementInScope accepts list instead of single element
        if (not _hasElementInTableScope(Html::TBODY) and
            not _hasElementInTableScope(Html::THEAD) and
            not _hasElementInTableScope(Html::TFOOT)) {
            // this is a parse error; ignore the token.
            _raise();
            return;
        }

        // Otherwise:

        // Clear the stack back to a table body context. (See below.)
        _clearTheStackBackToATableBodyContext();

        // Pop the current node from the stack of open elements. Switch the insertion mode to "in table".
        _openElements.popBack();
        _switchTo(Mode::IN_TABLE);

        // Reprocess the token.
        accept(t);
    }

    // An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html", "td", "th", "tr"
    else if (
        t.type == HtmlToken::END_TAG and
        (t.name == "body" or t.name == "caption" or t.name == "col" or
         t.name == "colgroup" or t.name == "html" or
         t.name == "td" or t.name == "th" or t.name == "tr"
        )
    ) {
        // Parse error. Ignore the token.
        _raise();
    }

    else {
        // Process the token using the rules for the "in table" insertion mode.
        _acceptIn(Mode::IN_TABLE, t);
    }
}

// 13.2.6.4.14 MARK: The "in row" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intr
void HtmlParser::_handleInTableRow(HtmlToken const& t) {
    auto _clearTheStackBackToATableRowContext = [&]() {
        while (_currentElement()->tagName != Html::TR and
               _currentElement()->tagName != Html::TEMPLATE and
               _currentElement()->tagName != Html::HTML) {

            _openElements.popBack();
        }
    };

    // A start tag whose tag name is one of: "th", "td"
    if (t.type == HtmlToken::START_TAG and (t.name == "th" or t.name == "td")) {
        // Clear the stack back to a table row context. (See below.)
        _clearTheStackBackToATableRowContext();

        // Insert an HTML element for the token, then switch the insertion mode to "in cell".
        _insertHtmlElement(t);
        _switchTo(Mode::IN_CELL);

        // TODO: Insert a marker at the end of the list of active formatting elements.
    }

    // An end tag whose tag name is "tr"
    else if (t.type == HtmlToken::END_TAG and t.name == "tr") {
        if (not _hasElementInTableScope(Html::TR)) {
            _raise();
            return;
        }

        // Otherwise:

        // Clear the stack back to a table row context. (See below.)
        _clearTheStackBackToATableRowContext();

        // Pop the current node (which will be a tr element) from the stack of open elements.
        _openElements.popBack();

        // Switch the insertion mode to "in table body".
        _switchTo(Mode::IN_TABLE_BODY);
    }

    // A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody", "tfoot", "thead", "tr"
    // An end tag whose tag name is "table"
    else if ((t.type == HtmlToken::START_TAG and
              (t.name == "caption" or t.name == "col" or t.name == "colgroup" or
               t.name == "tbody" or t.name == "tfoot" or t.name == "thead" or t.name == "tr"
              )) or
             (t.type == HtmlToken::END_TAG and t.name == "table"
             )) {

        // If the stack of open elements does not have a tr element in table scope,
        if (not _hasElementInTableScope(Html::TR)) {
            // this is a parse error; ignore the token.
            _raise();
            return;
        }

        // Otherwise:

        // Clear the stack back to a table row context. (See below.)
        _clearTheStackBackToATableRowContext();

        // Pop the current node (which will be a tr element) from the stack of open elements.
        _openElements.popBack();

        // Switch the insertion mode to "in table body".
        _switchTo(Mode::IN_TABLE_BODY);

        // Reprocess the token.
        accept(t);
    }

    // An end tag whose tag name is one of: "tbody", "tfoot", "thead"
    else if (t.type == HtmlToken::END_TAG and (t.name == "tbody" or t.name == "tfoot" or t.name == "thead")) {
        // If the stack of open elements does not have an element in table scope that is an HTML element with the same
        // tag name as the token,

        if (not _hasElementInTableScope(TagName::make(t.name, Vaev::HTML))) {
            // this is a parse error; ignore the token.
            _raise();
            return;
        }

        // Clear the stack back to a table body context. (See below.)
        _clearTheStackBackToATableRowContext();

        // Pop the current node (which will be a tr element) from the stack of open elements.
        _openElements.popBack();

        // Switch the insertion mode to "in table body".
        _switchTo(Mode::IN_TABLE_BODY);

        // Reprocess the token.
        accept(t);
    }

    // An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html", "td", "th"
    else if (t.type == HtmlToken::END_TAG and
             (t.name == "body" or t.name == "caption" or t.name == "col" or
              t.name == "colgroup" or t.name == "html" or
              t.name == "td" or t.name == "th"
             )) {
        // Parse error. Ignore the token.
        _raise();
    }

    else {
        // Process the token using the rules for the "in table" insertion mode.
        _acceptIn(Mode::IN_TABLE, t);
    }
}

// 13.2.6.4.15 MARK: The "in cell" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intd
void HtmlParser::_handleInCell(HtmlToken const& t) {
    auto _closeTheCell = [&]() {
        // Generate implied end tags.
        _generateImpliedEndTags(*this);

        // If the current node is not now a td element or a th element, then this is a parse error.
        if (_currentElement()->tagName != Html::TD and _currentElement()->tagName != Html::TR) {
            _raise();
        }

        // Pop elements from the stack of open elements until a td element or a th element has been popped from the stack.
        while (Karm::any(_openElements)) {
            auto poppedEl = _openElements.popBack();
            if (poppedEl->tagName == Html::TD or poppedEl->tagName == Html::TH)
                break;
        }

        // TODO: Clear the list of active formatting elements up to the last marker.

        // Switch the insertion mode to "in row".
        _switchTo(Mode::IN_ROW);
    };

    // An end tag whose tag name is one of: "td", "th"
    if (t.type == HtmlToken::END_TAG and (t.name == "td" or t.name == "th")) {
        // If the stack of open elements does not have an element in table scope that is an HTML element with the same
        // tag name as that of the token,
        TagName tokenTagName{TagName::make(t.name, Vaev::HTML)};

        if (not _hasElementInTableScope(tokenTagName)) {
            // this is a parse error; ignore the token.
            _raise();
            return;
        }

        // Otherwise:

        // Generate implied end tags.
        _generateImpliedEndTags(*this);

        // Now, if the current node is not an HTML element with the same tag name as the token,
        if (_currentElement()->tagName != tokenTagName) {
            // then this is a parse error.
            _raise();
        }

        // Pop elements from the stack of open elements until an HTML element with the same tag name as
        // the token has been popped from the stack.
        while (Karm::any(_openElements) and _openElements.popBack()->tagName != tokenTagName) {
            // do nothing
        }

        // TODO: Clear the list of active formatting elements up to the last marker.

        // Switch the insertion mode to "in row".
        _switchTo(Mode::IN_ROW);
    }

    // A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody", "td", "tfoot", "th", "thead", "tr"
    else if (t.type == HtmlToken::START_TAG and
             (t.name == "caption" or t.name == "col" or t.name == "colgroup" or
              t.name == "tbody" or t.name == "td" or t.name == "tfoot" or
              t.name == "th" or t.name == "thead" or t.name == "tr"
             )) {

        // Assert: The stack of open elements has a td or th element in table scope.
        if (not _hasElementInTableScope(Html::TD) and not _hasElementInTableScope(Html::TR)) {
            _raise();
            // FIXME: should this be a panic()?
        }

        // Close the cell (see below) and reprocess the token.
        _closeTheCell();
        accept(t);
    }

    // An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html"
    else if (t.type == HtmlToken::END_TAG and
             (t.name == "body" or t.name == "caption" or t.name == "col" or
              t.name == "colgroup" or t.name == "html"
             )) {
        // Parse error. Ignore the token.
        _raise();
    }

    // An end tag whose tag name is one of: "table", "tbody", "tfoot", "thead", "tr"
    else if (t.type == HtmlToken::END_TAG and
             (t.name == "table" or t.name == "tbody" or t.name == "tfoot" or t.name == "thead" or t.name == "tr")) {

        // If the stack of open elements does not have an element in table scope that is an HTML element with the same
        // tag name as the token,
        if (not _hasElementInTableScope(TagName::make(t.name, Vaev::HTML))) {
            // this is a parse error; ignore the token.
            _raise();
            return;
        }

        // Otherwise, close the cell (see below) and reprocess the token.
        _closeTheCell();
        accept(t);
    }

    else {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    }
}

// 3.2.6.4.22 MARK: The "after after body" insertion mode
// https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-body-insertion-mode
void HtmlParser::_handleAfterBody(HtmlToken const& t) {
    // A comment token
    if (t.type == HtmlToken::COMMENT) {
        // Insert a comment.
        _insertAComment(t);
    }

    // A DOCTYPE token
    // A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    // A start tag whose tag name is "html"
    else if (t.type == HtmlToken::DOCTYPE or
             (t.type == HtmlToken::CHARACTER and (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')) or
             (t.type == HtmlToken::START_TAG and t.name == "html")) {
        // Process the token using the rules for the "in body" insertion mode.
        _acceptIn(Mode::IN_BODY, t);
    }

    else if (t.type == HtmlToken::END_OF_FILE) {
        // Stop parsing.
    }

    else {
        // Parse error. Switch the insertion mode to "in body" and reprocess the token.
        _raise();
        _switchTo(Mode::IN_BODY);
        accept(t);
    }
}

// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inforeign
void HtmlParser::_handleInForeignContent(HtmlToken const& t) {

    auto handleScript = [&] {
        // TODO

        // Pop the current node off the stack of open elements.
        _openElements.popBack();

        // Let the old insertion point have the same value as the current insertion point. Let the insertion point be just before the next input character.

        // Increment the parser's script nesting level by one. Set the parser pause flag to true.

        // If the active speculative HTML parser is null and the user agent supports SVG, then Process the SVG script element according to the SVG rules. [SVG]

        // Even if this causes new characters to be inserted into the tokenizer, the parser will not be executed reentrantly, since the parser pause flag is true.

        // Decrement the parser's script nesting level by one. If the parser's script nesting level is zero, then set the parser pause flag to false.

        // Let the insertion point have the value of the old insertion point. (In other words, restore the insertion point to its previous value. This value might be the "undefined" value.)
    };

    // A character token that is U+0000 NULL
    if (t.type == HtmlToken::CHARACTER and t.rune == '\0') {
        // Parse error. Insert a U+FFFD REPLACEMENT CHARACTER character.
        _raise();
        _insertACharacter(0xFFFD);
    }

    // A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
    else if (
        t.type == HtmlToken::CHARACTER and
        (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')
    ) {
        // Insert the token's character.
        _insertACharacter(t.rune);
    }

    // Any other character token
    else if (
        t.type == HtmlToken::CHARACTER
    ) {
        // Insert the token's character.
        _insertACharacter(t.rune);

        // Set the frameset-ok flag to "not ok".
        _framesetOk = false;
    }

    // A comment token
    else if (t.type == HtmlToken::COMMENT) {
        // Insert a comment.
        _insertAComment(t);
    }

    // A DOCTYPE token
    else if (t.type == HtmlToken::DOCTYPE) {
        // Parse error. Ignore the token.
        _raise();
    }

    // A start tag whose tag name is one of: "b", "big", "blockquote", "body", "br", "center", "code", "dd", "div", "dl", "dt", "em", "embed", "h1", "h2", "h3", "h4", "h5", "h6", "head", "hr", "i", "img", "li", "listing", "menu", "meta", "nobr", "ol", "p", "pre", "ruby", "s", "small", "span", "strong", "strike", "sub", "sup", "table", "tt", "u", "ul", "var"
    // A start tag whose tag name is "font", if the token has any attributes named "color", "face", or "size"
    // An end tag whose tag name is "br", "p"
    else if (
        (t.type == HtmlToken::START_TAG and contains(Array{"b"s, "big", "blockquote", "body", "br", "center", "code", "dd", "div", "dl", "dt", "em", "embed", "h1", "h2", "h3", "h4", "h5", "h6", "head", "hr", "i", "img", "li", "listing", "menu", "meta", "nobr", "ol", "p", "pre", "ruby", "s", "small", "span", "strong", "strike", "sub", "sup", "table", "tt", "u", "ul", "var"}, t.name)) or
        (t.type == HtmlToken::START_TAG and (t.hasAttribute("color") or t.hasAttribute("face") or t.hasAttribute("size"))) or
        (t.type == HtmlToken::END_TAG and (t.name == "br" or t.name == "p"))
    ) {
        // Parse error.
        _raise();

        while (_openElements.len()) {
            auto el = _currentElement();
            // While the current node is not:
            //  - a MathML text integration point,
            //  - an HTML integration point,
            //  - or an element in the HTML namespace
            if (_isMathMlTextIntegrationPoint(*el) and
                _isHtmlIntegrationPoint(*el) and
                el->tagName.ns == Vaev::HTML) {
                break;
            }

            // pop elements from the stack of open elements.
            _openElements.popBack();
        }

        // Reprocess the token according to the rules given in the section
        // corresponding to the current insertion mode in HTML content.
        accept(t);
    }

    // Any other start tag
    else if (t.type == HtmlToken::START_TAG) {
        // If the adjusted current node is an element in the MathML namespace, ...
        if (_currentElement()->tagName.ns == Vaev::MathML) {
            // TODO: ...adjust MathML attributes for the token. (This fixes the case of MathML attributes that are not all lowercase.)
        }

        // If the adjusted current node is an element in the SVG namespace
        if (_adjustedCurrentElement()->tagName.ns == SVG) {
            // TODO: and the token's tag name is one of the ones in the first column of the following table, change the tag name to the name given in the corresponding cell in the second column. (This fixes the case of SVG elements that are not all lowercase.)
        }

        // If the adjusted current node is an element in the SVG namespace, ...
        if (_adjustedCurrentElement()->tagName.ns == SVG) {
            // TODO: ...adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink in SVG.)
        }

        // Insert a foreign element for the token, with adjusted current node's namespace and false.
        _insertAForeignElement(t, _currentElement()->tagName.ns);

        // If the token has its self-closing flag set, then run the appropriate steps from the following list:
        if (t.selfClosing) {
            // If the token's tag name is "script", and the new current node is in the SVG namespace
            // Acknowledge the token's self-closing flag, and then act as described in the steps for a "script" end tag below.
            if (t.name == "script" and _currentElement()->tagName.ns == SVG) {
                _acknowledgeSelfClosingFlag(t);
                handleScript();
            }

            // Otherwise
            else {
                // Pop the current node off the stack of open elements and acknowledge the token's self-closing flag.
                _openElements.popBack();
                _acknowledgeSelfClosingFlag(t);
            }
        }
    }

    // An end tag whose tag name is "script", if the current node is an SVG script element
    else if (t.type == HtmlToken::END_TAG and t.name == "script") {
        handleScript();
    }

    // Any other end tag
    else if (t.type == HtmlToken::END_TAG) {
        // Run these steps:

        // If node's tag name, converted to ASCII lowercase, is not the same as the tag name of the token, then this is a parse error.
        if (not eqCi(_currentElement()->tagName.name(), t.name.str())) {
            _raise();
        }

        usize curr = _openElements.len();
        while (curr > 0) {
            auto node = _openElements[curr - 1];
            // Loop: If node is the topmost element in the stack of open elements, then return. (fragment case)
            if (curr == 0)
                return;

            // If node's tag name, converted to ASCII lowercase, is the same as the tag name of the token,
            if (eqCi(node->tagName.name(), t.name.str())) {
                // pop elements from the stack of open elements until node has been popped from the stack, and then return.
                while (_currentElement() != node) {
                    _openElements.popBack();
                }
                _openElements.popBack();
                return;
            }

            // Set node to the previous entry in the stack of open elements.
            curr--;
            node = _openElements[curr];

            // If node is not an element in the HTML namespace, return to the step labeled loop.
            if (node->tagName.ns != HTML)
                continue;

            // Otherwise, process the token according to the rules given in the section corresponding to the current insertion mode in HTML content.
            _acceptIn(_insertionMode, t);
            break;
        }
    }
}

void HtmlParser::_switchTo(Mode mode) {
    _insertionMode = mode;
}

void HtmlParser::_acceptIn(Mode mode, HtmlToken const& t) {
    if (t.type != HtmlToken::CHARACTER)
        logDebugIf(DEBUG_HTML_PARSER, "Parsing {} in {}", t, mode);

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

    case Mode::IN_TABLE:
        _handleInTable(t);
        break;

    case Mode::IN_TABLE_TEXT:
        _handleInTableText(t);
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incaption
    case Mode::IN_CAPTION:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incolumngroup
    case Mode::IN_COLUMN_GROUP:
        break;

    case Mode::IN_TABLE_BODY:
        _handleInTableBody(t);
        break;

    case Mode::IN_ROW:
        _handleInTableRow(t);
        break;

    case Mode::IN_CELL:
        _handleInCell(t);
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselect
    case Mode::IN_SELECT:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselectintable
    case Mode::IN_SELECT_IN_TABLE:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intemplate
    case Mode::IN_TEMPLATE:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-body-insertion-mode
    case Mode::AFTER_BODY:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-in-frameset-insertion-mode
    case Mode::IN_FRAMESET:
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-frameset-insertion-mode
    case Mode::AFTER_FRAMESET:
        break;

    case Mode::AFTER_AFTER_BODY:
        _handleAfterBody(t);
        break;

    // TODO: https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-frameset-insertion-mode
    case Mode::AFTER_AFTER_FRAMESET:
        break;

    default:
        break;
    }
}

// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction
void HtmlParser::accept(HtmlToken const& t) {
    // If the stack of open elements is empty
    // If the adjusted current node is an element in the HTML namespace
    // If the adjusted current node is a MathML text integration point and the token is a start tag whose tag name is neither "mglyph" nor "malignmark"
    // If the adjusted current node is a MathML text integration point and the token is a character token
    // If the adjusted current node is a MathML annotation-xml element and the token is a start tag whose tag name is "svg"
    // If the adjusted current node is an HTML integration point and the token is a start tag
    // If the adjusted current node is an HTML integration point and the token is a character token
    // If the token is an end-of-file token
    if (
        isEmpty(_openElements) or
        _currentElement()->tagName.ns == HTML or
        (t.type == HtmlToken::START_TAG and _isHtmlIntegrationPoint(*_currentElement())) or
        (t.type == HtmlToken::CHARACTER and _isMathMlTextIntegrationPoint(*_currentElement())) or
        t.type == HtmlToken::END_OF_FILE
    ) {
        // Process the token according to the rules given in the section
        // corresponding to the current insertion mode in HTML content.
        _acceptIn(_insertionMode, t);
    }

    // Otherwise
    else {
        // Process the token according to the rules given in the section for parsing tokens in foreign content.
        _handleInForeignContent(t);
    }
}

} // namespace Vaev::Dom
