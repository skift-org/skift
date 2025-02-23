#include <karm-test/macros.h>

#include "karm-math/insets.h"

namespace Karm::Math::Tests {

test$("insets-map-order") {
    u8 start = 1;
    u8 top = 2;
    u8 bottom = 3;
    u8 end = 4;

    Insets<u8> oldInset{
        top,
        end,
        bottom,
        start
    };

    auto newInset = oldInset.map([](auto x) {
        return (u8)x;
    });

    expectEq$(oldInset.start, newInset.start);
    expectEq$(oldInset.top, newInset.top);
    expectEq$(oldInset.bottom, newInset.bottom);
    expectEq$(oldInset.end, newInset.end);

    return Ok();
}

test$("insets-cast-order") {
    u8 start = 1;
    u8 top = 2;
    u8 bottom = 3;
    u8 end = 4;

    Insets<u8> oldInset{
        top,
        end,
        bottom,
        start
    };

    auto newInset = oldInset.cast<u8>();

    expectEq$(oldInset.start, newInset.start);
    expectEq$(oldInset.top, newInset.top);
    expectEq$(oldInset.bottom, newInset.bottom);
    expectEq$(oldInset.end, newInset.end);

    return Ok();
}

} // namespace Karm::Math::Tests
