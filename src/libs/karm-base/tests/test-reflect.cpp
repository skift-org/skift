#include <karm-base/reflect.h>
#include <karm-test/macros.h>

struct Foo {
    int bar;
    float baz;
};

Reflectable$(Foo, bar, baz);

namespace Karm::Base::Tests {

test$("reflect-struct") {
    using R = Reflect<Foo>;

    expectEq$(R::NAME, "Foo");
    expectEq$(R::Fields::T0::NAME, "bar");
    expectEq$(R::Fields::T0::Reflect::NAME, "int");
    expectEq$(R::Fields::T1::NAME, "baz");
    expectEq$(R::Fields::T1::Reflect::NAME, "float");

    return Ok();
}

enum class Bar {
    A,
    B,
    C,

    _LEN,
};

test$("reflect-enum-name-of") {
    auto name = nameOf<Bar, Bar::A>();
    expectEq$(name, "A");

    return Ok();
}

test$("reflect-enum-min-max-len") {
    using R = Reflect<Bar>;
    expectEq$(R::MIN, 0);
    expectEq$(R::MAX, 2);
    expectEq$(R::LEN, 3);

    return Ok();
}

test$("reflect-enum-value-name-of") {
    expectEq$(nameOf(Bar::A), "A");
    expectEq$(nameOf(Bar::B), "B");
    expectEq$(nameOf(Bar::C), "C");

    return Ok();
}

test$("reflect-enum-value-value-of") {
    expectEq$(valueOf<Bar>("A"), Bar::A);
    expectEq$(valueOf<Bar>("B"), Bar::B);
    expectEq$(valueOf<Bar>("C"), Bar::C);

    return Ok();
}

} // namespace Karm::Base::Tests
