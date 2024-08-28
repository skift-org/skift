#include <karm-text/loader.h>
#include <vaev-paint/borders.h>
#include <vaev-paint/box.h>
#include <vaev-paint/text.h>

#include "block.h"
#include "flex.h"
#include "frag.h"
#include "grid.h"
#include "table.h"
#include "values.h"

namespace Vaev::Layout {

// MARK: Frag ------------------------------------------------------------------

Frag::Frag(Strong<Style::Computed> style, Text::Font font)
    : style{std::move(style)}, font{font} {}

Frag::Frag(Strong<Style::Computed> style, Text::Font font, Content content)
    : style{std::move(style)}, font{font}, content{std::move(content)} {}

Karm::Slice<Frag> Frag::children() const {
    if (auto *frags = content.is<Vec<Frag>>())
        return *frags;
    return {};
}

Karm::MutSlice<Frag> Frag::children() {
    if (auto *frags = content.is<Vec<Frag>>()) {
        return *frags;
    }
    return {};
}

void Frag::add(Frag &&frag) {
    if (content.is<None>())
        content = Vec<Frag>{};

    if (auto *frags = content.is<Vec<Frag>>()) {
        frags->pushBack(std::move(frag));
    }
}

void Frag::repr(Io::Emit &e) const {
    if (children()) {
        e("(flow {} {}", style->display, layout.borderBox());
        e.indentNewline();
        for (auto &c : children()) {
            c.repr(e);
            e.newline();
        }
        e.deindent();
        e(")");
    } else {
        e("(frag {} {})", style->display, layout.borderBox());
    }
}

// MARK: Build -----------------------------------------------------------------

static Opt<Strong<Text::Fontface>> _regularFontface = NONE;

static Strong<Text::Fontface> regularFontface() {
    if (not _regularFontface)
        _regularFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url).unwrap();
    return *_regularFontface;
}

static void buildChildren(Style::Computer &c, Vec<Strong<Dom::Node>> const &children, Frag &parent) {
    for (auto &child : children) {
        build(c, *child, parent);
    }
}

static void buildElement(Style::Computer &c, Dom::Element const &el, Frag &parent) {
    auto style = c.computeFor(*parent.style, el);
    auto font = Text::Font{regularFontface(), 16};

    if (el.tagName == Html::IMG) {
        Image::Picture img = Gfx::Surface::fallback();
        parent.add({style, font, img});
        return;
    }

    auto display = style->display;

    if (display == Display::NONE)
        return;

    if (display == Display::CONTENTS) {
        buildChildren(c, el.children(), parent);
        return;
    }

    Frag frag = {style, font};
    buildChildren(c, el.children(), frag);
    parent.add(std::move(frag));
}

static void buildRun(Style::Computer &, Dom::Text const &node, Frag &parent) {
    auto style = makeStrong<Style::Computed>(Style::Computed::initial());
    style->inherit(*parent.style);

    auto font = Text::Font{regularFontface(), 16};
    Io::SScan scan{node.data};
    scan.eat(Re::space());
    if (scan.ended())
        return;
    auto run = makeStrong<Text::Run>(font);
    while (not scan.ended()) {
        run->append(scan.next());
        if (scan.eat(Re::space())) {
            run->append(' ');
        }
    }

    parent.add({style, font, run});
}

void build(Style::Computer &c, Dom::Node const &node, Frag &parent) {
    if (auto *el = node.is<Dom::Element>()) {
        buildElement(c, *el, parent);
    } else if (auto *text = node.is<Dom::Text>()) {
        buildRun(c, *text, parent);
    } else if (auto *doc = node.is<Dom::Document>()) {
        buildChildren(c, doc->children(), parent);
    }
}

Frag build(Style::Computer &c, Dom::Document const &doc) {
    auto font = Text::Font{regularFontface(), 16};
    Frag root = {makeStrong<Style::Computed>(Style::Computed::initial()), font};
    build(c, doc, root);
    return root;
}

// MARK: Layout ----------------------------------------------------------------

