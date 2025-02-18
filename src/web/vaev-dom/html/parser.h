#pragma once

#include <karm-gc/heap.h>

#include "../document.h"
#include "../element.h"
#include "lexer.h"

namespace Vaev::Dom {

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

    Gc::Heap& _heap;

    bool _scriptingEnabled = false;
    bool _framesetOk = true;
    bool _fosterParenting = false;

    Mode _insertionMode = Mode::INITIAL;
    Mode _originalInsertionMode = Mode::INITIAL;

    HtmlLexer _lexer;
    Gc::Ref<Document> _document;
    Vec<Gc::Ref<Element>> _openElements;
    Gc::Ptr<Element> _headElement = nullptr;
    Gc::Ptr<Element> _formElement = nullptr;

    Vec<HtmlToken> _pendingTableCharacterTokens;

    HtmlParser(Gc::Heap& heap, Gc::Ref<Document> document)
        : _heap(heap), _document(document) {
        _lexer.bind(*this);
    }

    // MARK: Algorithm
    void _raise(Str msg = "parse-error");

    bool _isSpecial(TagName const& tag);

    void _reconstructActiveFormattingElements();

    void _acknowledgeSelfClosingFlag(HtmlToken const&);

    struct AdjustedInsertionLocation {
        Gc::Ptr<Element> parent;

        // https://html.spec.whatwg.org/multipage/parsing.html#insert-an-element-at-the-adjusted-insertion-location
        void insert(Gc::Ref<Node> node) {
            // NOSPEC
            if (parent)
                parent->appendChild(node);
        }

        Gc::Ptr<Node> lastChild() {
            // NOSPEC
            if (not parent->hasChildren())
                return nullptr;
            return parent->lastChild();
        }
    };

    AdjustedInsertionLocation _apropriatePlaceForInsertingANode(Gc::Ptr<Element> overrideTarget = nullptr);

    Gc::Ref<Element> _createElementFor(HtmlToken const& t, Ns ns);

    Gc::Ref<Element> _insertAForeignElement(HtmlToken const& t, Ns ns, bool onlyAddToElementStack = false);

    Gc::Ref<Element> _insertHtmlElement(HtmlToken const& t);

    void _insertACharacter(Rune c);

    void _insertAComment(HtmlToken const& t);

    void _resetTheInsertionModeAppropriately();

    void _parseRawTextElement(HtmlToken const& t);

    void _parseRcDataElement(HtmlToken const& t);

    // MARK: Utilities

    // FIXME: remaining elements from https://html.spec.whatwg.org/#has-an-element-in-scope
    static constexpr Array BASIC_SCOPE_DELIMITERS = {
        Html::APPLET, Html::CAPTION, Html::HTML, Html::TABLE, Html::TD,
        Html::TH, Html::MARQUEE, Html::OBJECT, Html::TEMPLATE
    };

    bool _hasElementInLambdaScope(TagName tag, auto inScopeList) {
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

    void _inTableModeAnythingElse(HtmlToken const& t);

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

} // namespace Vaev::Dom
