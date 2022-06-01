#include <karm-test/macros.h>

#include "../vec.h"

template <typename V>
Error test_vec(Driver &_driver) {
    describe$("constructor") {
        it$("should be empty when created") {
            V v;

            expectEq$(v.len(), 0uz);
            expectEq$(v.cap(), 0uz);
        }

        it$("should be copyable") {
            V v = {1, 2, 3};
            V v2 = v;

            expectEq$(v2.len(), 3uz);
            expectEq$(v2.cap(), 3uz);
            expectEq$(v2.at(0), 1);
            expectEq$(v2.at(1), 2);
            expectEq$(v2.at(2), 3);
        }

        it$("should be moveable") {
            V v = {1, 2, 3};
            V v2 = std::move(v);

            expectEq$(v2.len(), 3uz);
            expectEq$(v2.cap(), 3uz);
            expectEq$(v2.at(0), 1);
            expectEq$(v2.at(1), 2);
            expectEq$(v2.at(2), 3);
        }

        it$("should be empty when created with capacity") {
            V v(10);

            expectEq$(v.len(), 0uz);
            expectEq$(v.cap(), 10uz);
        }

        it$("should be empty when created with an empty initializer list") {
            V v = {};

            expectEq$(v.len(), 0uz);
            expectEq$(v.cap(), 0uz);
        }

        it$("should contain the elements of the initializer list") {
            V v = {1, 2, 3};

            expectEq$(v.len(), 3uz);
            expectEq$(v.cap(), 3uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
        }
    }

    describe$("insert") {
        it$("should insert at the end") {
            V v = {1, 2, 3};
            v.insert(3, 4);

            expectEq$(v.len(), 4uz);
            expectEq$(v.cap(), 4uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(v.at(3), 4);
        }

        it$("should insert at the beginning") {
            V v = {1, 2, 3};
            v.insert(0, 0);

            expectEq$(v.len(), 4uz);
            expectEq$(v.cap(), 4uz);
            expectEq$(v.at(0), 0);
            expectEq$(v.at(1), 1);
            expectEq$(v.at(2), 2);
            expectEq$(v.at(3), 3);
        }

        it$("should insert at the middle") {
            V v = {1, 2, 3};
            v.insert(1, 0);

            expectEq$(v.len(), 4uz);
            expectEq$(v.cap(), 4uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 0);
            expectEq$(v.at(2), 2);
            expectEq$(v.at(3), 3);
        }

        it$("should insert at the end when the vector is full") {
            V v = {1, 2, 3};
            v.insert(3, 4);
            v.insert(4, 5);

            expectEq$(v.len(), 5uz);
            expectEq$(v.cap(), 5uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(v.at(3), 4);
            expectEq$(v.at(4), 5);
        }
    }

    describe$("pushBack") {
        it$("should increase the length and capacity") {
            V v;

            size_t oldCap = v.cap();

            v.pushBack(1);
            v.pushBack(2);
            v.pushBack(3);

            expectEq$(v.len(), 3uz);
            expectGteq$(v.cap(), v.len());
            expectGt$(v.cap(), oldCap);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
        }

        it$("should not increase the capacity when the capacity is sufficient") {
            V v(10);

            size_t oldCap = v.cap();

            v.pushBack(1);
            v.pushBack(2);
            v.pushBack(3);

            expectEq$(v.len(), 3uz);
            expectEq$(v.cap(), oldCap);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
        }

        it$("let elements be popped") {
            V v = {1, 2, 3};

            v.pushBack(4);
            auto popped = v.popBack();

            expectEq$(v.len(), 3uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(popped, 4);
        }

        it$("let element be peeked") {
            V v = {1, 2};

            v.pushBack(3);
            auto peeked = v.peekBack();

            expectEq$(v.len(), 3uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(peeked, 3);
        }
    }

    describe$("pushFront") {
        it$("should increase the length and capacity") {
            V v;

            size_t oldCap = v.cap();

            v.pushFront(1);
            v.pushFront(2);
            v.pushFront(3);

            expectEq$(v.len(), 3uz);
            expectGteq$(v.cap(), v.len());
            expectGt$(v.cap(), oldCap);
            expectEq$(v.at(0), 3);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 1);
        }

        it$("should not increase the capacity when the capacity is sufficient") {
            V v(10);

            size_t oldCap = v.cap();

            v.pushFront(1);
            v.pushFront(2);
            v.pushFront(3);

            expectEq$(v.len(), 3uz);
            expectEq$(v.cap(), oldCap);
            expectEq$(v.at(0), 3);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 1);
        }

        it$("let elements be popped") {
            V v = {1, 2, 3};

            v.pushFront(4);
            auto popped = v.popFront();

            expectEq$(v.len(), 3uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(popped, 4);
        }

        it$("let element be peeked") {
            V v = {1, 2};

            v.pushFront(3);
            auto peeked = v.peekFront();

            expectEq$(v.len(), 3uz);
            expectEq$(v.at(0), 3);
            expectEq$(v.at(1), 1);
            expectEq$(v.at(2), 2);
            expectEq$(peeked, 3);
        }
    }

    return OK;
}

test$("Vec") {
    try$(test_vec<Vec<int>>(_driver));
    return OK;
}

test$("InlineVec") {
    try$((test_vec<InlineVec<int, 10>>(_driver)));
    return OK;
}
