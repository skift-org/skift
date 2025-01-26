#include <karm-base/array.h>
#include <karm-test/macros.h>

namespace Karm::Base {

struct Foo {
    isize value;
    isize order;

    auto operator<=>(Foo const& other) const {
        return order <=> other.order;
    }
};

test$("sort") {
    Array arr{
        Foo{1, 1},
        Foo{2, 2},
        Foo{3, 3},
        Foo{4, 4},
        Foo{5, 5},
    };

    sort(arr);

    expectEq$(arr[0].value, 1);
    expectEq$(arr[1].value, 2);
    expectEq$(arr[2].value, 3);
    expectEq$(arr[3].value, 4);
    expectEq$(arr[4].value, 5);

    return Ok();
}

test$("stable-sort") {
    Array arr{
        Foo{1, 1},
        Foo{2, 2},
        Foo{3, 3},
        Foo{4, 2},
        Foo{5, 1},
    };

    stableSort(arr);

    expectEq$(arr[0].value, 1);
    expectEq$(arr[1].value, 5);
    expectEq$(arr[2].value, 2);
    expectEq$(arr[3].value, 4);
    expectEq$(arr[4].value, 3);

    return Ok();
}

test$("stable-sort-small") {
    Array arr{
        2,
        1
    };

    stableSort(arr);

    expectEq$(arr[0], 1);
    expectEq$(arr[1], 2);

    return Ok();
}

} // namespace Karm::Base
