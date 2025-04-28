module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:builder;

import :values;

namespace Vaev::Layout {

static constexpr bool DEBUG_BUILDER = false;

// MARK: Attributes ------------------------------------------------------------

static Opt<Str> _parseStrAttr(Gc::Ref<Dom::Element> el, AttrName name) {
    return el->getAttribute(name);
}

static Opt<usize> _parseUsizeAttr(Gc::Ref<Dom::Element> el, AttrName name) {
    auto str = _parseStrAttr(el, name);
    if (not str)
        return NONE;
    return Io::atoi(str.unwrap());
}

static Attrs _parseDomAttr(Gc::Ref<Dom::Element> el) {
    Attrs attrs;

    // https://html.spec.whatwg.org/multipage/tables.html#the-col-element

    // The element may have a span content attribute specified, whose value must
    // be a valid non-negative integer greater than zero and less than or equal to 1000.

    attrs.span = _parseUsizeAttr(el, Html::SPAN_ATTR).unwrapOr(1);
    if (attrs.span == 0 or attrs.span > 1000)
        attrs.span = 1;

    // https://html.spec.whatwg.org/multipage/tables.html#attributes-common-to-td-and-th-elements

    // The td and th elements may have a colspan content attribute specified,
    // whose value must be a valid non-negative integer greater than zero and less than or equal to 1000.
    attrs.colSpan = _parseUsizeAttr(el, Html::COLSPAN_ATTR).unwrapOr(1);
    if (attrs.colSpan == 0 or attrs.colSpan > 1000)
        attrs.colSpan = 1;

    // The td and th elements may also have a rowspan content attribute specified,
    // whose value must be a valid non-negative integer less than or equal to 65534.
    attrs.rowSpan = _parseUsizeAttr(el, Html::ROWSPAN_ATTR).unwrapOr(1);
    if (attrs.rowSpan > 65534)
        attrs.rowSpan = 65534;

    return attrs;
}

// MARK: Build Prose ----------------------------------------------------------

static Opt<Rc<Karm::Text::Fontface>> _monospaceFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _regularFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _boldFontface = NONE;

static Rc<Karm::Text::Fontface> _lookupFontface(Text::FontBook& fontBook, Style::ComputedStyle& style) {
    Text::FontQuery fq{
        .weight = style.font->weight,
        .style = style.font->style.val,
    };

    for (auto family : style.font->families) {
        fq.family = family;
        if (auto font = fontBook.queryClosest(fq))
            return font.unwrap();
    }

    if (
        auto font = fontBook.queryClosest({
            .family = String{"Inter"s},
        })
    )
        return font.unwrap();

    return Text::Fontface::fallback();
}

bool isSegmentBreak(Rune rune) {
    return rune == '\n' or rune == '\r' or rune == '\f' or rune == '\v';
}

static Text::ProseStyle _proseStyleFomStyle(Style::ComputedStyle& style, Rc<Text::Fontface> fontFace) {
    // FIXME: We should pass this around from the top in order to properly resolve rems
    Resolver resolver{
        .rootFont = Text::Font{fontFace, 16},
        .boxFont = Text::Font{fontFace, 16},
    };
    Text::ProseStyle proseStyle{
        .font = {
            fontFace,
            resolver.resolve(style.font->size).cast<f64>(),
        },
        .color = style.color,
        .multiline = true,
    };

    switch (style.text->align) {
    case TextAlign::START:
    case TextAlign::LEFT:
        proseStyle.align = Text::TextAlign::LEFT;
        break;

    case TextAlign::END:
    case TextAlign::RIGHT:
        proseStyle.align = Text::TextAlign::RIGHT;
        break;

    case TextAlign::CENTER:
        proseStyle.align = Text::TextAlign::CENTER;
        break;

    default:
        // FIXME: Implement the rest
        break;
    }

    return proseStyle;
}

static Text::ProseStyle _proseStyleFomStyle(Style::Computer& c, Style::ComputedStyle& style) {
    auto fontFace = _lookupFontface(c.fontBook, style);
    return _proseStyleFomStyle(style, fontFace);
}

void _transformAndAppendRuneToProse(Rc<Text::Prose> prose, Rune rune, TextTransform transform) {
    switch (transform) {
    case TextTransform::UPPERCASE:
        prose->append(toAsciiUpper(rune));
        break;

    case TextTransform::LOWERCASE:
        prose->append(toAsciiLower(rune));
        break;

    case TextTransform::NONE:
    default:
        prose->append(rune);
        break;
    }
}

// https://www.w3.org/TR/css-text-3/#white-space-phase-1
// https://www.w3.org/TR/css-text-3/#white-space-phase-2
void _appendTextToInlineBox(Io::SScan scan, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox) {
    auto whitespace = parentStyle->text->whiteSpace;
    bool whiteSpacesAreCollapsible =
        whitespace == WhiteSpace::NORMAL or
        whitespace == WhiteSpace::NOWRAP or
        whitespace == WhiteSpace::PRE_LINE;

    // A sequence of collapsible spaces at the beginning of a line is removed.
    if (not rootInlineBox.active())
        scan.eat(Re::space());

    while (not scan.ended()) {
        auto rune = scan.next();

        if (not isAsciiSpace(rune)) {
            _transformAndAppendRuneToProse(rootInlineBox.prose, rune, parentStyle->text->transform);
            continue;
        }

        // https://www.w3.org/TR/css-text-3/#collapse
        if (whiteSpacesAreCollapsible) {
            // Any sequence of collapsible spaces and tabs immediately preceding or following a segment break is removed.
            bool visitedSegmentBreak = false;
            while (true) {
                if (isSegmentBreak(rune))
                    visitedSegmentBreak = true;

                if (scan.ended() or not isAsciiSpace(scan.peek()))
                    break;

                rune = scan.next();
            }

            // Any collapsible space immediately following another collapsible space—​even one outside the boundary
            // of the inline containing that space, provided both spaces are within the same inline formatting
            // context—​is collapsed to have zero advance width. (It is invisible, but retains its soft wrap
            // opportunity, if any.)
            // TODO: not compliant regarding wrap opportunity

            // https://www.w3.org/TR/css-text-3/#valdef-white-space-pre-line
            // Collapsible segment breaks are transformed for rendering according to the segment
            // break transformation rules.
            if (whitespace == WhiteSpace::PRE_LINE and visitedSegmentBreak)
                rootInlineBox.prose->append('\n');
            else
                rootInlineBox.prose->append(' ');
        } else if (whitespace == WhiteSpace::PRE) {
            rootInlineBox.prose->append(rune);
        } else {
            panic("unimplemented whitespace case");
        }
    }
}

bool _buildText(Gc::Ref<Dom::Text> node, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox, bool skipIfWhitespace) {
    if (skipIfWhitespace) {
        Io::SScan scan{node->data()};
        scan.eat(Re::space());
        if (scan.ended())
            return false;
    }

    _appendTextToInlineBox(node->data(), parentStyle, rootInlineBox);
    return true;
}

// MARK: Build Block -----------------------------------------------------------

struct BuilderContext {
    enum struct From {
        BLOCK,
        INLINE,
        FLEX,
        TABLE
    };

