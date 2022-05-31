#include <karm-test/macros.h>

#include "../vec.h"

test$("Vec") {
    describe$("constructor") {
        it$("should be empty when created") {
            Vec<int> v;

            expectEq$(v.len(), 0uz);
            expectEq$(v.cap(), 0uz);
        }

        it$("should be copyable") {
            Vec<int> v = {1, 2, 3};
            Vec<int> v2 = v;

            expectEq$(v2.len(), 3uz);
            expectEq$(v2.cap(), 3uz);
            expectEq$(v2.at(0), 1);
            expectEq$(v2.at(1), 2);
            expectEq$(v2.at(2), 3);
        }

        it$("should be moveable") {
            Vec<int> v = {1, 2, 3};
            Vec<int> v2 = std::move(v);

            expectEq$(v2.len(), 3uz);
            expectEq$(v2.cap(), 3uz);
            expectEq$(v2.at(0), 1);
            expectEq$(v2.at(1), 2);
            expectEq$(v2.at(2), 3);
        }

        it$("should be empty when created with capacity") {
            Vec<int> v(10);

            expectEq$(v.len(), 0uz);
            expectEq$(v.cap(), 10uz);
        }

        it$("should be empty when created with an empty initializer list") {
            Vec<int> v = {};

            expectEq$(v.len(), 0uz);
            expectEq$(v.cap(), 0uz);
        }

        it$("should contain the elements of the initializer list") {
            Vec<int> v = {1, 2, 3};

            expectEq$(v.len(), 3uz);
            expectEq$(v.cap(), 3uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
        }
    }

    describe$("pushBack") {
        it$("should increase the length") {
            Vec<int> v;

            v.pushBack(1);
            v.pushBack(2);
            v.pushBack(3);

            expectEq$(v.len(), 3uz);
            expectEq$(v.cap(), 3uz);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
        }

        it$("should increase the capacity") {
            Vec<int> v;

            size_t oldCap = v.cap();

            v.pushBack(1);
            v.pushBack(2);
            v.pushBack(3);
            v.pushBack(4);
            v.pushBack(5);

            expectEq$(v.len(), 5uz);
            expectGteq$(v.cap(), v.len());
            expectGt$(v.cap(), oldCap);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(v.at(3), 4);
            expectEq$(v.at(4), 5);
        }

        it$("should not increase the capacity when the capacity is sufficient") {
            Vec<int> v(10);

            size_t oldCap = v.cap();

            v.pushBack(1);
            v.pushBack(2);
            v.pushBack(3);
            v.pushBack(4);
            v.pushBack(5);

            expectEq$(v.len(), 5uz);
            expectEq$(v.cap(), oldCap);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(v.at(3), 4);
            expectEq$(v.at(4), 5);
        }

        it$("should not increase the capacity when the capacity is sufficient and the length is zero") {
            Vec<int> v(10);

            size_t oldCap = v.cap();

            v.pushBack(1);
            v.pushBack(2);
            v.pushBack(3);
            v.pushBack(4);
            v.pushBack(5);
            v.pushBack(6);
            v.pushBack(7);
            v.pushBack(8);
            v.pushBack(9);
            v.pushBack(10);

            expectEq$(v.len(), 10uz);
            expectEq$(v.cap(), oldCap);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(v.at(3), 4);
            expectEq$(v.at(4), 5);
            expectEq$(v.at(5), 6);
            expectEq$(v.at(6), 7);
            expectEq$(v.at(7), 8);
            expectEq$(v.at(8), 9);
            expectEq$(v.at(9), 10);
        }

        it$("should not increase the capacity when the capacity is sufficient and the length is one") {
            Vec<int> v(10);

            size_t oldCap = v.cap();

            v.pushBack(1);
            v.pushBack(2);
            v.pushBack(3);
            v.pushBack(4);
            v.pushBack(5);
            v.pushBack(6);
            v.pushBack(7);
            v.pushBack(8);
            v.pushBack(9);
            v.pushBack(10);
            v.pushBack(11);

            expectEq$(v.len(), 11uz);
            expectEq$(v.cap(), oldCap);
            expectEq$(v.at(0), 1);
            expectEq$(v.at(1), 2);
            expectEq$(v.at(2), 3);
            expectEq$(v.at(3), 4);
            expectEq$(v.at(4), 5);
            expectEq$(v.at(5), 6);
            expectEq$(v.at(6), 7);
            expectEq$(v.at(7), 8);
            expectEq$(v.at(8), 9);
            expectEq$(v.at(9), 10);
            expectEq$(v.at(10), 11);
        }
    }

    return OK;
}
