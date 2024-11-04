#include "inline.h"

#include "box.h"

namespace Vaev::Layout {

Output inlineLayout(Tree &, Box &box, Input input) {
    // NOTE: We are not supposed to get there if the content is not a prose
    auto &prose = *box.content.unwrap<Strong<Text::Prose>>("inlineLayout");

    auto inlineSize = input.knownSize.x.unwrapOrElse([&] {
        if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
            return Px{0};
        } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
            return Limits<Px>::MAX;
        } else {
            return input.availableSpace.x;
        }
    });

    auto res = prose.layout(inlineSize.cast<f64>());

    return Output::fromSize(res.cast<Px>());
}

} // namespace Vaev::Layout
