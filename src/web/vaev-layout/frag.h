#pragma once

#include <karm-image/picture.h>
#include <karm-text/run.h>
#include <vaev-markup/dom.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computed.h>
#include <vaev-style/computer.h>

#include "base.h"

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
    Layout layout;

    Frag(Strong<Style::Computed> style, Text::Font font);

    Frag(Strong<Style::Computed> style, Text::Font font, Content content);

    Slice<Frag> children() const;

    MutSlice<Frag> children();

    void add(Frag &&frag);

    void repr(Io::Emit &e) const;
};

struct Tree {
    Frag root;
    Viewport viewport;
};

// MARK: Build -----------------------------------------------------------------

void build(Style::Computer &c, Markup::Node const &n, Frag &parent);

Frag build(Style::Computer &c, Markup::Document const &doc);

// MARK: Layout ----------------------------------------------------------------

Output layout(Tree &t, Frag &f, Input input);

Px measure(Tree &t, Frag &f, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

// MARK: Paint -----------------------------------------------------------------

void paint(Frag &frag, Paint::Stack &stack, Math::Vec2f pos = {});

void wireframe(Frag &frag, Gfx::Canvas &g);

} // namespace Vaev::Layout
