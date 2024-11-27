#include <karm-image/loader.h>
#include <karm-text/loader.h>

#include "builder.h"
#include "values.h"

namespace Vaev::Layout {

static void _buildNode(Style::Computer &c, Markup::Node const &node, Box &parent);

// MARK: Attributes ------------------------------------------------------------

static Opt<Str> _parseStrAttr(Markup::Element const &el, AttrName name) {
    return el.getAttribute(name);
}

static Opt<usize> _parseUsizeAttr(Markup::Element const &el, AttrName name) {
    auto str = _parseStrAttr(el, name);
    if (not str)
        return NONE;
    return Io::atoi(str.unwrap());
}

static Attrs _parseDomAttr(Markup::Element const &el) {
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

static Opt<Strong<Karm::Text::Fontface>> _monospaceFontface = NONE;
static Opt<Strong<Karm::Text::Fontface>> _regularFontface = NONE;
static Opt<Strong<Karm::Text::Fontface>> _boldFontface = NONE;

static Strong<Karm::Text::Fontface> _lookupFontface(Style::Computed &style) {
    if (contains(style.font->families, "monospace"s)) {
        if (not _monospaceFontface)
            _monospaceFontface = Karm::Text::loadFontfaceOrFallback("bundle://fonts-fira-code/fonts/FiraCode-Regular.ttf"_url).unwrap();
        return *_monospaceFontface;
    } else if (style.font->style != FontStyle::NORMAL) {
        if (style.font->weight != FontWeight::NORMAL) {
            if (not _boldFontface)
                _boldFontface = Karm::Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-BoldItalic.ttf"_url).unwrap();
            return *_boldFontface;
        } else {
            if (not _regularFontface)
                _regularFontface = Karm::Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Italic.ttf"_url).unwrap();
            return *_regularFontface;
        }
    } else {
        if (style.font->weight != FontWeight::NORMAL) {
            if (not _boldFontface)
                _boldFontface = Karm::Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Bold.ttf"_url).unwrap();
            return *_boldFontface;
        } else {
            if (not _regularFontface)
                _regularFontface = Karm::Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url).unwrap();
            return *_regularFontface;
        }
    }
}

auto RE_SEGMENT_BREAK = Re::single('\n', '\r', '\f', '\v');

static void _buildRun(Style::Computer &, Markup::Text const &node, Box &parent) {
    auto style = makeStrong<Style::Computed>(Style::Computed::initial());
    style->inherit(*parent.style);

    auto fontFace = _lookupFontface(*style);
    Io::SScan scan{node.data};
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

    auto prose = makeStrong<Text::Prose>(proseStyle);
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

void _buildChildren(Style::Computer &c, Vec<Strong<Markup::Node>> const &children, Box &parent) {
    for (auto &child : children) {
        _buildNode(c, *child, parent);
    }
}

static void _buildBlock(Style::Computer &c, Strong<Style::Computed> style, Markup::Element const &el, Box &parent) {
    auto font = _lookupFontface(*style);
    Box box = {style, font};
    _buildChildren(c, el.children(), box);
    box.attrs = _parseDomAttr(el);
    parent.add(std::move(box));
}

// MARK: Build Replace ---------------------------------------------------------

static void _buildImage(Style::Computer &c, Markup::Element const &el, Box &parent) {
    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(*style);

    auto src = el.getAttribute(Html::SRC_ATTR).unwrapOr(""s);
    auto url = Mime::Url::parse(src);
    auto img = Karm::Image::loadOrFallback(url).unwrap();
    parent.add({style, font, img});
}

// MARK: Build Table -----------------------------------------------------------

static void _buildTableChildren(Style::Computer &c, Vec<Strong<Markup::Node>> const &children, Box &tableWrapperBox, Strong<Style::Computed> tableBoxStyle) {
    Box tableBox{
        tableBoxStyle, tableWrapperBox.fontFace
    };

    tableBox.style->display = Display::Internal::TABLE_BOX;

    for (auto &child : children) {
        if (auto el = child->is<Markup::Element>()) {
            if (el->tagName == Html::CAPTION) {
                _buildNode(c, *child, tableWrapperBox);
            } else {
                _buildNode(c, *child, tableBox);
            }
        }
    }
    tableWrapperBox.add(std::move(tableBox));
}

static void _buildTable(Style::Computer &c, Strong<Style::Computed> style, Markup::Element const &el, Box &parent) {
    auto font = _lookupFontface(*style);

    auto wrapperStyle = makeStrong<Style::Computed>(Style::Computed::initial());
    wrapperStyle->display = style->display;
    wrapperStyle->margin = style->margin;

    Box wrapper = {wrapperStyle, font};
    _buildTableChildren(c, el.children(), wrapper, style);
    wrapper.attrs = _parseDomAttr(el);

    parent.add(std::move(wrapper));
}

// MARK: Build -----------------------------------------------------------------

static void _buildElement(Style::Computer &c, Markup::Element const &el, Box &parent) {
    if (el.tagName == Html::IMG) {
        _buildImage(c, el, parent);
        return;
    }

    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(*style);

    auto display = style->display;

    if (display == Display::NONE) {
        // Do nothing
    } else if (display == Display::CONTENTS) {
        _buildChildren(c, el.children(), parent);
    } else if (display == Display::TABLE) {
        _buildTable(c, style, el, parent);
    } else {
        _buildBlock(c, style, el, parent);
    }
}

static void _buildNode(Style::Computer &c, Markup::Node const &node, Box &parent) {
    if (auto el = node.is<Markup::Element>()) {
        _buildElement(c, *el, parent);
    } else if (auto text = node.is<Markup::Text>()) {
        _buildRun(c, *text, parent);
    } else if (auto doc = node.is<Markup::Document>()) {
        _buildChildren(c, doc->children(), parent);
    }
}

Box build(Style::Computer &c, Markup::Document const &doc) {
    auto style = makeStrong<Style::Computed>(Style::Computed::initial());
    Box root = {style, _lookupFontface(*style)};
    _buildNode(c, doc, root);
    return root;
}

} // namespace Vaev::Layout
