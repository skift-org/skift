import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("set-put") {
    Set<int> set{};
    set.put(420);
    expectEq$(set.len(), 1uz);
    expect$(set.has(420));

    return Ok();
}

test$("set-remove") {
    Set<int> set{};
    set.put(420);
    expect$(set.has(420));
    set.del(420);
    expect$(not set.has(420));

    return Ok();
}

test$("set-clear") {
    Set<int> set{};
    set.put(420);
    set.put(69);
    expect$(set.has(420));
    expect$(set.has(69));
    set.clear();
    expect$(not set.has(420));
    expect$(not set.has(69));

    return Ok();
}

test$("set-len") {
    Set<int> set{};
    expectEq$(set.len(), 0uz);
    set.put(420);
    expectEq$(set.len(), 1uz);
    set.put(69);
    expectEq$(set.len(), 2uz);
    set.del(420);
    expectEq$(set.len(), 1uz);

    return Ok();
}

test$("set-usage") {
    Set<int> set{10};
    expectEq$(set.usage(), 0uz);
    set.put(420);
    expectEq$(set.usage(), 10uz);
    set.put(69);
    expectEq$(set.usage(), 20uz);
    set.del(420);
    expectEq$(set.usage(), 10uz);

    return Ok();
}

test$("set-ensure") {
    Set<int> set{10};
    expectEq$(set._cap, 10uz);
    set.rehash(20);
    expectEq$(set._cap, 20uz);
    set.rehash(10);
    expectEq$(set._cap, 20uz);

    return Ok();
}

test$("set-put-collision") {
    Set<int> set{10};
    set.put(420);
    set.put(69);
    set.put(420);
    expectEq$(set.len(), 2uz);

    return Ok();
}

test$("set-put-resize") {
    Set<int> set{10};
    for (int i = 0; i < 10; i++) {
        set.put(i);
    }
    expectEq$(set.len(), 10uz);
    set.put(420);
    expect$(set.has(420));

    return Ok();
}

} // namespace Karm::Base::Tests
