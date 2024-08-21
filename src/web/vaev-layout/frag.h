#pragma once

#include <karm-image/picture.h>
#include <karm-text/run.h>
#include <vaev-dom/document.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computed.h>
#include <vaev-style/computer.h>

#include "base.h"
#include "box.h"

namespace Vaev::Layout {

// MARK: Frag ------------------------------------------------------------------

using Content = Union<
    None,
    Vec<Frag>,
    Strong<Text::Run>,
    Image::Picture>;

struct Frag : public Meta::NoCopy {
    Strong<Style::Computed> style;
    Text::Font font;
    Content content = NONE;
    Box box{};

    Frag(Strong<Style::Computed> style, Text::Font font);

    Frag(Strong<Style::Computed> style, Text::Font font, Content content);

    Slice<Frag> children() const;

    MutSlice<Frag> children();

    Style::Computed const *operator->() const {
        return &*style;
    }

    void add(Frag &&frag);

    void repr(Io::Emit &e) const;
};

struct Tree {
    Frag root;
    Viewport viewport;
};

// MARK: Build -----------------------------------------------------------------

void build(Style::Computer &c, Dom::Node const &n, Frag &parent);

Frag build(Style::Computer &c, Dom::Document const &doc);

// MARK: Layout ----------------------------------------------------------------

Output layout(Tree &t, Frag &f, Box box, Input input);

Px measure(Tree &t, Frag &f, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

// MARK: Paint -----------------------------------------------------------------

void paint(Frag &frag, Paint::Stack &stack);

void wireframe(Frag &frag, Gfx::Canvas &g);

} // namespace Vaev::Layout
