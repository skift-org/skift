#pragma once

#include <karm-image/picture.h>
#include <karm-text/prose.h>
#include <vaev-base/length.h>
#include <vaev-base/resolution.h>
#include <vaev-base/writing.h>
#include <vaev-style/computer.h>

namespace Vaev::Layout {

// MARK: Fragmentainer ---------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#fragmentainer
// https://www.w3.org/TR/css-break-3/#fragmentation-context
struct Fragmentainer {
    Vec2Px _size;
    bool _isDiscoveryMode = false; //< Are we looking for suitable breakpoints?
    usize _monolithicCount = 0;    //< How deep we are in a monolithic box

    Fragmentainer(Vec2Px currSize = Vec2Px::MAX) : _size(currSize) {}

    Vec2Px size() const { return _size; }

    void enterDiscovery() { _isDiscoveryMode = true; }

    void leaveDiscovery() { _isDiscoveryMode = false; }

    bool isDiscoveryMode() {
        return allowBreak() and _isDiscoveryMode;
    }

    bool isMonolithicBox() {
        return _monolithicCount == 0;
    }

    void enterMonolithicBox() {
        _monolithicCount++;
    }

    void leaveMonolithicBox() {
        _monolithicCount--;
    }

    bool hasInfiniteDimensions() {
        return _size == Vec2Px::MAX;
    }

    bool allowBreak() {
        return not hasInfiniteDimensions() and _monolithicCount == 0;
    }

    bool acceptsFit(Px verticalPosition, Px verticalSize, Px pendingVerticalSizes) {
        // TODO: consider apply this check only when in discovery mode
        return verticalPosition + verticalSize + pendingVerticalSizes <= _size.y;
    }

    Px leftVerticalSpace(Px verticalPosition, Px pendingVerticalSizes) {
        return _size.y - verticalPosition - pendingVerticalSizes;
    }
};

// MARK: Breakpoint ------------------------------------------------------------

// TODO: consider adding classification for breakpoints, what would make appeal computing easier and less error prone
struct Breakpoint {

    usize endIdx = 0;

    enum struct Appeal : usize {
        EMPTY = 0,
        OVERFLOW = 1,
        AVOID = 2,
        CLASS_B = 3,
        FORCED = 4,
        MAX = Limits<usize>::MAX
    };
    Appeal appeal = Appeal::EMPTY;

    Vec<Opt<Breakpoint>> children = {};

    enum struct ADVANCE_CASE {
        NOT_ADVANCE, // keeping children
        ADVANCE_WITH_CHILDREN,
        ADVANCE_WITHOUT_CHILDREN
    } advanceCase;

    void overrideIfBetter(Breakpoint&& BPWithMoreContent) {
        // in case of overflows, we need the earliest breakpoint possible
        if (appeal == Appeal::OVERFLOW and BPWithMoreContent.appeal == Appeal::OVERFLOW)
            return;

        if (appeal <= BPWithMoreContent.appeal)
            *this = std::move(BPWithMoreContent);
    }

    void repr(Io::Emit& e) const {
        e("(end: {} appeal: {} advance case: {}", endIdx, appeal, advanceCase);
        if (children.len() == 0)
            e("; no child)");
        else
            e("; children : {})", children);
    }

    static Breakpoint buildForced(usize endIdx) {
        return Breakpoint{
            .endIdx = endIdx,
            // since this is a FORCED break, it will have maximum appeal
            .appeal = Appeal::FORCED,
            .advanceCase = ADVANCE_CASE::ADVANCE_WITHOUT_CHILDREN,
        };
    }

    // NOTE: a bit inconsistent with the rest of the API
    void applyAvoid() {
        appeal = Appeal::AVOID;
    }

    static Breakpoint buildFromChild(Breakpoint&& childBreakpoint, usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = childBreakpoint.appeal,
            .children = {std::move(childBreakpoint)},
            .advanceCase = ADVANCE_CASE::NOT_ADVANCE,
        };

        if (isAvoid)
            b.appeal = Appeal::AVOID;

        return b;
    }

    static Breakpoint buildFromChildren(Vec<Opt<Breakpoint>> childrenBreakpoints, usize endIdx, bool isAvoid, bool advance) {
        Appeal appeal = Appeal::MAX;
        for (auto& breakpoint : childrenBreakpoints) {
            if (not breakpoint)
                continue;

            appeal = min(appeal, breakpoint.unwrap().appeal);
        }

        if (appeal == Appeal::MAX)
            panic("cannot build breakpoint from children when no children have a breakpoint");

        Breakpoint b{
            .endIdx = endIdx,
            .appeal = appeal,
            .children = {std::move(childrenBreakpoints)},
            .advanceCase = advance ? ADVANCE_CASE::ADVANCE_WITH_CHILDREN : ADVANCE_CASE::NOT_ADVANCE
        };

        if (isAvoid)
            b.appeal = Appeal::AVOID;

        return b;
    }

