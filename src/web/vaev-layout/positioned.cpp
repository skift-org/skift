#include "positioned.h"

#include "layout.h"
#include "values.h"

namespace Vaev::Layout {

void layoutPositioned(Tree &tree, Box &box, RectPx containingBlock) {
    if (box.style->position == Position::ABSOLUTE or box.style->position == Position::RELATIVE) {
        auto origin = containingBlock.topStart();
        if (box.style->position == Position::RELATIVE)
            origin = box.layout.position;

        auto top = box.layout.position.y;
        auto start = box.layout.position.x;

        auto topOffset = box.style->offsets->top;
        if (topOffset != Width::AUTO) {
            top = origin.y + resolve(tree, box, topOffset, containingBlock.height);
        }

        auto startOffset = box.style->offsets->start;
        if (startOffset != Width::AUTO) {
            start = origin.x + resolve(tree, box, startOffset, containingBlock.width);
        }

        auto endOffset = box.style->offsets->end;
        if (endOffset != Width::AUTO) {
            start = (origin.x + containingBlock.width) - resolve(tree, box, endOffset, containingBlock.width) - box.layout.borderSize.width;
        }

        layout(
            tree,
            box,
            {
                .commit = Commit::YES,
                .knownSize = box.layout.borderBox().size().cast<Opt<Px>>(),
                .position = {start, top},
            }
        );

        containingBlock = box.layout.contentBox();
    }

    for (auto &c : box.children()) {
        layoutPositioned(tree, c, containingBlock);
    }
}
} // namespace Vaev::Layout
