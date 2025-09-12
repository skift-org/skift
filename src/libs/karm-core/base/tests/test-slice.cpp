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

    auto customCmp = [](Rune a, Rune b) {
        return toAsciiLower(a) == toAsciiLower(b);
    };

    expect$(contains("Ab"s, "ab"s, customCmp));
    expect$(contains("ab"s, "Ab"s, customCmp));
    expectNot$(contains("Ab"s, "ab"s));
    expectNot$(contains("ab"s, "Ab"s));

    return Ok();
}

test$("slice-split-simple") {

    Str text = "hello my friends"s;

    auto pieces = split(text, ' ');

    expectEq$(pieces.next(), "hello"s);
    expectEq$(pieces.next(), "my"s);
    expectEq$(pieces.next(), "friends"s);
    expectEq$(pieces.next(), NONE);

    return Ok();
}

test$("slice-split-consecutive-delim") {
    {
        Str text = "hello  my  friends"s;

        auto pieces = split(text, ' ');

        expectEq$(pieces.next(), "hello"s);
        expectEq$(pieces.next(), ""s);
        expectEq$(pieces.next(), "my"s);
        expectEq$(pieces.next(), ""s);
        expectEq$(pieces.next(), "friends"s);
        expectEq$(pieces.next(), NONE);

        return Ok();
    }
    {
        Str text = " my "s;
        auto pieces = split(text, ' ');

        expectEq$(pieces.next(), ""s);
        expectEq$(pieces.next(), "my"s);
        expectEq$(pieces.next(), ""s);
        expectEq$(pieces.next(), NONE);

        return Ok();
    }
}

test$("slice-split-no-delim") {

    Str text = "hellomyfriends"s;
    auto pieces = split(text, ' ');

    expectEq$(pieces.next(), "hellomyfriends"s);
    expectEq$(pieces.next(), NONE);

    return Ok();
}

test$("slice-split-empty") {
    {
        Str text = ""s;
        auto pieces = split(text, ' ');

        expectEq$(pieces.next(), ""s);
        expectEq$(pieces.next(), NONE);
    }

    {
        Str text = " "s;
        auto pieces = split(text, ' ');

        expectEq$(pieces.next(), ""s);
        expectEq$(pieces.next(), ""s);
        expectEq$(pieces.next(), NONE);
    }

    return Ok();
}

test$("slice-niche") {
    Opt<Slice<char>> test;

    auto comp = Slice<char>("test", 5);

    expectEq$(sizeof(test), sizeof(Slice<char>));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = Slice<char>("test", 5);
    expectEq$(test.unwrap(), comp);
    expectEq$(test.take(), comp);
    expectEq$(test, NONE);
    test = Slice<char>("", 1);
    expectEq$(test.has(), true);

    return Ok();
}

test$("mutslice-niche") {
    Opt<MutSlice<char>> test;

    auto comp = Slice<char>("test", 5);

    expectEq$(sizeof(test), sizeof(MutSlice<char>));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = MutSlice<char>(new char[5], 5);
    copy(comp, test.unwrap());
    expectEq$(test.unwrap(), comp);
    delete[] test.take().buf();
    expectEq$(test, NONE);

    test = MutSlice<char>(new char[5], 5);
    expectEq$(test.has(), true);
    delete[] test->buf();

    return Ok();
}

} // namespace Karm::Base
