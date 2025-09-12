module;

#include <karm-logger/logger.h>

export module Vaev.Engine:html.parser;

import Karm.Gc;
import Karm.Debug;
import :dom;
import :html.token;
import :html.lexer;

namespace Vaev::Html {

static Debug::Flag debugParser{"web-html-parser"s};

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

export struct HtmlParser : HtmlSink {
    enum struct Mode {
#define ITER(NAME) NAME,
        FOREACH_INSERTION_MODE(ITER)
#undef ITER

            _LEN,
    };

    Gc::Heap& _heap;

    bool _scriptingEnabled = false;
    bool _framesetOk = true;
    bool _fosterParenting = false;

    Mode _insertionMode = Mode::INITIAL;
    Mode _originalInsertionMode = Mode::INITIAL;

    HtmlLexer _lexer;
    Gc::Ref<Dom::Document> _document;
    Vec<Gc::Ref<Dom::Element>> _openElements;
    Gc::Ptr<Dom::Element> _headElement = nullptr;
    Gc::Ptr<Dom::Element> _formElement = nullptr;

    // https://html.spec.whatwg.org/multipage/parsing.html#concept-frag-parse-context
    Gc::Ptr<Dom::Element> _contextElement = nullptr;

    Vec<HtmlToken> _pendingTableCharacterTokens;

    HtmlParser(Gc::Heap& heap, Gc::Ref<Dom::Document> document)
        : _heap(heap), _document(document) {
        _lexer.bind(*this);
    }

    // MARK: Algorithm

