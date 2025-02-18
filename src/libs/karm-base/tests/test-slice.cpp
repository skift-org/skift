#include <karm-test/macros.h>

namespace Karm::Base {

auto needle(isize value) {
    return [value](auto x) {
        return x <=> value;
    };
}

test$("search") {
    Array arr{1, 5, 10};

    expectEq$(search(arr, needle(0)), NONE);

    expectEq$(search(arr, needle(1)), 0uz);
    expectEq$(search(arr, needle(2)), NONE);
    expectEq$(search(arr, needle(3)), NONE);
    expectEq$(search(arr, needle(4)), NONE);
    expectEq$(search(arr, needle(5)), 1uz);
    expectEq$(search(arr, needle(6)), NONE);
    expectEq$(search(arr, needle(7)), NONE);
    expectEq$(search(arr, needle(8)), NONE);
    expectEq$(search(arr, needle(9)), NONE);
    expectEq$(search(arr, needle(10)), 2uz);

    expectEq$(search(arr, needle(11)), NONE);

    return Ok();
}

test$("search-lower-bound") {
    Array arr{1, 5, 10};

    expectEq$(searchLowerBound(arr, needle(0)), NONE);

    expectEq$(searchLowerBound(arr, needle(1)), 0uz);
    expectEq$(searchLowerBound(arr, needle(2)), 0uz);
    expectEq$(searchLowerBound(arr, needle(3)), 0uz);
    expectEq$(searchLowerBound(arr, needle(4)), 0uz);
    expectEq$(searchLowerBound(arr, needle(5)), 1uz);
    expectEq$(searchLowerBound(arr, needle(6)), 1uz);
    expectEq$(searchLowerBound(arr, needle(7)), 1uz);
    expectEq$(searchLowerBound(arr, needle(8)), 1uz);
    expectEq$(searchLowerBound(arr, needle(9)), 1uz);
    expectEq$(searchLowerBound(arr, needle(10)), 2uz);

    expectEq$(searchLowerBound(arr, needle(11)), 2uz);

    return Ok();
}

test$("search-upper-bound") {
    Array arr{1, 5, 10};

    expectEq$(searchUpperBound(arr, needle(0)), 0uz);

    expectEq$(searchUpperBound(arr, needle(1)), 0uz);
    expectEq$(searchUpperBound(arr, needle(2)), 1uz);
    expectEq$(searchUpperBound(arr, needle(3)), 1uz);
    expectEq$(searchUpperBound(arr, needle(4)), 1uz);
    expectEq$(searchUpperBound(arr, needle(5)), 1uz);
    expectEq$(searchUpperBound(arr, needle(6)), 2uz);
    expectEq$(searchUpperBound(arr, needle(7)), 2uz);
    expectEq$(searchUpperBound(arr, needle(8)), 2uz);
    expectEq$(searchUpperBound(arr, needle(9)), 2uz);
    expectEq$(searchUpperBound(arr, needle(10)), 2uz);

    expectEq$(searchUpperBound(arr, needle(11)), NONE);

    return Ok();
}

test$("slice-contains") {
    expect$(contains("Hello, world!"s, "world"s));
    expect$(contains("Hello, world!"s, "world!"s));
    expect$(contains("Hello, world!"s, "Hello"s));
    expect$(contains("Hello, world!"s, "Hello, world!"s));
    expectNot$(contains("Hello, world!"s, "Hello, world! "s));
    expectNot$(contains("Hello, world!"s, "bruh"s));

    return Ok();
}

} // namespace Karm::Base