    Style::Computer& computer;

    From const from;
    Rc<Style::ComputedStyle> const parentStyle;

    Box& _parent;
    MutCursor<InlineBox> _rootInlineBox;

    // https://www.w3.org/TR/css-inline-3/#model
    void flushRootInlineBoxIntoAnonymousBox() {
        if (not rootInlineBox().active())
            return;

        // The root inline box inherits from its parent block container, but is otherwise unstyleable.
        auto style = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
        style->inherit(*parentStyle);
        style->display = Display{Display::Inside::FLOW, Display::Outside::BLOCK};

        auto newInlineBox = InlineBox::fromInterruptedInlineBox(*_rootInlineBox);
        _parent.add({style, _parent.fontFace, std::move(*_rootInlineBox), nullptr});
        *_rootInlineBox = std::move(newInlineBox);
    }

    void finalizeParentBoxAndFlushInline() {
        if (not rootInlineBox().active())
            return;

        if (_parent.children()) {
            flushRootInlineBoxIntoAnonymousBox();
            return;
        }

        auto newRootInlineBox = InlineBox::fromInterruptedInlineBox(*_rootInlineBox);
        _parent.content = std::move(*_rootInlineBox);
        *_rootInlineBox = std::move(newRootInlineBox);
    }

    Rc<Style::ComputedStyle> style() {
        return parentStyle;
    }

