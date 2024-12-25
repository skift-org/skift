#include <karm-async/one.h>
#include <karm-async/queue.h>
#include <karm-test/macros.h>

namespace Karm::Async::Tests {

test$("karm-queue-enqueue-dequeue") {
    Queue<isize> q;
    q.enqueue(42);
    q.enqueue(69);

    auto res1 = Async::run(q.dequeueAsync());
    auto res2 = Async::run(q.dequeueAsync());

    expectEq$(res1, 42);
    expectEq$(res2, 69);

    return Ok();
}

test$("karm-queue-dequeue-enqueue") {
    Queue<isize> q;

    isize res1 = 0;
    isize res2 = 0;
    bool orderOk = false;

    Async::detach(q.dequeueAsync(), [&](isize v) {
        res1 = v;
    });

    Async::detach(q.dequeueAsync(), [&](isize v) {
        if (res1 == 42)
            orderOk = true;
        res2 = v;
    });

    q.enqueue(42);
    q.enqueue(69);
    q.enqueue(96);

    expect$(orderOk);
    expectEq$(res1, 42);
    expectEq$(res2, 69);

    expect$(not q.empty());
    expectEq$(q.dequeue(), 96);

    return Ok();
}

} // namespace Karm::Async::Tests