    static Breakpoint buildClassB(usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = isAvoid ? Appeal::AVOID : Appeal::CLASS_B,
            .advanceCase = ADVANCE_CASE::ADVANCE_WITHOUT_CHILDREN
        };

        return b;
    }

    static Breakpoint buildOverflow() {
        // this is a placeholder breakpoint and should be overriden
        return {
            .endIdx = 0,
            .appeal = Appeal::OVERFLOW,
            .advanceCase = ADVANCE_CASE::NOT_ADVANCE
        };
    }
};

struct BreakpointTraverser {
    MutCursor<Breakpoint> prevIteration, currIteration;

    BreakpointTraverser(
        MutCursor<Breakpoint> prev = nullptr,
        MutCursor<Breakpoint> curr = nullptr
    ) : prevIteration(prev), currIteration(curr) {}

    bool isDeactivated() {
        return prevIteration == nullptr and currIteration == nullptr;
    }

    MutCursor<Breakpoint> traversePrev(usize i, usize j) {
        if (prevIteration and prevIteration->children.len() > 0 and
            (i + 1 == prevIteration->endIdx or
             (prevIteration->advanceCase == Breakpoint::ADVANCE_CASE::ADVANCE_WITH_CHILDREN and i == prevIteration->endIdx)
            )) {
            if (prevIteration->children[j])
                return &prevIteration->children[j].unwrap();
        }
        return nullptr;
    }

    MutCursor<Breakpoint> traverseCurr(usize i, usize j) {
        if (currIteration and currIteration->children.len() > 0 and i + 1 == currIteration->endIdx) {
            if (currIteration->children[j])
                return &currIteration->children[j].unwrap();
        }
        return nullptr;
    }

    BreakpointTraverser traverseInsideUsingIthChildToJthParallelFlow(usize i, usize j) {
        BreakpointTraverser deeperBPT;
        deeperBPT.prevIteration = traversePrev(i, j);
        deeperBPT.currIteration = traverseCurr(i, j);
        return deeperBPT;
    }

    BreakpointTraverser traverseInsideUsingIthChild(usize i) {
        return traverseInsideUsingIthChildToJthParallelFlow(i, 0);
    }

    Opt<usize> getStart() {
        if (prevIteration == nullptr)
            return NONE;
        return prevIteration->endIdx - (prevIteration->advanceCase == Breakpoint::ADVANCE_CASE::NOT_ADVANCE);
    }

    Opt<usize> getEnd() {
        if (currIteration == nullptr)
            return NONE;
        return currIteration->endIdx;
    }
};

// MARK: Box -------------------------------------------------------------------

struct FormatingContext;
struct Box;

using Content = Union<
    None,
    Vec<Box>,
    Rc<Text::Prose>,
    Karm::Image::Picture>;

struct Attrs {
    usize span = 1;
    usize rowSpan = 1;
    usize colSpan = 1;

    void repr(Io::Emit& e) const {
        e("(attrs span: {} rowSpan: {} colSpan: {})", span, rowSpan, colSpan);
    }
};

struct Box : public Meta::NoCopy {
    Rc<Style::Computed> style;
    Rc<Text::Fontface> fontFace;
    Content content = NONE;
    Attrs attrs;
    Opt<Rc<FormatingContext>> formatingContext = NONE;

    Box(Rc<Style::Computed> style, Rc<Karm::Text::Fontface> fontFace);

    Box(Rc<Style::Computed> style, Rc<Karm::Text::Fontface> fontFace, Content content);

    Slice<Box> children() const;

    MutSlice<Box> children();

    void add(Box&& box);

    void repr(Io::Emit& e) const;
};

struct Viewport {
    Resolution dpi = Resolution::fromDpi(96);
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectPx small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectPx large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectPx dynamic = small;
};

struct Tree {
    Box root;
    Viewport viewport = {};
    Fragmentainer fc = {};
};

// MARK: Fragment --------------------------------------------------------------

struct Metrics {
    InsetsPx padding{};
    InsetsPx borders{};
    Vec2Px position; //< Position relative to the content box of the containing block
    Vec2Px borderSize;
    InsetsPx margin{};
    RadiiPx radii{};