    void addToParentBox(Box&& box) {
        _parent.add(std::move(box));
    }

    InlineBox& rootInlineBox() {
        if (_rootInlineBox == nullptr)
            panic("no root inline box set for the current builder context");
        return *_rootInlineBox;
    }

    void addToInlineRoot(Box&& box) {
        rootInlineBox().add(std::move(box));
    }

    // FIXME: find me a better name
    void startInlineBox(Text::ProseStyle proseStyle) {
        rootInlineBox().startInlineBox(proseStyle);
    }

    void endInlineBox() {
        rootInlineBox().endInlineBox();
    }

    BuilderContext toBlockContext(Box& parent, InlineBox& rootInlineBox) {
        return {
            computer,
            From::BLOCK,
            parent.style,
            parent,
            &rootInlineBox,
        };
    }

    // NOTE: although all inline elements from FLEX containers are blockified, its less complex to have a
    // rootInlineBox setted for it and then calling `_flushRootInlineBoxIntoAnonymousBox` right after a text is added
    BuilderContext toFlexContext(Box& parent, InlineBox& rootInlineBox) {
        return {
            computer,
            From::FLEX,
            parent.style,
            parent,
            &rootInlineBox,
        };
    }

    BuilderContext toBlockContextWithoutRootInline(Box& parent) {
        return {
            computer,
            From::BLOCK,
            parent.style,
            parent,
            nullptr,
        };
    }

    BuilderContext toInlineContext(Rc<Style::ComputedStyle> parentStyle) {
        return {
            computer,
            From::INLINE,
            parentStyle,
            _parent,
            _rootInlineBox,
        };
    }

    // https://www.w3.org/TR/css-tables-3/#fixup-algorithm
    // SPEC: All inline elements are wrapped by cell boxes. Thus, only the cell table content will have a inline box
    // set, while other internal table contexts won't have a inline box
    BuilderContext toTableContent(Box& parent) {
        return {
            computer,
            From::TABLE,
            parent.style,
            parent,
            nullptr,
        };
    }

    BuilderContext toTableCellContent(Box& parent, InlineBox& rootInlineBox) {
        return {
            computer,
            From::TABLE,
            parent.style,
            parent,
            &rootInlineBox,
        };
    }
};

static void _buildNode(BuilderContext bc, Gc::Ref<Dom::Node> node);

// MARK: Build void/leaves ---------------------------------------------------------

// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace#how_does_css_process_whitespace/
static void _buildText(BuilderContext bc, Gc::Ref<Dom::Text> node, Rc<Style::ComputedStyle> parentStyle) {
    // https://www.w3.org/TR/css-tables-3/#fixup-algorithm
    // TODO: For tables, the default case is to skip whitespace text, but there are some extra checks to be done

    // https://www.w3.org/TR/css-flexbox-1/#flex-items
    // However, if the entire sequence of child text runs contains only white space
    // (i.e. characters that can be affected by the white-space property) it is instead not rendered
    // (just as if its text nodes were display:none).

    bool shouldSkipWhitespace =
        bc.from == BuilderContext::From::TABLE or
        bc.from == BuilderContext::From::FLEX or
        bc.from == BuilderContext::From::BLOCK;

    bool addedNonWhitespace = _buildText(node, parentStyle, bc.rootInlineBox(), shouldSkipWhitespace);

    // https://www.w3.org/TR/css-flexbox-1/#algo-anon-box
    // https://www.w3.org/TR/css-flexbox-1/#flex-items
    // Each in-flow child of a flex container becomes a flex item,
    // and each contiguous sequence of child text runs is wrapped in an anonymous block container flex item.
    // However, if the entire sequence of child text runs contains only white space
    // (i.e. characters that can be affected by the white-space property) it is instead not rendered
    // (just as if its text nodes were display:none).
    if (addedNonWhitespace and bc.from == BuilderContext::From::FLEX) {
        bc.flushRootInlineBoxIntoAnonymousBox();
    }
}

static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox) {
    auto style = c.computeFor(*parentStyle, el);
    auto font = _lookupFontface(c.fontBook, *style);

    auto src = el->getAttribute(Html::SRC_ATTR).unwrapOr(""s);
    auto url = Mime::Url::resolveReference(el->baseURI(), Mime::parseUrlOrPath(src))
                   .unwrapOr("bundle://vaev-driver/missing.qoi"_url);

    auto img = Karm::Image::load(url).unwrapOrElse([] {
        return Karm::Image::loadOrFallback("bundle://vaev-driver/missing.qoi"_url).unwrap();
    });

    rootInlineBox.add({style, font, el});
}