    // https://html.spec.whatwg.org/multipage/parsing.html#current-node
    Gc::Ref<Dom::Element> _currentElement() {
        if (_openElements.len() == 0)
            panic("no current element");
        return last(_openElements);
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#adjusted-current-node
    Gc::Ref<Dom::Element> _adjustedCurrentElement() {
        // If the parser was created as part of the HTML fragment parsing
        // algorithm and the stack of open elements has
        // only one element in it (fragment case);
        if (_contextElement and _openElements.len() == 1) {
            // The adjusted current node is the context element
            return *_contextElement;
        }

        // Otherwise, the adjusted current node is the current node.
        return _currentElement();
    }

    // 13.2.2 MARK: Parse errors
    // https://html.spec.whatwg.org/multipage/parsing.html#parse-errors
    void _raise(Str msg) {
        logWarn("{}: {}", _insertionMode, msg);
    }

    // 13.2.4.3 MARK: The list of active formatting elements
    // https://html.spec.whatwg.org/multipage/parsing.html#list-of-active-formatting-elements

    // https://html.spec.whatwg.org/multipage/parsing.html#special
    bool _isSpecial(Dom::QualifiedName const& name) {
        static Array const SPECIAL{
#define SPECIAL(NAME) NAME,
#include "defs/special.inc"
#undef SPECIAL
        };
        return contains(SPECIAL, name);
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#mathml-text-integration-point
    bool _isMathMlTextIntegrationPoint(Dom::Element const& el) {
        // A node is a MathML text integration point if it is one of the following elements:
        Array const MATHML_TEXT_INTEGRATION_POINT{
            MathMl::MI_TAG,
            MathMl::MO_TAG,
            MathMl::MN_TAG,
            MathMl::MS_TAG,
            MathMl::MTEXT_TAG,
        };

        // - A MathML mi element
        // - A MathML mo element
        // - A MathML mn element
        // - A MathML ms element
        // - A MathML mtext element
        return contains(MATHML_TEXT_INTEGRATION_POINT, el.qualifiedName);
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#html-integration-point
    bool _isHtmlIntegrationPoint(Dom::Element const& el) {
        // A node is an HTML integration point if it is one of the following elements:

        // - A MathML annotation-xml element whose start tag token had an attribute with the name "encoding" whose value was an ASCII case-insensitive match for the string "text/html"
        // - A MathML annotation-xml element whose start tag token had an attribute with the name "encoding" whose value was an ASCII case-insensitive match for the string "application/xhtml+xml"
        if (el.qualifiedName == MathMl::ANNOTATION_XML_TAG and
            (el.getAttribute(MathMl::ENCODING_ATTR) == "text/html" or
             el.getAttribute(MathMl::ENCODING_ATTR) == "application/xhtml+xml")) {
            return true;
        }

        // - An SVG foreignObject element
        // - An SVG desc element
        // - An SVG title element
        Array const HTML_INTEGRATION_POINT{
            Svg::FOREIGN_OBJECT_TAG,
            Svg::DESC_TAG,
            Svg::TITLE_TAG,
        };

        return contains(HTML_INTEGRATION_POINT, el.qualifiedName);
    }

    // 13.2.4.3 MARK: The list of active formatting elements
    void _reconstructActiveFormattingElements() {
        // TODO
    }

    // 13.2.5 MARK: Tokenization
    // https://html.spec.whatwg.org/multipage/parsing.html#tokenization
    void _acknowledgeSelfClosingFlag(HtmlToken const&) {
        logDebugIf(debugParser, "acknowledgeSelfClosingFlag not implemented");
    }

    // 13.2.6 MARK: Tree construction
    // https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

    // 13.2.6.1 MARK: Creating and inserting nodes
    // https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes

    struct AdjustedInsertionLocation {
        Gc::Ptr<Dom::Element> parent;

        // https://html.spec.whatwg.org/multipage/parsing.html#insert-an-element-at-the-adjusted-insertion-location
        void insert(Gc::Ref<Dom::Node> node) {
            // NOSPEC
            if (parent)
                parent->appendChild(node);
        }

        Gc::Ptr<Dom::Node> lastChild() {
            // NOSPEC
            if (not parent->hasChildren())
                return nullptr;
            return parent->lastChild();
        }
    };

    // https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
    AdjustedInsertionLocation _apropriatePlaceForInsertingANode(Gc::Ptr<Dom::Element> overrideTarget = nullptr) {
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
    Gc::Ref<Dom::Element> _createElementFor(HtmlToken const& t, Symbol ns) {
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
        auto el = _heap.alloc<Dom::Element>(Dom::QualifiedName{ns, t.name});

        // 10. Append each attribute in the given token to element.
        for (auto& [name, value] : t.attrs) {
            el->setAttribute(Dom::QualifiedName{ns, name}, value);
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
    Gc::Ref<Dom::Element> _insertAForeignElement(HtmlToken const& t, Symbol ns, bool onlyAddToElementStack) {
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
    Gc::Ref<Dom::Element> _insertHtmlElement(HtmlToken const& t) {
        // To insert an HTML element given a token token:
        // 1. insert a foreign element given token, the HTML namespace, and false.
        return _insertAForeignElement(t, Html::NAMESPACE, false);
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
    void _insertACharacter(Rune c) {
        // 2. Let the adjusted insertion location be the appropriate place for inserting a node.
        auto location = _apropriatePlaceForInsertingANode();

        // 3. If the adjusted insertion location is inside a Document node, then ignore the token.
        if (location.parent->nodeType() == Dom::NodeType::DOCUMENT)
            return;

        // 4. If there is a Text node immediately before the adjusted insertion
        //    location, then append data to that Text node's data.
        auto lastChild = location.lastChild();
        if (lastChild and lastChild->nodeType() == Dom::NodeType::TEXT) {
            auto text = lastChild->is<Dom::Text>();
            text->appendData(c);
        }

        // Otherwise, create a new Text node whose data is data and whose node
        //            document is the same as that of the element in which the
        //            adjusted insertion location finds itself, and insert the
        //            newly created node at the adjusted insertion location.
        else {
            auto text = _heap.alloc<Dom::Text>(""s);
            text->appendData(c);
            location.insert(text);
        }
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#insert-a-comment
    void _insertAComment(HtmlToken const& t) {
        // 1. Let data be the data given in the comment token being processed.

        // TODO:
        // 2. If position was specified, then let the adjusted insertion
        //    location be position. Otherwise, let adjusted insertion location
        //    be the appropriate place for inserting a node.
        auto location = _apropriatePlaceForInsertingANode();

        // 3. Create a Comment node whose data attribute is set to data and
        //    whose node document is the same as that of the node in which
        //    the adjusted insertion location finds itself.
        auto comment = _heap.alloc<Dom::Comment>(t.data);

        // 4. Insert the newly created node at the adjusted insertion location.
        location.insert(comment);
    }

    // https://html.spec.whatwg.org/multipage/parsing.html#reset-the-insertion-mode-appropriately
    void _resetTheInsertionModeAppropriately() {
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
            if (node->qualifiedName == Html::SELECT_TAG) {
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
                    if (_openElements[ancestorIdx]->qualifiedName == Html::SELECT_TAG)
                        break;

                    // 4.6 If ancestor is a table node, switch the insertion mode to "in select in table" and return.
                    if (_openElements[ancestorIdx]->qualifiedName == Html::TABLE_TAG) {
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
            if ((node->qualifiedName == Html::TD_TAG or node->qualifiedName == Html::TH_TAG) and not _last) {
                _switchTo(HtmlParser::Mode::IN_CELL);
                return;
            }

            // 6. If node is a tr element, then switch the insertion mode to "in row" and return.
            if (node->qualifiedName == Html::TR_TAG) {
                _switchTo(HtmlParser::Mode::IN_ROW);
                return;
            }

            // 7. If node is a tbody, thead, or tfoot element, then switch the insertion mode to "in table body" and return.
            if (node->qualifiedName == Html::TBODY_TAG or node->qualifiedName == Html::THEAD_TAG or node->qualifiedName == Html::TFOOT_TAG) {
                _switchTo(HtmlParser::Mode::IN_TABLE_BODY);
                return;
            }

            // 8. If node is a caption element, then switch the insertion mode to "in caption" and return.
            if (node->qualifiedName == Html::CAPTION_TAG) {
                _switchTo(HtmlParser::Mode::IN_CAPTION);
                return;
            }

            // 9. If node is a colgroup element, then switch the insertion mode to "in column group" and return.
            if (node->qualifiedName == Html::COLGROUP_TAG) {
                _switchTo(HtmlParser::Mode::IN_COLUMN_GROUP);
                return;
            }

            // 10. If node is a table element, then switch the insertion mode to "in table" and return.
            if (node->qualifiedName == Html::TABLE_TAG) {
                _switchTo(HtmlParser::Mode::IN_TABLE);
                return;
            }

            // 11. If node is a template element, then switch the insertion mode to the current template insertion mode and return.

            // 12. If node is a head element and last is false, then switch the insertion mode to "in head" and return.
            if (node->qualifiedName == Html::HEAD_TAG and not _last) {
                _switchTo(HtmlParser::Mode::IN_HEAD);
                return;
            }

            // 13. If node is a body element, then switch the insertion mode to "in body" and return.
            if (node->qualifiedName == Html::BODY_TAG) {
                _switchTo(HtmlParser::Mode::IN_BODY);
                return;
            }

            // 14. If node is a frameset element, then switch the insertion mode to "in frameset" and return. (fragment case)
            if (node->qualifiedName == Html::FRAMESET_TAG) {
                _switchTo(HtmlParser::Mode::IN_FRAMESET);
                return;
            }

            // 15. If node is an html element, run these substeps:
            if (node->qualifiedName == Html::HTML_TAG) {
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
    void _parseRawTextElement(HtmlToken const& t) {
        _insertHtmlElement(t);
        _lexer._switchTo(HtmlLexer::RAWTEXT);
        _originalInsertionMode = _insertionMode;
        _switchTo(HtmlParser::Mode::TEXT);
    }

    void _parseRcDataElement(HtmlToken const& t) {
        _insertHtmlElement(t);
        _lexer._switchTo(HtmlLexer::RCDATA);
        _originalInsertionMode = _insertionMode;
        _switchTo(HtmlParser::Mode::TEXT);
    }

    // 13.2.6.3 MARK: Closing elements that have implied end tags
    // https://html.spec.whatwg.org/multipage/parsing.html#generate-implied-end-tags

    static void _generateImpliedEndTags(HtmlParser& b, Opt<Dom::QualifiedName> except = NONE) {
        while (contains(IMPLIED_END_TAGS, b._currentElement()->qualifiedName) and
               b._currentElement()->qualifiedName != except) {
            b._openElements.popBack();
        }
    }

    // MARK: Utilities

    // https://html.spec.whatwg.org/#has-an-element-in-the-specific-scope
    bool _hasElementInLambdaScope(Dom::QualifiedName const& name, auto inScopeList) {
        // 1. Initialize node to be the current node (the bottommost node of the stack).
        if (_openElements.len() == 0)
            panic("html element should always be in scope");

        for (usize i = _openElements.len() - 1; i >= 0; --i) {
            auto& el = _openElements[i];

            // 2. If node is target node, terminate in a match state.
            if (el->qualifiedName == name)
                return true;

            // 3. Otherwise, if node is one of the element types in list,
            //    terminate in a failure state.
            if (inScopeList(el->qualifiedName))
                return false;

            // 4. Otherwise, set node to the previous entry in the stack
            //    of open elements and return to step 2.
        }

        // NOTE: This will never fail, since the loop will always terminate in the
        //       previous step if the top of the stack — an html element — is reached.
        unreachable();
    }

    // https://html.spec.whatwg.org/#has-an-element-in-scope
    bool _hasElementInScope(Dom::QualifiedName const& name) {
        return _hasElementInLambdaScope(name, [](Dom::QualifiedName const& name) -> bool {
            return contains(BASIC_SCOPE_DELIMITERS, name);
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-button-scope
    bool _hasElementInButtonScope(Dom::QualifiedName const& name) {
        return _hasElementInLambdaScope(name, [](Dom::QualifiedName const& name) -> bool {
            return name == Html::BUTTON_TAG or contains(BASIC_SCOPE_DELIMITERS, name);
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-list-item-scope
    bool _hasElementInListItemScope(Dom::QualifiedName const& name) {
        return _hasElementInLambdaScope(name, [](Dom::QualifiedName const& name) -> bool {
            return name == Html::OL_TAG or name == Html::UL_TAG or contains(BASIC_SCOPE_DELIMITERS, name);
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-table-scope
    bool _hasElementInTableScope(Dom::QualifiedName const& name) {
        return _hasElementInLambdaScope(name, [](Dom::QualifiedName const& name) -> bool {
            return name == Html::HTML_TAG or name == Html::TABLE_TAG or name == Html::TEMPLATE_TAG;
        });
    }

    // https://html.spec.whatwg.org/#has-an-element-in-select-scope
    bool _hasElementInSelectScope(Dom::QualifiedName const& name) {
        return _hasElementInLambdaScope(name, [](Dom::QualifiedName const& name) -> bool {
            return name != Html::OPTGROUP_TAG and name != Html::OPTION_TAG;
        });
    }

    // MARK: Modes

    // 13.2.6.4.1 MARK: The "initial" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode

    static Dom::QuirkMode _whichQuirkMode(HtmlToken const&) {
        // NOSPEC: We assume no quirk mode
        return Dom::QuirkMode::NO;
    }

    void _handleInitialMode(HtmlToken& t) {
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
            _document->appendChild(_heap.alloc<Dom::Comment>(t.data));
        }

        // A DOCTYPE token
        else if (t.type == HtmlToken::DOCTYPE) {
            _document->appendChild(
                _heap.alloc<Dom::DocumentType>(
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
    void _handleBeforeHtml(HtmlToken& t) {
        // A DOCTYPE token
        if (t.type == HtmlToken::DOCTYPE) {
            // ignore
            _raise("unexpected DOCTYPE");
        }

        // A comment token
        else if (t.type == HtmlToken::COMMENT) {
            _document->appendChild(_heap.alloc<Dom::Comment>(t.data));
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
            auto el = _createElementFor(t, Html::NAMESPACE);
            _document->appendChild(el);
            _openElements.pushBack(el);
            _switchTo(Mode::BEFORE_HEAD);
        }

        // Any other end tag
        else if (t.type == HtmlToken::END_TAG and not(t.name == "head" or t.name == "body" or t.name == "html" or t.name == "br")) {
            // ignore
            _raise("unexpected end tag");
        }

        // An end tag whose tag name is one of: "head", "body", "html", "br"
        // Anything else
        else {
            auto el = _heap.alloc<Dom::Element>(Html::HTML_TAG);
            _document->appendChild(el);
            _openElements.pushBack(el);
            _switchTo(Mode::BEFORE_HEAD);
            accept(t);
        }
    }

    // 13.2.6.4.3 MARK: The "before head" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
    void _handleBeforeHead(HtmlToken& t) {
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
            _raise("unexpected DOCTYPE token");
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
            _raise("unexpected end tag");
        }

        // An end tag whose tag name is one of: "head", "body", "html", "br"
        // Anything else
        else {
            HtmlToken headToken;
            headToken.type = HtmlToken::START_TAG;
            headToken.name = "head"_sym;
            _headElement = _insertHtmlElement(headToken);
            _switchTo(Mode::IN_HEAD);
            accept(t);
        }
    }

    // 13.2.6.4.4 MARK: The "in head" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
    void _handleInHead(HtmlToken& t) {
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
            _raise("unexpected DOCTYPE token");
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
            auto el = _createElementFor(t, Html::NAMESPACE);

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
            _raise("unexpected head tag");
        } else {
            anythingElse();
        }
    }

    // 13.2.6.4.5 MARK: The "in head noscript" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
    void _handleInHeadNoScript(HtmlToken& t) {
        auto anythingElse = [&] {
            _raise("unexpected token");
            _openElements.popBack();
            _switchTo(Mode::IN_HEAD);
            accept(t);
        };

        // A DOCTYPE token
        if (t.type == HtmlToken::DOCTYPE) {
            _raise("unexpected DOCTYPE token");
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
            _raise("unexpected end tag");
        }

        // Anything else
        else {
            anythingElse();
        }
    }

    // 13.2.6.4.6 MARK: The "after head" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
    void _handleAfterHead(HtmlToken& t) {
        auto anythingElse = [&] {
            HtmlToken bodyToken;
            bodyToken.type = HtmlToken::START_TAG;
            bodyToken.name = "body"_sym;
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
            _raise("unexpected DOCTYPE token");
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
            _raise("unexpected start tag");
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
        // Any other end tag
        else if (t.type == HtmlToken::END_TAG or (t.type == HtmlToken::START_TAG and t.name == "head")) {
            // ignore
            _raise("unexpected head tag");
        }

        // Anything else
        else {
            anythingElse();
        }
    }

    // 13.2.6.4.7 MARK: The "in body" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
    void _handleInBody(HtmlToken& t) {
        auto closePElementIfInButtonScope = [&]() {
            // https://html.spec.whatwg.org/#close-a-p-element
            // If the stack of open elements has a p element in button scope, then close a p element.
            if (_hasElementInButtonScope(Html::P_TAG)) {

                // Generate implied end tags, except for p elements.
                _generateImpliedEndTags(*this, Html::P_TAG);

                // If the current node is not a p element, then this is a parse error.
                if (_currentElement()->qualifiedName != Html::P_TAG)
                    _raise("unexpected p element");

                // Pop elements from the stack of open elements until a p element has been popped from the stack.
                while (_openElements.len() > 0) {
                    auto lastEl = last(_openElements);

                    _openElements.popBack();
                    if (lastEl->qualifiedName == Html::P_TAG)
                        break;
                }
            }
        };

        // A character token that is U+0000 NULL
        if (t.type == HtmlToken::CHARACTER and t.rune == '\0') {
            _raise("unexpected NULL character");
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
            _raise("unexpected DOCTYPE token");
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
            if (not _hasElementInScope(Html::BODY_TAG)) {
                _raise("unexpected end tag for body");
                return;
            }

            // Otherwise, if there is a node in the stack of open elements that
            // is not an implied end tag or
            // tbody element, a td element, a tfoot element, a th element, a thead element, a tr element, the body element, or the html
            // then this is a parse error.
            for (auto& el : _openElements) {
                if (not contains(IMPLIED_END_TAGS, el->qualifiedName) and
                    el->qualifiedName != Html::TBODY_TAG and el->qualifiedName != Html::TD_TAG and el->qualifiedName != Html::TFOOT_TAG and
                    el->qualifiedName != Html::TH_TAG and el->qualifiedName != Html::THEAD_TAG and el->qualifiedName != Html::TR_TAG and
                    el->qualifiedName != Html::BODY_TAG and el->qualifiedName != Html::HTML_TAG) {
                    _raise("unexpected end tag for body");
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
             t.name == "p" or t.name == "search" or t.name == "section" or t.name == "summary" or t.name == "ul")
        ) {
            // If the stack of open elements has a p element in button scope, then close a p element.
            closePElementIfInButtonScope();

            // Insert an HTML element for the token.
            _insertHtmlElement(t);
        }

        // A start tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"
        else if (
            t.type == HtmlToken::START_TAG and
            (t.name == "h1" or t.name == "h2" or t.name == "h3" or t.name == "h4" or
             t.name == "h5" or t.name == "h6")
        ) {
            // If the stack of open elements has a p element in button scope, then close a p element.
            closePElementIfInButtonScope();

            // If the current node is an HTML element whose tag name is one of "h1", "h2", "h3", "h4", "h5", or "h6",
            // then this is a parse error; pop the current node off the stack of open elements.
            if (
                _currentElement()->qualifiedName == Html::H1_TAG or _currentElement()->qualifiedName == Html::H2_TAG or
                _currentElement()->qualifiedName == Html::H3_TAG or _currentElement()->qualifiedName == Html::H4_TAG or
                _currentElement()->qualifiedName == Html::H5_TAG or _currentElement()->qualifiedName == Html::H6_TAG
            ) {
                _raise("unexpected h1-h6 tag");
                _openElements.popBack();
            }

            // Insert an HTML element for the token.
            _insertHtmlElement(t);
        }

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
                auto tag = _openElements[curr - 1]->qualifiedName;
                // If node is a dd element, then run these substeps:
                if (tag == Html::DD_TAG) {
                    // 1. Generate implied end tags, except for dd elements.
                    _generateImpliedEndTags(*this, Html::DD_TAG);

                    // 2. If the current node is not a dd element, then this is a parse error.
                    if (_currentElement()->qualifiedName != Html::DD_TAG) {
                        _raise("unexpected dd tag");
                    }

                    // 3. Pop elements from the stack of open elements until a dd element has been popped from the stack.
                    while (Karm::any(_openElements) and _openElements.popBack()->qualifiedName != Html::DD_TAG) {
                        // do nothing
                    }

                    // 4. Jump to the step labeled done below.
                    done();
                    return;
                }

                if (tag == Html::DT_TAG) {
                    // 1. Generate implied end tags, except for dt elements.
                    _generateImpliedEndTags(*this, Html::DT_TAG);

                    // 2. If the current node is not a dt element, then this is a parse error.
                    if (_currentElement()->qualifiedName != Html::DT_TAG) {
                        _raise("unexpected dt tag");
                    }

                    // 3. Pop elements from the stack of open elements until a dt element has been popped from the stack.
                    while (Karm::any(_openElements) and _openElements.popBack()->qualifiedName != Html::DT_TAG) {
                        // do nothing
                    }

                    // 4. Jump to the step labeled done below.
                    done();
                    return;
                }

                // 5. If node is in the special category, but is not an address,
                //    div, or p element, then jump to the step labeled done below.
                if (_isSpecial(tag) and not(tag == Html::ADDRESS_TAG or tag == Html::DIV_TAG or tag == Html::P_TAG)) {
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

        // An end tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"
        else if (
            t.type == HtmlToken::END_TAG and
            (t.name == "h1" or t.name == "h2" or t.name == "h3" or t.name == "h4" or
             t.name == "h5" or t.name == "h6")
        ) {
            // If the stack of open elements does not have an element in scope that is an HTML element and whose tag name is
            // one of "h1", "h2", "h3", "h4", "h5", or "h6",
            if (not _hasElementInScope(Html::H1_TAG) and
                not _hasElementInScope(Html::H2_TAG) and
                not _hasElementInScope(Html::H3_TAG) and
                not _hasElementInScope(Html::H4_TAG) and
                not _hasElementInScope(Html::H5_TAG) and
                not _hasElementInScope(Html::H6_TAG)) {
                // then this is a parse error; ignore the token.
                _raise("unexpected end tag for h1-h6");
                return;
            }

            // Otherwise, run these steps:

            // 1. Generate implied end tags.
            _generateImpliedEndTags(*this);

            // 2. If the current node is not an HTML element with the same tag name as that of the token, then this is a parse error.
            if (_currentElement()->qualifiedName != Dom::QualifiedName{Html::NAMESPACE, t.name}) {
                // then this is a parse error.
                _raise("unexpected end tag for h1-h6");
            }

            // 3. Pop elements from the stack of open elements until an HTML element whose tag name is one of "h1", "h2",
            // "h3", "h4", "h5", or "h6" has been popped from the stack.
            while (Karm::any(_openElements)) {
                auto poppedEl = _openElements.popBack();
                if (
                    poppedEl->qualifiedName == Html::H1_TAG or poppedEl->qualifiedName == Html::H2_TAG or
                    poppedEl->qualifiedName == Html::H3_TAG or poppedEl->qualifiedName == Html::H4_TAG or
                    poppedEl->qualifiedName == Html::H5_TAG or poppedEl->qualifiedName == Html::H6_TAG
                )
                    break;
            }
        }

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

        // An end tag whose tag name is "br"
        // A start tag whose tag name is one of: "area", "br", "embed", "img", "keygen", "wbr"
        else if (
            t.name == "br" or
            (t.type == HtmlToken::START_TAG and
             (t.name == "area" or t.name == "br" or t.name == "embed" or t.name == "img" or t.name == "keygen" or t.name == "wbr"))
        ) {
            if (t.type == HtmlToken::END_TAG) {
                // Parse error.
                _raise("unexpected end tag for br");

                // Drop the attributes from the token, and act as described in the next entry; i.e. act as if
                // this was a "br" start tag token with no attributes, rather than the end tag token that it actually is.
                // FIXME: cannot drop attributes since token is const
            }

            // Reconstruct the active formatting elements, if any.
            _reconstructActiveFormattingElements();

            // Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.
            _insertHtmlElement(t);
            _openElements.popBack();

            // Acknowledge the token's self-closing flag, if it is set.
            _acknowledgeSelfClosingFlag(t);

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

        // A start tag whose tag name is "hr"
        else if (t.type == HtmlToken::START_TAG and t.name == "hr") {
            // If the stack of open elements has a p element in button scope, then close a p element.
            closePElementIfInButtonScope();
            // Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.
            _insertHtmlElement(t);
            _openElements.popBack();
            // Acknowledge the token's self-closing flag, if it is set.
            _acknowledgeSelfClosingFlag(t);

            // Set the frameset-ok flag to "not ok".
            _framesetOk = false;
        }

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

            // Adjust SVG attributes for the token. (This fixes the case of
            // SVG attributes that are not all lowercase.)
            for (auto& [name, value] : t.attrs)
                name = Svg::qualifiedAttrNameCased(name.str());

            // Adjust foreign attributes for the token. (This fixes the use of
            // namespaced attributes, in particular XLink in SVG.)

            // Insert a foreign element for the token, with SVG namespace and false.
            _insertAForeignElement(t, Svg::NAMESPACE, false);

            // If the token has its self-closing flag set, pop the current node
            // off the stack of open elements and acknowledge the token's self-closing flag.
            if (t.selfClosing) {
                _openElements.popBack();
                _acknowledgeSelfClosingFlag(t);
            }
        }

        // A start tag whose tag name is one of: "caption", "col", "colgroup", "frame", "head", "tbody", "td", "tfoot", "th", "thead", "tr"
        else if (
            t.type == HtmlToken::START_TAG and
            (t.name == "caption" or t.name == "col" or t.name == "colgroup" or t.name == "frame" or
             t.name == "head" or t.name == "tbody" or t.name == "td" or t.name == "tfoot" or
             t.name == "th" or t.name == "thead" or t.name == "tr")
        ) {
            // Parse error. Ignore the token.
            _raise("unexpected start tag");
        }

        else if (t.type == HtmlToken::START_TAG) {
            _reconstructActiveFormattingElements();
            _insertHtmlElement(t);
        }

        // Any other end tag
        else if (t.type == HtmlToken::END_TAG) {
            // loop:
            usize curr = _openElements.len();
            while (curr > 0) {
                // 1. Initialize node to be the current node (the bottommost node of the stack).
                auto node = _openElements[curr - 1];

                // 2. Loop: If node is an HTML element with the same tag name as the token, then:
                if (node->qualifiedName.name == t.name) {
                    // 1. Generate implied end tags, except for HTML elements with the same tag name as the token.
                    _generateImpliedEndTags(*this, node->qualifiedName);

                    // 2. If node is not the current node, then this is a parse error.
                    if (node != _currentElement())
                        _raise("unexpected end tag");

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
    void _handleText(HtmlToken const& t) {
        // A character token
        if (t.type == HtmlToken::CHARACTER) {
            _insertACharacter(
                t.rune == '\0'
                    ? 0xFFFD
                    : t.rune
            );
        }

        else if (t.type == HtmlToken::END_OF_FILE) {
            _raise("unexpected end of file");

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

    // 13.2.6.4.9 MARK: The "in table" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
    void _inTableModeAnythingElse(HtmlToken& t) {
        // Parse error.
        _raise("unexpected token");

        // Enable foster parenting,
        _fosterParenting = true;

        // process the token using the rules for the "in body" insertion mode,
        _acceptIn(HtmlParser::Mode::IN_BODY, t);

        // and then disable foster parenting.
        _fosterParenting = false;
    }

    void _handleInTable(HtmlToken& t) {
        auto _clearTheStackBackToATableContext = [&]() {
            while (_currentElement()->qualifiedName != Html::TABLE_TAG and
                   _currentElement()->qualifiedName != Html::TEMPLATE_TAG and
                   _currentElement()->qualifiedName != Html::HTML_TAG) {

                _openElements.popBack();
            }
        };

        // A character token, if the current node is table, tbody, template, tfoot, thead, or tr element
        if (t.type == HtmlToken::CHARACTER and
            (_currentElement()->qualifiedName == Html::TABLE_TAG or _currentElement()->qualifiedName == Html::TBODY_TAG or
             _currentElement()->qualifiedName == Html::TEMPLATE_TAG or _currentElement()->qualifiedName == Html::TFOOT_TAG or
             _currentElement()->qualifiedName == Html::THEAD_TAG or _currentElement()->qualifiedName == Html::TR_TAG)) {
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
            _raise("unexpected DOCTYPE token");
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
            colGroupToken.name = "colgroup"_sym;
            _insertAForeignElement(colGroupToken, Html::NAMESPACE, false);
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
            HtmlToken tableBodyToken;
            tableBodyToken.type = HtmlToken::START_TAG;
            tableBodyToken.name = "tbody"_sym;
            _insertAForeignElement(tableBodyToken, Html::NAMESPACE, false);
            _switchTo(Mode::IN_TABLE_BODY);

            // Reprocess the current token.
            accept(t);
        }

        // A start tag whose tag name is "table"
        else if (t.type == HtmlToken::START_TAG and t.name == "table") {
            // Parse error.
            _raise("unexpected table start tag");

            // If the stack of open elements does not have a table element in table scope, ignore the token.
            if (not _hasElementInTableScope(Html::TABLE_TAG))
                return;

            // Otherwise:

            // Pop elements from this stack until a table element has been popped from the stack.
            while (Karm::any(_openElements) and _openElements.popBack()->qualifiedName != Html::TABLE_TAG) {
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
            if (not _hasElementInTableScope(Html::TABLE_TAG)) {
                _raise("unexpected table end tag");
                return;
            }

            // Pop elements from this stack until a table element has been popped from the stack.
            while (Karm::any(_openElements) and _openElements.popBack()->qualifiedName != Html::TABLE_TAG) {
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
                  t.name == "thead" or t.name == "tr")) {
            // Parse error. Ignore the token.
            _raise("unexpected end tag");
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
            _raise("unexpected input start tag");

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
            _raise("unexpected form start tag");

            // If there is a template element on the stack of open elements, or if the form element pointer is not null, ignore the token.
            for (auto& el : _openElements) {
                if (el->qualifiedName == Html::TEMPLATE_TAG)
                    return;
            }

            if (not _formElement)
                return;

            // Insert an HTML element for the token, and set the form element pointer to point to the element created.
            HtmlToken formToken;
            formToken.type = HtmlToken::START_TAG;
            formToken.name = "form"_sym;

            _formElement = _insertAForeignElement(formToken, Html::NAMESPACE, false);

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
    void _handleInTableText(HtmlToken& t) {

        // A character token that is U+0000 NULL
        if (t.type == HtmlToken::CHARACTER and t.rune == '\0') {
            // Parse error. Ignore the token.
            _raise("unexpected NULL character token in table text");
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
                for (auto& token : _pendingTableCharacterTokens) {
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

    // 13.2.6.4.11 MARK: The "in caption" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incaption
    void _handleInCaption(HtmlToken& t) {
        auto _closeTheCaption = [&]() {
            // If the stack of open elements does not have a caption element in table scope,
            if (not _hasElementInTableScope(Html::CAPTION_TAG)) {
                // this is a parse error; ignore the token. (fragment case)
                _raise("unexpected caption end tag");
                return false;
            }

            // Otherwise:

            // Generate implied end tags.
            _generateImpliedEndTags(*this);

            // Now, if the current node is not a caption element, then this is a parse error.
            if (_currentElement()->qualifiedName != Html::CAPTION_TAG)
                _raise("unexpected caption end tag");

            // Pop elements from this stack until a caption element has been popped from the stack.
            while (Karm::any(_openElements) and _openElements.popBack()->qualifiedName != Html::CAPTION_TAG) {
                // do nothing
            }

            // TODO: Clear the list of active formatting elements up to the last marker.

            // Switch the insertion mode to "in table".
            _switchTo(Mode::IN_TABLE);

            return true;
        };

        // An end tag whose tag name is "caption"
        if (t.type == HtmlToken::END_TAG and t.name == "caption") {
            _closeTheCaption();
        }

        // A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody", "td", "tfoot", "th", "thead", "tr"
        // An end tag whose tag name is "table"
        else if (
            (t.type == HtmlToken::START_TAG and
             (t.name == "caption" or t.name == "col" or t.name == "colgroup" or t.name == "tbody" or t.name == "td" or
              t.name == "tfoot" or t.name == "th" or t.name == "thead" or t.name == "tr")) or
            (t.type == HtmlToken::END_TAG and t.name == "table")
        ) {
            if (_closeTheCaption()) {
                // Reprocess the token.
                accept(t);
            }
        }

        // An end tag whose tag name is one of: "body", "col", "colgroup", "html", "tbody", "td", "tfoot", "th", "thead", "tr"
        else if (t.type == HtmlToken::END_TAG and
                 (t.name == "body" or t.name == "col" or t.name == "colgroup" or t.name == "html" or t.name == "tbody" or
                  t.name == "td" or t.name == "tfoot" or t.name == "th" or t.name == "thead" or t.name == "tr")) {
            // Parse error. Ignore the token.
            _raise("unexpected end tag");
        }

        // Anything else
        else {
            // Process the token using the rules for the "in body" insertion mode.
            _acceptIn(Mode::IN_BODY, t);
        }
    }

    // 13.2.6.4.12 MARK: The "in column group" insertion modeMARK:
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incolgroup
    void _handleInColumnGroup(HtmlToken& t) {
        // A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF),
        // U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
        if (
            t.type == HtmlToken::CHARACTER and
            (t.rune == '\t' or t.rune == '\n' or t.rune == '\f' or t.rune == '\r' or t.rune == ' ')
        ) {
            // Insert the character.
            _insertACharacter(t.rune);
        }

        // A comment token
        else if (t.type == HtmlToken::COMMENT) {
            // Insert a comment.
            _insertAComment(t);
        }

        // A DOCTYPE token
        else if (t.type == HtmlToken::DOCTYPE) {
            // Parse error. Ignore the token.
            _raise("unexpected DOCTYPE token");
        }

        // A start tag whose tag name is "html"
        else if (t.type == HtmlToken::START_TAG and t.name == "html") {
            // Process the token using the rules for the "in body" insertion mode.
            _acceptIn(Mode::IN_BODY, t);
        }

        // A start tag whose tag name is "col"
        else if (t.type == HtmlToken::START_TAG and t.name == "col") {
            // Insert an HTML element for the token.
            _insertHtmlElement(t);

            // Immediately pop the current node off the stack of open elements.
            _openElements.popBack();

            // Acknowledge the token's self-closing flag, if it is set.
            _acknowledgeSelfClosingFlag(t);
        }

        // An end tag whose tag name is "colgroup"
        else if (t.type == HtmlToken::END_TAG and t.name == "colgroup") {
            // If the current node is not a colgroup element,
            if (_currentElement()->qualifiedName != Html::COLGROUP_TAG) {
                // then this is a parse error;
                _raise("unexpected colgroup end tag");
                // ignore the token.
                return;
            }

            // Otherwise, pop the current node from the stack of open elements.
            _openElements.popBack();

            // Switch the insertion mode to "in table".
            _switchTo(Mode::IN_TABLE);
        }

        // An end tag whose tag name is "col"
        else if (t.type == HtmlToken::END_TAG and t.name == "col") {
            // Parse error. Ignore the token.
            _raise("unexpected col end tag");
        }

        // A start tag whose tag name is "template"
        // An end tag whose tag name is "template"
        else if ((t.type == HtmlToken::START_TAG or t.type == HtmlToken::END_TAG) and t.name == "template") {
            // Process the token using the rules for the "in head" insertion mode.
            _acceptIn(Mode::IN_HEAD, t);
        }

        // An end-of-file token
        else if (t.type == HtmlToken::END_OF_FILE) {
            // Process the token using the rules for the "in body" insertion mode.
            _acceptIn(Mode::IN_BODY, t);
        }

        // Anything else
        else {
            // If the current node is not a colgroup element,
            if (_currentElement()->qualifiedName != Html::COLGROUP_TAG) {
                // then this is a parse error; ignore the token.
                _raise("unexpected token in column group");
                return;
            }

            // Otherwise, pop the current node from the stack of open elements.
            _openElements.popBack();

            // Switch the insertion mode to "in table".
            _switchTo(Mode::IN_TABLE);

            // Reprocess the token.
            accept(t);
        }
    }

    // 13.2.6.4.13 MARK: The "in table body" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intbody
    void _handleInTableBody(HtmlToken& t) {
        auto _clearTheStackBackToATableBodyContext = [&]() {
            while (
                _currentElement()->qualifiedName != Html::TBODY_TAG and
                _currentElement()->qualifiedName != Html::TFOOT_TAG and
                _currentElement()->qualifiedName != Html::THEAD_TAG and
                _currentElement()->qualifiedName != Html::TEMPLATE_TAG and
                _currentElement()->qualifiedName != Html::HTML_TAG
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
            _raise("unexpected th/td start tag");

            // Clear the stack back to a table body context. (See below.)
            _clearTheStackBackToATableBodyContext();

            // Insert an HTML element for a "tr" start tag token with no attributes, then switch the insertion mode to "in row".
            HtmlToken tableRowToken;
            tableRowToken.type = HtmlToken::START_TAG;
            tableRowToken.name = "tr"_sym;
            _insertAForeignElement(tableRowToken, Html::NAMESPACE, false);

            _switchTo(Mode::IN_ROW);

            accept(t);
        }

        else if (t.type == HtmlToken::END_TAG and (t.name == "tbody" or t.name == "tfoot" or t.name == "thead")) {
            // If the stack of open elements does not have an element in table scope that is an HTML element with the same
            // tag name as the token, this is a parse error; ignore the token.
            if (not _hasElementInTableScope(Dom::QualifiedName{Html::NAMESPACE, t.name})) {
                _raise("unexpected end tag");
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
            if (not _hasElementInTableScope(Html::TBODY_TAG) and
                not _hasElementInTableScope(Html::THEAD_TAG) and
                not _hasElementInTableScope(Html::TFOOT_TAG)) {
                // this is a parse error; ignore the token.
                _raise("unexpected start tag or end tag");
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
             t.name == "td" or t.name == "th" or t.name == "tr")
        ) {
            // Parse error. Ignore the token.
            _raise("unexpected end tag");
        }

        // Anything else
        else {
            // Process the token using the rules for the "in table" insertion mode.
            _acceptIn(Mode::IN_TABLE, t);
        }
    }

    // 13.2.6.4.14 MARK: The "in row" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intr
    void _handleInTableRow(HtmlToken& t) {
        auto _clearTheStackBackToATableRowContext = [&]() {
            while (_currentElement()->qualifiedName != Html::TR_TAG and
                   _currentElement()->qualifiedName != Html::TEMPLATE_TAG and
                   _currentElement()->qualifiedName != Html::HTML_TAG) {

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
            if (not _hasElementInTableScope(Html::TR_TAG)) {
                _raise("unexpected tr end tag");
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
                   t.name == "tbody" or t.name == "tfoot" or t.name == "thead" or t.name == "tr")) or
                 (t.type == HtmlToken::END_TAG and t.name == "table")) {

            // If the stack of open elements does not have a tr element in table scope,
            if (not _hasElementInTableScope(Html::TR_TAG)) {
                // this is a parse error; ignore the token.
                _raise("unexpected start/end tag");
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

            if (not _hasElementInTableScope(Dom::QualifiedName{Html::NAMESPACE, t.name})) {
                // this is a parse error; ignore the token.
                _raise("unexpected end tag");
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
                  t.name == "td" or t.name == "th")) {
            // Parse error. Ignore the token.
            _raise("unexpected end tag");
        }

        else {
            // Process the token using the rules for the "in table" insertion mode.
            _acceptIn(Mode::IN_TABLE, t);
        }
    }

    // 13.2.6.4.15 MARK: The "in cell" insertion mode
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intd
    void _handleInCell(HtmlToken& t) {
        auto _closeTheCell = [&]() {
            // Generate implied end tags.
            _generateImpliedEndTags(*this);

            // If the current node is not now a td element or a th element, then this is a parse error.
            if (_currentElement()->qualifiedName != Html::TD_TAG and _currentElement()->qualifiedName != Html::TH_TAG) {
                _raise("unexpected end tag");
            }

            // Pop elements from the stack of open elements until a td element or a th element has been popped from the stack.
            while (Karm::any(_openElements)) {
                auto poppedEl = _openElements.popBack();
                if (poppedEl->qualifiedName == Html::TD_TAG or poppedEl->qualifiedName == Html::TH_TAG)
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
            Dom::QualifiedName tokenQualifiedName{Html::NAMESPACE, t.name};

            if (not _hasElementInTableScope(tokenQualifiedName)) {
                // this is a parse error; ignore the token.
                _raise("unexpected end tag");
                return;
            }

            // Otherwise:

            // Generate implied end tags.
            _generateImpliedEndTags(*this);

            // Now, if the current node is not an HTML element with the same tag name as the token,
            if (_currentElement()->qualifiedName != tokenQualifiedName) {
                // then this is a parse error.
                _raise("unexpected end tag");
            }

            // Pop elements from the stack of open elements until an HTML element with the same tag name as
            // the token has been popped from the stack.
            while (Karm::any(_openElements) and _openElements.popBack()->qualifiedName != tokenQualifiedName) {
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
                  t.name == "th" or t.name == "thead" or t.name == "tr")) {

            // Assert: The stack of open elements has a td or th element in table scope.
            if (not _hasElementInTableScope(Html::TD_TAG) and not _hasElementInTableScope(Html::TR_TAG)) {
                _raise("unexpected start tag");
                // FIXME: should this be a panic()?
            }

            // Close the cell (see below) and reprocess the token.
            _closeTheCell();
            accept(t);
        }

        // An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html"
        else if (t.type == HtmlToken::END_TAG and
                 (t.name == "body" or t.name == "caption" or t.name == "col" or
                  t.name == "colgroup" or t.name == "html")) {
            // Parse error. Ignore the token.
            _raise("unexpected end tag");
        }

        // An end tag whose tag name is one of: "table", "tbody", "tfoot", "thead", "tr"
        else if (t.type == HtmlToken::END_TAG and
                 (t.name == "table" or t.name == "tbody" or t.name == "tfoot" or t.name == "thead" or t.name == "tr")) {

            // If the stack of open elements does not have an element in table scope that is an HTML element with the same
            // tag name as the token,
            if (not _hasElementInTableScope(Dom::QualifiedName{Html::NAMESPACE, t.name})) {
                // this is a parse error; ignore the token.
                _raise("unexpected end tag");
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
    void _handleAfterBody(HtmlToken& t) {
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
            _raise("unexpected token");
            _switchTo(Mode::IN_BODY);
            accept(t);
        }
    }

    // 13.2.6.5 MARK: The rules for parsing tokens in foreign content
    // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inforeign
    void _handleInForeignContent(HtmlToken& t) {
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
            _raise("unexpected NULL character token");
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
            _raise("unexpected DOCTYPE token");
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
            _raise("unexpected foreign content start tag");

            while (_openElements.len()) {
                auto el = _currentElement();
                // While the current node is not:
                //  - a MathML text integration point,
                //  - an HTML integration point,
                //  - or an element in the HTML namespace
                if (_isMathMlTextIntegrationPoint(*el) and
                    _isHtmlIntegrationPoint(*el) and
                    el->qualifiedName.ns == Html::NAMESPACE) {
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
            if (_currentElement()->qualifiedName.ns == MathMl::NAMESPACE) {
                // TODO: ...adjust MathML attributes for the token. (This fixes the case of MathML attributes that are not all lowercase.)
            }

            // If the adjusted current node is an element in the SVG namespace
            if (_adjustedCurrentElement()->qualifiedName.ns == Svg::NAMESPACE) {
                // and the token's tag name is one of the ones in the first column of the following table, change the tag name to the name given in the corresponding cell in the second column. (This fixes the case of SVG elements that are not all lowercase.)
                t.name = Svg::qualifiedTagNameCased(t.name.str());
            }

            // If the adjusted current node is an element in the SVG namespace, ...
            if (_adjustedCurrentElement()->qualifiedName.ns == Svg::NAMESPACE) {
                // ...adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink in SVG.)
                for (auto& [name, value] : t.attrs)
                    name = Svg::qualifiedAttrNameCased(name.str());
            }

            // Insert a foreign element for the token, with adjusted current node's namespace and false.
            _insertAForeignElement(t, _currentElement()->qualifiedName.ns, false);

            // If the token has its self-closing flag set, then run the appropriate steps from the following list:
            if (t.selfClosing) {
                // If the token's tag name is "script", and the new current node is in the SVG namespace
                // Acknowledge the token's self-closing flag, and then act as described in the steps for a "script" end tag below.
                if (t.name == "script" and _currentElement()->qualifiedName.ns == Svg::NAMESPACE) {
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
        else if (t.type == HtmlToken::END_TAG and _currentElement()->qualifiedName == Html::SCRIPT_TAG) {
            handleScript();
        }

        // Any other end tag
        else if (t.type == HtmlToken::END_TAG) {
            // Run these steps:

            // If node's tag name, converted to ASCII lowercase, is not the same as the tag name of the token, then this is a parse error.
            if (not eqCi(_currentElement()->qualifiedName.name.str(), t.name.str())) {
                _raise("unexpected end tag");
            }

            usize curr = _openElements.len();
            while (curr > 0) {
                auto node = _openElements[curr - 1];
                // Loop: If node is the topmost element in the stack of open elements, then return. (fragment case)
                if (curr == 0)
                    return;

                // If node's tag name, converted to ASCII lowercase, is the same as the tag name of the token,
                if (eqCi(node->qualifiedName.name.str(), t.name.str())) {
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
                if (node->qualifiedName.ns != Html::NAMESPACE)
                    continue;

                // Otherwise, process the token according to the rules given in the section corresponding to the current insertion mode in HTML content.
                _acceptIn(_insertionMode, t);
                break;
            }
        }
    }

    void _switchTo(Mode mode) {
        _insertionMode = mode;
    }

    void _acceptIn(Mode mode, HtmlToken& t) {
        if (t.type != HtmlToken::CHARACTER)
            logDebugIf(debugParser, "Parsing {} in {}", t, mode);

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

        case Mode::IN_CAPTION:
            _handleInCaption(t);
            break;

        case Mode::IN_COLUMN_GROUP:
            _handleInColumnGroup(t);
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
    void accept(HtmlToken& t) override {
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
            _currentElement()->qualifiedName.ns == Html::NAMESPACE or
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

    void write(Str str) {
        for (auto r : iterRunes(str))
            _lexer.consume(r);
        // NOTE: '\3' (End of Text) is used here as a placeholder so we are directed to the EOF case
        _lexer.consume('\3', true);
    }
};

#undef FOREACH_INSERTION_MODE

} // namespace Vaev::Html