    void repr(Io::Emit& e) const {
        e("(layout paddings: {} borders: {} position: {} borderSize: {} margin: {} radii: {})",
          padding, borders, position, borderSize, margin, radii);
    }

    RectPx borderBox() const {
        return RectPx{position, borderSize};
    }

    RectPx paddingBox() const {
        return borderBox().shrink(borders);
    }

    RectPx contentBox() const {
        return paddingBox().shrink(padding);
    }

    RectPx marginBox() const {
        return borderBox().grow(margin);
    }
};

struct Frag {
    MutCursor<Box> box;
    Metrics metrics;
    Vec<Frag> children;

    Frag(MutCursor<Box> box) : box{std::move(box)} {}

    Frag() : box{nullptr} {}

    Style::Computed const& style() const {
        return *box->style;
    }

    /// Offset the position of this fragment and its subtree.
    void offset(Vec2Px d) {
        metrics.position = metrics.position + d;
        for (auto& c : children)
            c.offset(d);
    }

    /// Add a child fragment.
    void add(Frag&& frag) {
        children.pushBack(std::move(frag));
    }
};

// MARK: Input & Output --------------------------------------------------------

enum struct IntrinsicSize {
    AUTO,
    MIN_CONTENT,
    MAX_CONTENT,
    STRETCH_TO_FIT,
};

static inline bool isMinMaxIntrinsicSize(IntrinsicSize intrinsic) {
    return intrinsic == IntrinsicSize::MIN_CONTENT or
           intrinsic == IntrinsicSize::MAX_CONTENT;
}

struct Input {
    /// Parent fragment where the layout will be attached.
    MutCursor<Frag> fragment = nullptr;
    IntrinsicSize intrinsic = IntrinsicSize::AUTO;
    Math::Vec2<Opt<Px>> knownSize = {};
    Vec2Px position = {};
    Vec2Px availableSpace = {};
    Vec2Px containingBlock = {};

    BreakpointTraverser breakpointTraverser = {};

    // To be used between table wrapper and table box
    Opt<Px> capmin = NONE;

    // TODO: instead of stringing this around, maybe change this (and check method of fragmentainer) to a
    // "availableSpaceInFragmentainer" parameter
    Px pendingVerticalSizes = {};

    Input withFragment(MutCursor<Frag> f) const {
        auto copy = *this;
        copy.fragment = f;
        return copy;
    }

    Input withIntrinsic(IntrinsicSize i) const {
        auto copy = *this;
        copy.intrinsic = i;
        return copy;
    }

    Input withKnownSize(Math::Vec2<Opt<Px>> size) const {
        auto copy = *this;
        copy.knownSize = size;
        return copy;
    }

    Input withPosition(Vec2Px pos) const {
        auto copy = *this;
        copy.position = pos;
        return copy;
    }

    Input withAvailableSpace(Vec2Px space) const {
        auto copy = *this;
        copy.availableSpace = space;
        return copy;
    }

    Input withContainingBlock(Vec2Px block) const {
        auto copy = *this;
        copy.containingBlock = block;
        return copy;
    }

    Input withBreakpointTraverser(BreakpointTraverser bpt) const {
        auto copy = *this;
        copy.breakpointTraverser = bpt;
        return copy;
    }

    Input addPendingVerticalSize(Px newPendingVerticalSize) const {
        auto copy = *this;
        copy.pendingVerticalSizes += newPendingVerticalSize;
        return copy;
    }
};

struct Output {
    // size of subtree maximizing displayed content while respecting
    // - endchild constraint or
    // - not overflowing fragmentainer or
    // - forced break
    Vec2Px size;

    // was the box subtree laid out until the end?
    // - discovery mode: until the very end of the box
    // - non discovery mode: all subtrees until endChildren-1 were completly laid out
    // useful for:
    // - discovery mode: knowing if a child was complete so we can break after it
    //      (if not fully laid out, we need to stop the block formatting context)
    // - non-discovery mode: knowing if we can finish rendering
    bool completelyLaidOut;

    // only to be used in discovery mode
    Opt<Breakpoint> breakpoint = NONE;

    static Output fromSize(Vec2Px size) {
        return {
            .size = size,
            .completelyLaidOut = true
        };
    }

    Px width() const {
        return size.x;
    }

    Px height() const {
        return size.y;
    }
};

// MARK: Formating Context -----------------------------------------------------

struct FormatingContext {
    virtual ~FormatingContext() = default;

    virtual Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) = 0;
};

} // namespace Vaev::Layout