static void _buildInputProse(Style::Computer& c, Gc::Ref<Dom::Element> el, Box& parent) {
    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(c.fontBook, *style);
    Resolver resolver{
        .rootFont = Text::Font{font, 16},
        .boxFont = Text::Font{font, 16},
    };
    Text::ProseStyle proseStyle = _proseStyleFomStyle(*style, font);

    auto value = ""s;
    if (el->hasAttribute(Html::VALUE_ATTR))
        value = el->getAttribute(Html::VALUE_ATTR).unwrap();
    else if (el->hasAttribute(Html::PLACEHOLDER_ATTR))
        value = el->getAttribute(Html::PLACEHOLDER_ATTR).unwrap();

    auto prose = makeRc<Text::Prose>(proseStyle, value);

    // FIXME: we should guarantee that input has no children (not added before nor to add after)
    parent.content = InlineBox{prose};
}

always_inline static bool isVoidElement(Gc::Ref<Dom::Element> el) {
    return contains(Html::VOID_TAGS, el->tagName);
}

static void _buildVoidElement(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    if (el->tagName == Html::INPUT) {
        _buildInputProse(bc.computer, el, bc._parent);
    } else if (el->tagName == Html::IMG) {
        _buildImage(bc.computer, *el, bc.parentStyle, bc.rootInlineBox());
    }
}

// MARK: Build flow -------------------------------------------------------------------------------

static void _buildChildren(BuilderContext bc, Gc::Ref<Dom::Node> parent);

static void createAndBuildInlineFlowfromElement(BuilderContext bc, Rc<Style::ComputedStyle> style, Gc::Ref<Dom::Element> el) {
    if (el->tagName == Html::BR) {
        bc.flushRootInlineBoxIntoAnonymousBox();
        return;
    }

    if (isVoidElement(el)) {
        _buildVoidElement(bc, el);
        return;
    }

    auto proseStyle = _proseStyleFomStyle(bc.computer, *style);

    bc.startInlineBox(proseStyle);
    _buildChildren(bc.toInlineContext(style), el);
    bc.endInlineBox();
}

static void buildBlockFlowFromElement(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    if (el->tagName == Html::BR) {
        // do nothing
    } else if (isVoidElement(el)) {
        _buildVoidElement(bc, el);
    } else {
        _buildChildren(bc, el);
    }
    bc.finalizeParentBoxAndFlushInline();
}

static Box createAndBuildBoxFromElement(BuilderContext bc, Rc<Style::ComputedStyle> style, Gc::Ref<Dom::Element> el, Display display) {
    auto font = _lookupFontface(bc.computer.fontBook, *style);
    Box box = {style, font, el};
    InlineBox rootInlineBox{_proseStyleFomStyle(bc.computer, *style)};

    auto newBc = display == Display::Inside::FLEX
                     ? bc.toFlexContext(box, rootInlineBox)
                     : bc.toBlockContext(box, rootInlineBox);

    buildBlockFlowFromElement(newBc, el);

    box.attrs = _parseDomAttr(el);
    return box;
}

// MARK: Build Table -----------------------------------------------------------

static void _buildTableInternal(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Display display);

struct AnonymousTableBoxWrapper {
    Opt<Box> rowBox, cellBox;
    Opt<InlineBox> rootInlineBoxForCell;

    BuilderContext& bc;

    AnonymousTableBoxWrapper(BuilderContext& bc) : bc(bc) {}

    void createRowIfNone(Rc<Style::ComputedStyle> style) {
        if (rowBox)
            return;

        auto rowStyle = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
        rowStyle->inherit(*style);
        rowStyle->display = Display::Internal::TABLE_ROW;

        rowBox = Box{rowStyle, _lookupFontface(bc.computer.fontBook, *style), nullptr};
    }

    void createCellIfNone(Rc<Style::ComputedStyle> style) {
        if (cellBox)
            return;

        auto cellStyle = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
        cellStyle->inherit(*style);
        cellStyle->display = Display::Internal::TABLE_CELL;

        cellBox = Box{cellStyle, _lookupFontface(bc.computer.fontBook, *style), nullptr};
        rootInlineBoxForCell = InlineBox{_proseStyleFomStyle(bc.computer, *style)};
    }