Output _contentLayout(Tree &t, Frag &f, Input input) {
    auto display = f.style->display;

    if (auto *run = f.content.is<Strong<Text::Run>>()) {
        return Output::fromSize((*run)->layout().cast<Px>());
    } else if (display == Display::FLOW or display == Display::FLOW_ROOT) {
        return blockLayout(t, f, input);
    } else if (display == Display::FLEX) {
        return flexLayout(t, f, input);
    } else if (display == Display::GRID) {
        return gridLayout(t, f, input);
    } else if (display == Display::TABLE) {
        return tableLayout(t, f, input);
    } else {
        return blockLayout(t, f, input);
    }
}

static InsetsPx _computeMargins(Tree &t, Frag &f, Input input) {
    InsetsPx res;
    auto margin = f.style->margin;

    res.top = resolve(t, f, margin->top, input.containingBlock.height);
    res.end = resolve(t, f, margin->end, input.containingBlock.width);
    res.bottom = resolve(t, f, margin->bottom, input.containingBlock.height);
    res.start = resolve(t, f, margin->start, input.containingBlock.width);

    return res;
}

static InsetsPx _computeBorders(Tree &t, Frag &f) {
    InsetsPx res;
    auto borders = f.style->borders;

    if (borders->top.style != BorderStyle::NONE)
        res.top = resolve(t, f, borders->top.width);

    if (borders->end.style != BorderStyle::NONE)
        res.end = resolve(t, f, borders->end.width);

    if (borders->bottom.style != BorderStyle::NONE)
        res.bottom = resolve(t, f, borders->bottom.width);

    if (borders->start.style != BorderStyle::NONE)
        res.start = resolve(t, f, borders->start.width);

    return res;
}

static InsetsPx _computePaddings(Tree &t, Frag &f, Input input) {
    InsetsPx res;
    auto padding = f.style->padding;

    res.top = resolve(t, f, padding->top, input.containingBlock.height);
    res.end = resolve(t, f, padding->end, input.containingBlock.width);
    res.bottom = resolve(t, f, padding->bottom, input.containingBlock.height);
    res.start = resolve(t, f, padding->start, input.containingBlock.width);

    return res;
}

static Math::Radii<Px> _computeRadii(Tree &t, Frag &f, Vec2Px size) {
    auto radii = f.style->borders->radii;
    Math::Radii<Px> res;

    res.a = resolve(t, f, radii.a, size.height);
    res.b = resolve(t, f, radii.b, size.width);
    res.c = resolve(t, f, radii.c, size.width);
    res.d = resolve(t, f, radii.d, size.height);
    res.e = resolve(t, f, radii.e, size.height);
    res.f = resolve(t, f, radii.f, size.width);
    res.g = resolve(t, f, radii.g, size.width);
    res.h = resolve(t, f, radii.h, size.height);

    return res;
}

static Cons<Opt<Px>, IntrinsicSize> _computeSpecifiedSize(Tree &t, Frag &f, Input input, Size size) {
    if (size == Size::MIN_CONTENT) {
        return {NONE, IntrinsicSize::MIN_CONTENT};
    } else if (size == Size::MAX_CONTENT) {
        return {NONE, IntrinsicSize::MAX_CONTENT};
    } else if (size == Size::AUTO) {
        return {NONE, IntrinsicSize::AUTO};
    } else if (size == Size::FIT_CONTENT) {
        return {NONE, IntrinsicSize::STRETCH_TO_FIT};
    } else if (size == Size::LENGTH) {
        return {resolve(t, f, size.value, input.containingBlock.width), IntrinsicSize::AUTO};
    } else {
        logWarn("unknown specified size: {}", size);
        return {Px{0}, IntrinsicSize::AUTO};
    }
}

