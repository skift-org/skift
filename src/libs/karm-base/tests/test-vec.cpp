#include <karm-base/vec.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("vec-default-constructed") {
    Vec<int> vec;

    expectEq$(vec.len(), 0uz);
    expectEq$(vec.cap(), 0uz);
    expectEq$(vec.buf(), nullptr);

    return Ok();
}

test$("vec-push-front-slice") {
    Vec<int> vec = {4, 5};
    Array els{1, 2, 3};
    vec.pushFront(els);

    expectEq$(vec.len(), 5uz);
    expectEq$(vec[0], 1);
    expectEq$(vec[1], 2);
    expectEq$(vec[2], 3);
    expectEq$(vec[3], 4);
    expectEq$(vec[4], 5);

    return Ok();
}

test$("vec-niche") {
    Opt<Vec<int>> test;

    auto comp = Vec<int>{5, 0, 2};

    expectEq$(sizeof(test), sizeof(Vec<int>));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = Vec<int>{5, 0, 2};
    expectEq$(test.unwrap(), comp);
    expectEq$(test.take(), comp);
    expectEq$(test, NONE);
    test = Vec<int>{};
    expectEq$(test.has(), true);

    return Ok();
}

} // namespace Karm::Base::Tests