    void finalizeAndResetCell() {
        if (not cellBox)
            return;

        cellBuilderContext().finalizeParentBoxAndFlushInline();
        if (cellBox->content != NONE) {
            if (rowBox)
                bc.toTableContent(*rowBox).addToParentBox(std::move(*cellBox));
            else
                bc.addToParentBox(std::move(*cellBox));
        }

        cellBox = NONE;
    }

    void finalizeAndResetRow() {
        if (not rowBox)
            return;

        if (rowBox->content != NONE)
            bc.addToParentBox(std::move(*rowBox));

        rowBox = NONE;
    }

    BuilderContext cellBuilderContext() {
        return bc.toTableCellContent(*cellBox, *rootInlineBoxForCell);
    }

    BuilderContext rowBuilderContext() {
        return bc.toTableContent(*rowBox);
    }
};

static void _buildNodeWrappedByCell(BuilderContext cellBuilderContext, Gc::Ref<Dom::Element> wrappedEl) {
    _buildNode(cellBuilderContext, *wrappedEl);
}

static void _buildCell(BuilderContext rowBuilderContext, Gc::Ref<Dom::Element> cellEl) {
    _buildNode(rowBuilderContext, *cellEl);
}

static void _buildTableChildrenWhileWrappingIntoAnonymousBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, bool skipCaption, auto predForAccepted) {
    AnonymousTableBoxWrapper anonTableWrapper{bc};

    for (auto child = el->firstChild(); child; child = child->nextSibling()) {
        if (auto el = child->is<Dom::Element>()) {
            auto childStyle = bc.computer.computeFor(*style, *el);
            auto display = childStyle->display;

            if (skipCaption and display == Display::Internal::TABLE_CAPTION)
                continue;

            if (predForAccepted(display)) {
                anonTableWrapper.finalizeAndResetCell();
                anonTableWrapper.finalizeAndResetRow();

                if (display == Display::Internal::TABLE_CELL) {
                    _buildCell(bc, *el);
                } else {
                    _buildTableInternal(bc, *el, childStyle, display);
                }
            } else {
                // Unexpected display type for children, it will be wrapped by anonymous box
                // First dispatching based on the parent's display type
                if (bc.parentStyle->display == Display::Internal::TABLE_ROW) {
                    anonTableWrapper.createCellIfNone(style);
                    _buildNodeWrappedByCell(anonTableWrapper.cellBuilderContext(), *el);
                } else {
                    if (display == Display::Internal::TABLE_CELL) {
                        anonTableWrapper.finalizeAndResetCell();
                        anonTableWrapper.createRowIfNone(style);

                        _buildCell(anonTableWrapper.rowBuilderContext(), *el);
                    } else {
                        anonTableWrapper.createRowIfNone(style);
                        anonTableWrapper.createCellIfNone(style);

                        _buildNodeWrappedByCell(anonTableWrapper.cellBuilderContext(), *el);
                    }
                }
            }
        } else if (auto text = child->is<Dom::Text>()) {
            if (bc.parentStyle->display != Display::Internal::TABLE_ROW)
                anonTableWrapper.createRowIfNone(style);
            anonTableWrapper.createCellIfNone(style);
            _buildText(anonTableWrapper.cellBuilderContext(), *text, style);
        }
    }

    anonTableWrapper.finalizeAndResetCell();
    anonTableWrapper.finalizeAndResetRow();
}

