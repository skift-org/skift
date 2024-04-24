#pragma once

#include <karm-math/rect.h>
#include <karm-math/spacing.h>
#include <web-css/computed-values.h>
#include <web-dom/element.h>

namespace Web::Render {

// 9 MARK: Visual formatting model
// https://www.w3.org/TR/CSS22/visuren.html

struct Box {
    Strong<Dom::Element> _el;
    Strong<Css::ComputedValues> _style;

    Math::Rectf _bound{};
    Math::Spacingf _margin{};
    Math::Spacingf _padding{};
    Math::Spacingf _border{};

    Box(Strong<Dom::Element> el, Strong<Css::ComputedValues> style)
        : _el(el), _style(style) {
    }

    virtual ~Box() = default;
};

struct ParentBox {
    virtual ~ParentBox() = default;
};

// https://www.w3.org/TR/CSS22/visuren.html#block-level
struct BlockLevelBox :
    public Box {
};

// https://www.w3.org/TR/CSS22/visuren.html#block-boxes
struct BlockContainerBox :
    public ParentBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#block-boxes
struct BlockBox :
    public BlockLevelBox,
    public BlockContainerBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#inline-formatting
struct LineBox :
    public ParentBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#anonymous-block-level
struct InlineLevelBox :
    public Box {
};

// https://www.w3.org/TR/CSS22/visuren.html#anonymous-block-level
struct InlineBox :
    public InlineLevelBox,
    public ParentBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#anonymous
class AnonymousInlineBoxes :
    public InlineLevelBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#inline-boxes
class AtomicInlineLevelBox :
    public InlineLevelBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#inline-boxes
class InlineBlockBox :
    public AtomicInlineLevelBox,
    public BlockContainerBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#block-boxes
class ReplacedBox {
};

// https://www.w3.org/TR/CSS22/visuren.html#inline-boxes
class InlineReplacedBox :
    public ReplacedBox,
    public AtomicInlineLevelBox {
};

} // namespace Web::Render