Output layout(Tree &t, Frag &f, Input input) {
    auto margin = _computeMargins(t, f, input);
    auto borders = _computeBorders(t, f);
    auto padding = _computePaddings(t, f, input);
    auto sizing = f.style->sizing;

    auto [specifiedWidth, widthIntrinsicSize] = _computeSpecifiedSize(t, f, input, sizing->width);
    if (input.knownSize.width == NONE) {
        input.knownSize.width = specifiedWidth;
    }
    input.knownSize.width = input.knownSize.width.map([&](auto s) {
        // FIXME: Take box-sizing into account
        return s - padding.horizontal() - borders.horizontal();
    });
    input.intrinsic = widthIntrinsicSize;

    auto [specifiedHeight, heightIntrinsicSize] = _computeSpecifiedSize(t, f, input, sizing->height);
    if (input.knownSize.height == NONE) {
        input.knownSize.height = specifiedHeight;
    }

    input.knownSize.height = input.knownSize.height.map([&](auto s) {
        // FIXME: Take box-sizing into account
        return s - padding.vertical() - borders.vertical();
    });
    input.intrinsic = heightIntrinsicSize;

    auto [size, _] = _contentLayout(t, f, input);

    size.width = input.knownSize.width.unwrapOr(size.width);
    size.height = input.knownSize.height.unwrapOr(size.height);

    size = size + padding.all() + borders.all();

    if (input.commit == Commit::YES) {
        f.layout.size = size;
        f.layout.padding = padding;
        f.layout.borders = borders;
        f.layout.margin = margin;
        f.layout.radii = _computeRadii(t, f, size);
    }

    return Output::fromSizeAndMargin(size, margin);
}

// MARK: Paint -----------------------------------------------------------------

static void _paintInner(Frag &frag, Paint::Stack &stack, Math::Vec2f pos) {
    auto const &backgrounds = frag.style->backgrounds;

    Gfx::Color currentColor = Gfx::BLACK;
    currentColor = resolve(frag.style->color, currentColor);

    if (backgrounds.len()) {
        Paint::Box paint;

        paint.backgrounds.ensure(backgrounds.len());
        for (auto &bg : backgrounds) {
            paint.backgrounds.pushBack(resolve(bg.fill, currentColor));
        }

        paint.radii = frag.layout.radii.cast<f64>();
        paint.bound = frag.layout.borderBox().cast<f64>().offset(pos);

        stack.add(makeStrong<Paint::Box>(std::move(paint)));
    }

    for (auto &c : frag.children()) {
        paint(c, stack, pos + frag.layout.contentBox().topStart().cast<f64>());
    }

    if (auto *run = frag.content.is<Strong<Text::Run>>()) {
        Math::Vec2f baseline = {0, frag.font.metrics().ascend};
        stack.add(makeStrong<Paint::Text>(
            pos + frag.layout.borderBox().topStart().cast<f64>() + baseline,
            *run,
            currentColor
        ));
    }

    if (not frag.layout.borders.zero()) {
        Paint::Borders paint;
        auto bordersLayout = frag.layout.borders;
        auto bordersStyle = frag.style->borders;

        paint = Paint::Borders();
        paint.radii = frag.layout.radii.cast<f64>();
        paint.bound = frag.layout.paddingBox().cast<f64>().offset(pos);

        paint.top.width = bordersLayout.top.cast<f64>();
        paint.top.style = bordersStyle->top.style;
        paint.top.fill = resolve(bordersStyle->top.color, currentColor);

        paint.bottom.width = bordersLayout.bottom.cast<f64>();
        paint.bottom.style = bordersStyle->bottom.style;
        paint.bottom.fill = resolve(bordersStyle->bottom.color, currentColor);

        paint.start.width = bordersLayout.start.cast<f64>();
        paint.start.style = bordersStyle->start.style;
        paint.start.fill = resolve(bordersStyle->start.color, currentColor);

        paint.end.width = bordersLayout.end.cast<f64>();
        paint.end.style = bordersStyle->end.style;
        paint.end.fill = resolve(bordersStyle->end.color, currentColor);

        stack.add(makeStrong<Paint::Borders>(std::move(paint)));
    }
}

void paint(Frag &frag, Paint::Stack &stack, Math::Vec2f pos) {
    if (frag.style->zIndex == ZIndex::AUTO) {
        _paintInner(frag, stack, pos);
    } else {
        auto innerStack = makeStrong<Paint::Stack>();
        innerStack->zIndex = frag.style->zIndex.value;
        _paintInner(frag, *innerStack, pos);
        stack.add(std::move(innerStack));
    }
}

void wireframe(Frag &frag, Gfx::Canvas &g) {
    for (auto &c : frag.children())
        wireframe(c, g);

    g.strokeStyle({
        .fill = Gfx::BLACK,
        .width = 1,
        .align = Gfx::INSIDE_ALIGN,
    });

    g.stroke(frag.layout.borderBox().cast<f64>());
}

} // namespace Vaev::Layout