// https://www.w3.org/TR/css-tables-3/#fixup-algorithm
static void _buildTableInternal(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Display display) {
    auto font = _lookupFontface(bc.computer.fontBook, *style);
    Box tableInternalBox = {style, font, el};
    tableInternalBox.attrs = _parseDomAttr(el);

    switch (display.internal()) {
    case Display::Internal::TABLE_HEADER_GROUP:
    case Display::Internal::TABLE_FOOTER_GROUP:
    case Display::Internal::TABLE_ROW_GROUP: {
        // An anonymous table-row box must be generated around each sequence of consecutive children of a
        // table-row-group box which are not table-row boxes.
        _buildTableChildrenWhileWrappingIntoAnonymousBox(bc.toTableContent(tableInternalBox), el, style, false, [](Display const& display) {
            return display == Display::Internal::TABLE_ROW;
        });
        break;
    }
    case Display::Internal::TABLE_COLUMN_GROUP: {
        // "Children of a table-column-group which are not a table-column." should be discarded
        for (auto child = el->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                auto childStyle = bc.computer.computeFor(*style, *el);
                if (childStyle->display != Display::Internal::TABLE_COLUMN)
                    continue;
                _buildTableInternal(bc.toTableContent(tableInternalBox), *el, childStyle, display);
            }
        }
        break;
    }
    case Display::Internal::TABLE_ROW: {
        // An anonymous table-cell box must be generated around each sequence of consecutive children of a
        // table-row box which are not table-cell boxes.
        _buildTableChildrenWhileWrappingIntoAnonymousBox(bc.toTableContent(tableInternalBox), el, style, false, [](Display const& display) {
            return display == Display::Internal::TABLE_CELL;
        });
    }
    case Display::Internal::TABLE_COLUMN: {
        // "Children of a table-column." should be discarded
        // do nothing
        break;
    }
    case Display::Internal::TABLE_BOX:
    case Display::Internal::TABLE_CAPTION:
    case Display::Internal::TABLE_CELL: {
        panic("table internal display in wrong flow");
        break;
    }
    default:
        panic("non-table internal display in wrong flow");
    }

    bc.addToParentBox(std::move(tableInternalBox));
}

static void _buildTableBox(BuilderContext tableWrapperBc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> tableBoxStyle) {
    auto searchAndBuildCaption = [&]() {
        for (auto child = el->firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->is<Dom::Element>()) {
                auto childStyle = tableWrapperBc.computer.computeFor(*tableBoxStyle, *childEl);
                if (childStyle->display != Display::Internal::TABLE_CAPTION)
                    continue;
                _buildNode(tableWrapperBc, *childEl);
            }
        }
    };

    tableBoxStyle->display = Display::Internal::TABLE_BOX;
    auto font = _lookupFontface(tableWrapperBc.computer.fontBook, *tableBoxStyle);
    Box tableBox = {tableBoxStyle, font, el};
    tableBox.attrs = _parseDomAttr(el);

    bool captionsOnTop = tableBoxStyle->table->captionSide == CaptionSide::TOP;
    if (captionsOnTop) {
        searchAndBuildCaption();
    }

    // An anonymous table-row box must be generated around each sequence of consecutive children of a table-root
    // box which are not proper table child boxes.
    _buildTableChildrenWhileWrappingIntoAnonymousBox(tableWrapperBc.toTableContent(tableBox), *el, tableBoxStyle, true, [](Display const& display) {
        return display.isProperTableChild();
    });
    tableWrapperBc.addToParentBox(std::move(tableBox));

    if (not captionsOnTop) {
        searchAndBuildCaption();
    }
}

static Box _createTableWrapperAndBuildTable(BuilderContext bc, Rc<Style::ComputedStyle> tableStyle, Gc::Ref<Dom::Element> tableBoxEl) {
    auto font = _lookupFontface(bc.computer.fontBook, *tableStyle);

    auto wrapperStyle = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
    wrapperStyle->display = tableStyle->display;
    wrapperStyle->margin = tableStyle->margin;

    Box wrapper = {wrapperStyle, font, tableBoxEl};
    InlineBox rootInlineBox{_proseStyleFomStyle(bc.computer, *wrapperStyle)};

    // SPEC: The table wrapper box establishes a block formatting context.
    _buildTableBox(bc.toBlockContextWithoutRootInline(wrapper), tableBoxEl, tableStyle);
    wrapper.attrs = _parseDomAttr(tableBoxEl);

    return wrapper;
}

// MARK: Dispatch based on outside role -------------------------------------------------------------------------------

// https://www.w3.org/TR/css-display-3/#outer-role
static void _innerDisplayDispatchCreationOfBlockLevelBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(bc, style, el);
        bc.addToParentBox(std::move(wrapper));
    } else {
        // NOTE: FLOW-ROOT, FLEX and fallback
        auto box = createAndBuildBoxFromElement(bc, style, el, display);
        bc.addToParentBox(std::move(box));
    }
}

