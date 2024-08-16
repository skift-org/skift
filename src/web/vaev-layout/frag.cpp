#include <karm-text/loader.h>
#include <vaev-paint/borders.h>
#include <vaev-paint/box.h>
#include <vaev-paint/text.h>

#include "block.h"
#include "context.h"
#include "flex.h"
#include "frag.h"
#include "grid.h"
#include "sizing.h"
#include "table.h"

namespace Vaev::Layout {

// MARK: Frag ------------------------------------------------------------------

Frag::Frag(Strong<Style::Computed> style)
    : style{std::move(style)} {}

Frag::Frag(Strong<Style::Computed> style, Content content)
    : style{std::move(style)}, content{std::move(content)} {}

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
        e("(flow {} {}", style->display, box.borderBox);
        e.indentNewline();
        for (auto &c : children()) {
            c.repr(e);
            e.newline();
        }
        e.deindent();
        e(")");
    } else {
        e("(frag {} {})", style->display, box.borderBox);
    }
}

// MARK: Build -----------------------------------------------------------------

static void buildChildren(Style::Computer &c, Vec<Strong<Dom::Node>> const &children, Frag &parent) {
    for (auto &child : children) {
        build(c, *child, parent);
    }
}

static void buildElement(Style::Computer &c, Dom::Element const &el, Frag &parent) {
    auto style = c.computeFor(*parent.style, el);

    if (el.tagName == Html::IMG) {
        Image::Picture img = Gfx::Surface::fallback();
        parent.add({style, img});
        return;
    }

    auto display = style->display;

    if (display == Display::NONE)
        return;

    if (display == Display::CONTENTS) {
        buildChildren(c, el.children(), parent);
        return;
    }

    Frag frag = {style};
    buildChildren(c, el.children(), frag);
    parent.add(std::move(frag));
}

static Opt<Strong<Text::Fontface>> _regularFontface = NONE;

static Strong<Text::Fontface> regularFontface() {
    if (not _regularFontface) {
        _regularFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url).unwrap();
    }
    return *_regularFontface;
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

    parent.add({style, run});
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
    Frag root = makeStrong<Style::Computed>();
    build(c, doc, root);
    return root;
}

// MARK: Layout ----------------------------------------------------------------

Output layout(Context &ctx, Box box, Input input) {
    auto display = ctx.style().display;

    if (auto *run = ctx.frag.content.is<Strong<Text::Run>>()) {
        if (input.commit == Commit::YES)
            ctx.frag.box = box;
        return Output::fromSize((*run)->layout().cast<Px>());
    } else if (display == Display::FLOW or display == Display::FLOW_ROOT) {
        return blockLayout(ctx, box, input);
    } else if (display == Display::FLEX) {
        return flexLayout(ctx, box, input);
    } else if (display == Display::GRID) {
        return gridLayout(ctx, box, input);
    } else if (display == Display::TABLE) {
        return tableLayout(ctx, box, input);
    } else {
        return blockLayout(ctx, box, input);
    }
}

Px measure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px availableSpace) {
    Box box = computeBox(
        ctx,
        {
            axis == Axis::HORIZONTAL ? availableSpace : Px{0},
            axis == Axis::VERTICAL ? availableSpace : Px{0},
        }
    );

    auto ouput = layout(
        ctx,
        box,
        {.commit = Commit::NO,
         .axis = axis,
         .intrinsic = intrinsic,
         .availableSpace = box.borderBox.wh
        }
    );

    return ouput.size[axis.index()];
}

// MARK: Paint -----------------------------------------------------------------

void paint(Frag &frag, Paint::Stack &stack) {
    if (frag->backgrounds.len()) {
        Paint::Box background;

        background.backgrounds = frag->backgrounds;
        background.bound = frag.box.borderBox;
        stack.add(makeStrong<Paint::Box>(std::move(background)));
    }

    for (auto &c : frag.children()) {
        paint(c, stack);
    }

    if (auto *run = frag.content.is<Strong<Text::Run>>()) {
        stack.add(makeStrong<Paint::Text>(frag.box.borderBox.topStart().cast<f64>(), *run));
    }

    if (not frag.box.borders.zero()) {
        // FIXME colorContext should be context dependant
        ColorContext colorContext;

        Paint::Borders borders;

        borders = Paint::Borders();
        borders.radii = frag.box.radii.cast<f64>();

        borders.top.width = frag.box.borders.top.cast<f64>();
        borders.top.style = frag->borders->top.style;
        borders.top.fill = colorContext.resolve(frag->borders->top.color);

        borders.bottom.width = frag.box.borders.bottom.cast<f64>();
        borders.bottom.style = frag->borders->bottom.style;
        borders.bottom.fill = colorContext.resolve(frag->borders->bottom.color);

        borders.start.width = frag.box.borders.start.cast<f64>();
        borders.start.style = frag->borders->start.style;
        borders.start.fill = colorContext.resolve(frag->borders->start.color);

        borders.end.width = frag.box.borders.end.cast<f64>();
        borders.end.style = frag->borders->end.style;
        borders.end.fill = colorContext.resolve(frag->borders->end.color);

        stack.add(makeStrong<Paint::Borders>(std::move(borders)));
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

    g.stroke(frag.box.borderBox.cast<f64>());
}

} // namespace Vaev::Layout
