#include "positioned.h"

#include "layout.h"

namespace Vaev::Layout {

void layoutPositioned(Tree &t, Frag &f, RectPx containingBlock) {
    if (f.style->position == Position::ABSOLUTE or f.style->position == Position::RELATIVE) {
        auto origin = containingBlock.topStart();
        if (f.style->position == Position::RELATIVE)
            origin = f.layout.position;

        auto top = f.layout.position.y;
        auto start = f.layout.position.x;

        auto topOffset = f.style->offsets->top;
        if (topOffset != Width::AUTO) {
            top = origin.y + resolve(t, f, topOffset, containingBlock.height);
        }

        auto startOffset = f.style->offsets->start;
        if (startOffset != Width::AUTO) {
            start = origin.x + resolve(t, f, startOffset, containingBlock.width);
        }

        auto endOffset = f.style->offsets->end;
        if (endOffset != Width::AUTO) {
            start = (origin.x + containingBlock.width) - resolve(t, f, endOffset, containingBlock.width) - f.layout.borderSize.width;
        }

        layout(
            t,
            f,
            {
                .commit = Commit::YES,
                .knownSize = f.layout.borderBox().size().cast<Opt<Px>>(),
                .position = {start, top},
            }
        );

        containingBlock = f.layout.contentBox();
    }

    for (auto &c : f.children()) {
        layoutPositioned(t, c, containingBlock);
    }
}
} // namespace Vaev::Layout