// https://www.w3.org/TR/css-display-3/#outer-role
static void _innerDisplayDispatchCreationOfInlineLevelBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(bc, style, el);
        bc.addToInlineRoot(std::move(wrapper));
    } else {
        // NOTE: FLOW, FLOW-ROOT, FLEX and fallback
        auto box = createAndBuildBoxFromElement(bc, style, el, display);
        bc.addToInlineRoot(std::move(box));
    }
}

// MARK: Dispatching from Node to builder based on outside role ------------------------------------------------------

static void _buildChildren(BuilderContext bc, Gc::Ref<Dom::Node> parent) {
    for (auto child = parent->firstChild(); child; child = child->nextSibling()) {
        _buildNode(bc, *child);
    }
}

static void _buildChildBoxDisplay(BuilderContext bc, Gc::Ref<Dom::Node> node, Display display) {
    if (display == Display::NONE)
        return;

    // Display::CONTENTS
    _buildChildren(bc, node);
}

// https://www.w3.org/TR/css-display-3/#layout-specific-display
static void _buildChildInternalDisplay(BuilderContext bc, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle) {
    // FIXME: We should create wrapping boxes related to table or ruby, following the FC specification. However, for now,
    // we just wrap it in a single box.
    _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, childStyle->display);
}

static void _buildChildDefaultDisplay(BuilderContext bc, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Display display) {
    if (bc.from == BuilderContext::From::FLEX) {
        display = childStyle->display = childStyle->display.blockify();
        _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, display);
        return;
    }

    // NOTE: Flow for From::BLOCK and From::INLINE
    if (display == Display::Outside::BLOCK) {
        bc.flushRootInlineBoxIntoAnonymousBox();
        _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, display);
    } else {
        if (display == Display::Inside::FLOW) {
            createAndBuildInlineFlowfromElement(bc, childStyle, child);
            return;
        }
        _innerDisplayDispatchCreationOfInlineLevelBox(bc, child, childStyle, display);
    }
}

// https://www.w3.org/TR/css-display-3/#box-generation
static void _buildNode(BuilderContext bc, Gc::Ref<Dom::Node> node) {
    logDebugIf(DEBUG_BUILDER, "building node {} at context {}", node, bc.from);
    if (auto el = node->is<Dom::Element>()) {
        auto childStyle = bc.computer.computeFor(*bc.parentStyle, *el);
        auto display = childStyle->display;

        if (display.type() == Display::Type::BOX) {
            _buildChildBoxDisplay(bc, *el, display);
        } else if (display.type() == Display::Type::INTERNAL) {
            _buildChildInternalDisplay(bc, *el, childStyle);
        } else {
            _buildChildDefaultDisplay(bc, *el, childStyle, display);
        }
    } else if (auto text = node->is<Dom::Text>()) {
        _buildText(bc, *text, bc.parentStyle);
    }
}

// MARK: Entry points -----------------------------------------------------------------

export Box build(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
    if (auto el = doc->documentElement()) {
        auto style = c.computeFor(Style::ComputedStyle::initial(), *el);
        auto font = _lookupFontface(c.fontBook, *style);

        Box root = {style, font, el};
        InlineBox rootInlineBox{_proseStyleFomStyle(c, *style)};

        BuilderContext bc{
            c,
            BuilderContext::From::BLOCK,
            style,
            root,
            &rootInlineBox,
        };

        buildBlockFlowFromElement(bc, *el);
        return root;
    }
    // NOTE: Fallback in case of an empty document
    auto style = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
    return {
        style,
        _lookupFontface(c.fontBook, *style),
        nullptr
    };
}

export Box buildForPseudoElement(Text::FontBook& fontBook, Rc<Style::ComputedStyle> style) {
    auto fontFace = _lookupFontface(fontBook, *style);

    // FIXME: We should pass this around from the top in order to properly resolve rems
    Resolver resolver{
        .rootFont = Text::Font{fontFace, 16},
        .boxFont = Text::Font{fontFace, 16},
    };
    Text::ProseStyle proseStyle{
        .font = {
            fontFace,
            resolver.resolve(style->font->size).cast<f64>(),
        },
        .multiline = true,
    };

    auto prose = makeRc<Text::Prose>(proseStyle);
    if (style->content) {
        prose->append(style->content.str());
        return {style, fontFace, InlineBox{prose}, nullptr};
    }

    return {style, fontFace, nullptr};
}

} // namespace Vaev::Layout
