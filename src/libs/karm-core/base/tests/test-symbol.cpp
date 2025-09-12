import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Tests {

test$("symbol-equality") {
    Symbol sym0 = "test"_sym;
    Symbol sym1 = "test"_sym;
    Symbol sym2 = "test but different"_sym;
    Symbol sym3 = "test but different"_sym;

    expectEq$(sym0, sym1);
    expectEq$(sym2, sym3);

    return Ok();
}

} // namespace Karm::Tests
