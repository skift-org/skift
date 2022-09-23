#include <karm-test/macros.h>

#include "../range.h"

test$("Range") {
    describe$("substract") {
        it$("should split a range in two") {
            USizeRange r1 = {0, 10};
            USizeRange r2 = {2, 8};
            auto [l, u] = r1.substract(r2);

            auto expectL = USizeRange{0, 2};
            auto expectU = USizeRange{10, 2};

            expectEq$(l, expectL);
            expectEq$(u, expectU);
        }
    }

    return OK;
}
