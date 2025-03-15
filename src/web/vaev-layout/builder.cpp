module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:builder;

import :values;

namespace Vaev::Layout {

static void _buildNode(Style::Computer& c, Gc::Ref<Dom::Node> node, Box& parent);

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

// MARK: Build Inline ----------------------------------------------------------

static Opt<Rc<Karm::Text::Fontface>> _monospaceFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _regularFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _boldFontface = NONE;

static Rc<Karm::Text::Fontface> _lookupFontface(Text::FontBook& fontBook, Style::Computed& style) {
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

auto RE_SEGMENT_BREAK = Re::single('\n', '\r', '\f', '\v');

static void _buildRun(Style::Computer& c, Gc::Ref<Dom::Text> node, Box& parent) {
    auto style = makeRc<Style::Computed>(Style::Computed::initial());
    style->inherit(*parent.style);

    auto fontFace = _lookupFontface(c.fontBook, *style);
    Io::SScan scan{node->data()};
    scan.eat(Re::space());
    if (scan.ended())
        return;

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

    switch (style->text->align) {
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

    auto prose = makeRc<Text::Prose>(proseStyle);
    auto whitespace = style->text->whiteSpace;

    while (not scan.ended()) {
        switch (style->text->transform) {
        case TextTransform::UPPERCASE:
            prose->append(toAsciiUpper(scan.next()));
            break;

        case TextTransform::LOWERCASE:
            prose->append(toAsciiLower(scan.next()));
            break;

        case TextTransform::NONE:
            prose->append(scan.next());
            break;

        default:
            break;
        }

        if (whitespace == WhiteSpace::PRE) {
            auto tok = scan.token(Re::space());
            if (tok)
                prose->append(tok);
        } else if (whitespace == WhiteSpace::PRE_LINE) {
            bool hasBlank = false;
            if (scan.eat(Re::blank())) {
                hasBlank = true;
            }

            if (scan.eat(RE_SEGMENT_BREAK)) {
                prose->append('\n');
                scan.eat(Re::blank());
                hasBlank = false;
            }

            if (hasBlank)
                prose->append(' ');
        } else {
            // NORMAL
            if (scan.eat(Re::space()))
                prose->append(' ');
        }
    }

    parent.add({style, fontFace, std::move(prose)});
}

// MARK: Build Block -----------------------------------------------------------

void _buildChildren(Style::Computer& c, Gc::Ref<Dom::Node> node, Box& parent) {
    for (auto child = node->firstChild(); child; child = child->nextSibling()) {
        _buildNode(c, *child, parent);
    }
}

static void _buildBlock(Style::Computer& c, Rc<Style::Computed> style, Gc::Ref<Dom::Element> el, Box& parent) {
    auto font = _lookupFontface(c.fontBook, *style);
    Box box = {style, font};
    _buildChildren(c, el, box);
    box.attrs = _parseDomAttr(el);
    parent.add(std::move(box));
}

// MARK: Build Replace ---------------------------------------------------------

static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Box& parent) {
    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(c.fontBook, *style);

    auto src = el->getAttribute(Html::SRC_ATTR).unwrapOr(""s);
    auto url = Mime::Url::resolveReference(el->baseURI(), Mime::parseUrlOrPath(src))
                   .unwrapOr("bundle://vaev-driver/missing.qoi"_url);

    auto img = Karm::Image::load(url).unwrapOrElse([] {
        return Karm::Image::loadOrFallback("bundle://vaev-driver/missing.qoi"_url).unwrap();
    });
    parent.add({style, font, img});
}

// MARK: Build Table -----------------------------------------------------------

static void _buildTableChildren(Style::Computer& c, Gc::Ref<Dom::Node> node, Box& tableWrapperBox, Rc<Style::Computed> tableBoxStyle) {
    Box tableBox{
        tableBoxStyle,
        tableWrapperBox.fontFace,
    };

    tableBox.style->display = Display::Internal::TABLE_BOX;

    bool captionsOnTop = tableBox.style->table->captionSide == CaptionSide::TOP;

    if (captionsOnTop) {
        for (auto child = node->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                if (el->tagName == Html::CAPTION) {
                    _buildNode(c, *el, tableWrapperBox);
                }
            }
        }
    }

    for (auto child = node->firstChild(); child; child = child->nextSibling()) {
        if (auto el = child->is<Dom::Element>()) {
            if (el->tagName != Html::CAPTION) {
                _buildNode(c, *el, tableBox);
            }
        }
    }
    tableWrapperBox.add(std::move(tableBox));

    if (not captionsOnTop) {
        for (auto child = node->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                if (el->tagName == Html::CAPTION) {
                    _buildNode(c, *el, tableWrapperBox);
                }
            }
        }
    }
}

static void _buildTable(Style::Computer& c, Rc<Style::Computed> style, Gc::Ref<Dom::Element> el, Box& parent) {
    auto font = _lookupFontface(c.fontBook, *style);

    auto wrapperStyle = makeRc<Style::Computed>(Style::Computed::initial());
    wrapperStyle->display = style->display;
    wrapperStyle->margin = style->margin;

    Box wrapper = {wrapperStyle, font};
    _buildTableChildren(c, el, wrapper, style);
    wrapper.attrs = _parseDomAttr(el);

    parent.add(std::move(wrapper));
}

// MARK: Build -----------------------------------------------------------------

static void _buildElement(Style::Computer& c, Gc::Ref<Dom::Element> el, Box& parent) {
    if (el->tagName == Html::IMG) {
        _buildImage(c, el, parent);
        return;
    }

    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(c.fontBook, *style);

    auto display = style->display;

    if (display == Display::NONE) {
        // Do nothing
    } else if (display == Display::CONTENTS) {
        _buildChildren(c, el, parent);
    } else if (display == Display::TABLE) {
        _buildTable(c, style, el, parent);
    } else {
        _buildBlock(c, style, el, parent);
    }
}

static void _buildNode(Style::Computer& c, Gc::Ref<Dom::Node> node, Box& parent) {
    if (auto el = node->is<Dom::Element>()) {
        _buildElement(c, *el, parent);
    } else if (auto text = node->is<Dom::Text>()) {
        _buildRun(c, *text, parent);
    } else if (auto doc = node->is<Dom::Document>()) {
        _buildChildren(c, *doc, parent);
    }
}

export Box build(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
    if (auto el = doc->documentElement()) {
        auto style = c.computeFor(Style::Computed::initial(), *el);
        auto font = _lookupFontface(c.fontBook, *style);
        Box root = {style, _lookupFontface(c.fontBook, *style)};
        _buildChildren(c, *el, root);
        return root;
    }
    // NOTE: Fallback in case of an empty document
    auto style = makeRc<Style::Computed>(Style::Computed::initial());
    return {
        style,
        _lookupFontface(c.fontBook, *style),
    };
}

export Box buildForPseudoElement(Text::FontBook& fontBook, Rc<Style::Computed> style) {
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
        return {style, fontFace, prose};
    }

    return {style, fontFace};
}

} // namespace Vaev::Layout
