#include <karm-base/buf.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("buf-niche") {
    Opt<Buf<int>> test;

    auto comp = Buf<int>::init(5, 0);

    expectEq$(sizeof(test), sizeof(Buf<int>));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = Buf<int>::init(5, 0);
    expectEq$(test.unwrap(), comp);
    expectEq$(test.take(), comp);
    expectEq$(test, NONE);
    test = Buf<int>::init(0);
    expectEq$(test.has(), true);

    return Ok();
}

} // namespace Karm::Base::Tests
