#include <karm-base/reflect.h>
#include <karm-test/macros.h>

struct Foo {
    int bar;
    float baz;
};

Reflectable$(Foo, bar, baz);

namespace Karm::Base::Tests {

test$(reflect) {
    using R = Reflect<Foo>;

    expectEq$(R::NAME, "Foo");
    expectEq$(R::Fields::T0::NAME, "bar");
    expectEq$(R::Fields::T0::Reflect::NAME, "int");
    expectEq$(R::Fields::T1::NAME, "baz");
    expectEq$(R::Fields::T1::Reflect::NAME, "float");

    return Ok();
}

} // namespace Karm::Base::Tests
