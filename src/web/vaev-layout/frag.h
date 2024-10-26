#pragma once

#include <karm-image/picture.h>
#include <karm-text/run.h>
#include <vaev-markup/dom.h>
#include <vaev-style/computer.h>

#include "base.h"

namespace Vaev::Layout {

// MARK: Frag ------------------------------------------------------------------

using Content = Union<
    None,
    Vec<Frag>,
    Karm::Text::Run,
    Image::Picture>;

struct Attrs {
    usize span = 1;
    usize rowSpan = 1;
    usize colSpan = 1;

    void repr(Io::Emit &e) const {
        e("(attrs span: {} rowSpan: {} colSpan: {})", span, rowSpan, colSpan);
    }
};

struct Frag : public Meta::NoCopy {
    Strong<Style::Computed> style;
    Strong<Karm::Text::Fontface> fontFace;
    Content content = NONE;
    Layout layout;
    Attrs attrs;

    Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> fontFace);

    Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> fontFace, Content content);

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

void _buildNode(Style::Computer &c, Markup::Node const &n, Frag &parent);

Frag build(Style::Computer &c, Markup::Document const &doc);

} // namespace Vaev::Layout
