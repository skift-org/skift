#include <karm-base/clamp.h>
#include <karm-text/loader.h>

#include "frag.h"

namespace Vaev::Layout {

// MARK: Frag ------------------------------------------------------------------

Frag::Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> font)
    : style{std::move(style)}, fontFace{font} {}

Frag::Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> font, Content content)
    : style{std::move(style)}, fontFace{font}, content{std::move(content)} {}

Karm::Slice<Frag> Frag::children() const {
    if (auto frags = content.is<Vec<Frag>>())
        return *frags;
    return {};
}

Karm::MutSlice<Frag> Frag::children() {
    if (auto frags = content.is<Vec<Frag>>()) {
        return *frags;
    }
    return {};
}

void Frag::add(Frag &&frag) {
    if (content.is<None>())
        content = Vec<Frag>{};

    if (auto frags = content.is<Vec<Frag>>()) {
        frags->pushBack(std::move(frag));
    }
}

void Frag::repr(Io::Emit &e) const {
    if (children()) {
        e("(flow {} {} {} {}", attrs, style->display, style->position, layout.borderBox());
        e.indentNewline();
        for (auto &c : children()) {
            c.repr(e);
            e.newline();
        }
        e.deindent();
        e(")");
    } else {
        e("(frag {} {} {} {})", attrs, style->display, style->position, layout.borderBox());
    }
}

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

// MARK: Build -----------------------------------------------------------------

static Opt<Strong<Karm::Text::Fontface>> _regularFontface = NONE;
static Opt<Strong<Karm::Text::Fontface>> _boldFontface = NONE;

static Strong<Karm::Text::Fontface> _lookupFontface(Style::Computed &style) {
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

void _buildChildren(Style::Computer &c, Vec<Strong<Markup::Node>> const &children, Frag &parent) {
    for (auto &child : children) {
        _buildNode(c, *child, parent);
    }
}

static void _buildTableChildren(Style::Computer &c, Vec<Strong<Markup::Node>> const &children, Frag &tableWrapperBox, Strong<Style::Computed> tableBoxStyle) {
    Frag tableBox{
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

static void _buildElement(Style::Computer &c, Markup::Element const &el, Frag &parent) {
    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(*style);

    if (el.tagName == Html::IMG) {
        Image::Picture img = Gfx::Surface::fallback();
        parent.add({style, font, img});
        return;
    }

    auto display = style->display;

    if (display == Display::NONE)
        return;

    if (display == Display::CONTENTS) {
        _buildChildren(c, el.children(), parent);
        return;
    }

    auto buildFrag = [](Style::Computer &c, Markup::Element const &el, Strong<Karm::Text::Fontface> font, Strong<Style::Computed> style) {
        if (el.tagName == Html::TagId::TABLE) {

            auto wrapperStyle = makeStrong<Style::Computed>(Style::Computed::initial());
            wrapperStyle->display = style->display;
            wrapperStyle->margin = style->margin;

            Frag wrapper = {wrapperStyle, font};
            _buildTableChildren(c, el.children(), wrapper, style);
            return wrapper;
        } else {
            Frag frag = {style, font};
            _buildChildren(c, el.children(), frag);
            return frag;
        }
    };

    auto frag = buildFrag(c, el, font, style);
    frag.attrs = _parseDomAttr(el);
    parent.add(std::move(frag));
}

static void _buildRun(Style::Computer &, Markup::Text const &node, Frag &parent) {
    auto style = makeStrong<Style::Computed>(Style::Computed::initial());
    style->inherit(*parent.style);

    auto font = _lookupFontface(*style);
    Io::SScan scan{node.data};
    scan.eat(Re::space());
    if (scan.ended())
        return;
    Karm::Text::Run run;

    while (not scan.ended()) {
        switch (style->text->transform) {
        case TextTransform::UPPERCASE:
            run.append(toAsciiUpper(scan.next()));
            break;

        case TextTransform::LOWERCASE:
            run.append(toAsciiLower(scan.next()));
            break;

        case TextTransform::NONE:
            run.append(scan.next());
            break;

        default:
            break;
        }

        if (scan.eat(Re::space())) {
            run.append(' ');
        }
    }

    parent.add({style, font, std::move(run)});
}

void _buildNode(Style::Computer &c, Markup::Node const &node, Frag &parent) {
    if (auto el = node.is<Markup::Element>()) {
        _buildElement(c, *el, parent);
    } else if (auto text = node.is<Markup::Text>()) {
        _buildRun(c, *text, parent);
    } else if (auto doc = node.is<Markup::Document>()) {
        _buildChildren(c, doc->children(), parent);
    }
}

Frag build(Style::Computer &c, Markup::Document const &doc) {
    auto style = makeStrong<Style::Computed>(Style::Computed::initial());
    Frag root = {style, _lookupFontface(*style)};
    _buildNode(c, doc, root);
    return root;
}

} // namespace Vaev::Layout
