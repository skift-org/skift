#include "positioned.h"

#include "layout.h"
#include "values.h"

namespace Vaev::Layout {

void layoutPositioned(Tree& tree, Frag& frag, RectPx containingBlock) {
    auto& style = frag.style();
    auto& metrics = frag.metrics;

    if (style.position == Position::ABSOLUTE or style.position == Position::RELATIVE) {
        auto origin = containingBlock.topStart();
        if (style.position == Position::RELATIVE)
            origin = metrics.position;

        auto top = metrics.position.y;
        auto start = metrics.position.x;

        auto topOffset = style.offsets->top;
        if (topOffset != Width::AUTO) {
            top = origin.y + resolve(tree, *frag.box, topOffset, containingBlock.height);
        }

        auto startOffset = style.offsets->start;
        if (startOffset != Width::AUTO) {
            start = origin.x + resolve(tree, *frag.box, startOffset, containingBlock.width);
        }

        auto endOffset = frag.style().offsets->end;
        if (endOffset != Width::AUTO) {
            start = (origin.x + containingBlock.width) - resolve(tree, *frag.box, endOffset, containingBlock.width) - metrics.borderSize.width;
        }

        Vec2Px newPositionOffset = Vec2Px{start, top} - metrics.position;
        frag.offset(newPositionOffset);

        containingBlock = metrics.contentBox();
    }

    for (auto& c : frag.children) {
        layoutPositioned(tree, c, containingBlock);
    }
}
} // namespace Vaev::Layout
