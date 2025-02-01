#include "inline.h"

namespace Vaev::Layout {

struct InlineFormatingContext : public FormatingContext {
    virtual Output run([[maybe_unused]] Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        // NOTE: We are not supposed to get there if the content is not a prose
        auto& prose = *box.content.unwrap<Rc<Text::Prose>>("inlineLayout");

        auto inlineSize = input.knownSize.x.unwrapOrElse([&] {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                return 0_px;
            } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                return Limits<Px>::MAX;
            } else {
                return input.availableSpace.x;
            }
        });

        auto size = prose.layout(inlineSize.cast<f64>()).cast<Px>();

        if (tree.fc.allowBreak() and not tree.fc.acceptsFit(
                                         input.position.y,
                                         size.y,
                                         input.pendingVerticalSizes
                                     )) {
            return {
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow()
            };
        }

        return {
            .size = size,
            .completelyLaidOut = true,
        };
    }
};

Rc<FormatingContext> constructInlineFormatingContext(Box&) {
    return makeRc<InlineFormatingContext>();
}

} // namespace Vaev::Layout
