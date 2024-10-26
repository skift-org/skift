#include <karm-base/sieve.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("list-push-and-pop") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    expectEq$(list.popBack(), 3);
    expectEq$(list.popBack(), 2);
    expectEq$(list.popBack(), 1);

    return Ok();
}

test$("list-requeue") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    list.requeue();

    expectEq$(list[0], 2);
    expectEq$(list[1], 3);
    expectEq$(list[2], 1);

    return Ok();
}

test$("list-trunc") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    list.trunc(1);

    expectEq$(list.len(), 1uz);
    expectEq$(list[0], 1);

    return Ok();
}

test$("list-clear") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    list.clear();

    expectEq$(list.len(), 0uz);

    return Ok();
}

test$("list-insert") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    list.insert(1, 4);

    expectEq$(list.len(), 4uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 4);
    expectEq$(list[2], 2);
    expectEq$(list[3], 3);

    return Ok();
}

test$("list-remove") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    list.remove(1);

    expectEq$(list.len(), 2uz);

    expectEq$(list[0], 2);
    expectEq$(list[1], 3);

    return Ok();
}

test$("list-remove-at") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    list.removeAt(1);

    expectEq$(list.len(), 2uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 3);

    return Ok();
}

test$("list-iter") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    int i = 0;
    for (auto &el : list.iter()) {
        expectEq$(el, i + 1);
        i++;
    }

    return Ok();
}

test$("list-iter-rev") {
    List<int> list;

    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);

    expectEq$(list.len(), 3uz);

    expectEq$(list[0], 1);
    expectEq$(list[1], 2);
    expectEq$(list[2], 3);

    int i = 3;
    for (auto &el : list.iterRev()) {
        expectEq$(el, i);
        i--;
    }

    return Ok();
}

} // namespace Karm::Base::Tests
