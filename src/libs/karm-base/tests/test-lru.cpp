#include <karm-base/lru.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$(lruAccess) {
    Lru<int, int> cache{10};

    for (int i = 0; i < 10; i++) {
        (void)cache.access(i, [&] {
            return i * 10;
        });
    }

    for (int i = 0; i < 10; i++) {
        auto val = cache.get(i);
        expectEq$(val, i * 10);
    }

    return Ok();
}

test$(lruContains) {
    Lru<int, int> cache{10};
    expect$(not cache.contains(0));
    (void)cache.access(0, [&] {
        return 0;
    });
    expect$(cache.contains(0));
    return Ok();
}

test$(lruLen) {
    Lru<int, int> cache{10};
    expectEq$(cache.len(), 0uz);
    (void)cache.access(0, [&] {
        return 0;
    });
    expectEq$(cache.len(), 1uz);

    for (int i = 1; i < 10; i++) {
        (void)cache.access(i, [&] {
            return i * 10;
        });
    }

    expectEq$(cache.len(), 10uz);
    return Ok();
}

test$(lruEvict) {
    Lru<int, int> cache{10};

    for (int i = 0; i < 10; i++) {
        (void)cache.access(i, [&] {
            return i * 10;
        });
    }

    for (int i = 0; i < 10; i++) {
        auto val = cache.get(i);
        expectEq$(val, i * 10);
    }

    (void)cache.access(10, [&] {
        return 10 * 10;
    });

    expect$(not cache.contains(0));
    expectEq$(cache.len(), 10uz);

    return Ok();
}

} // namespace Karm::Base::Tests
