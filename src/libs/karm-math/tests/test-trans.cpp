#include <karm-math/trans.h>
#include <karm-test/macros.h>

namespace Karm::Math::Tests {

test$("transform-inverse") {
    Vec2<f64> v{1.0, 2.0};

    auto makeIdentity = [](Trans2f m) {
        return m.multiply(m.inverse());
    };

    expect$(makeIdentity(Trans2f::rotate(10)).isIdentity());
    expect$(makeIdentity(Trans2f::skew(v)).isIdentity());
    expect$(makeIdentity(Trans2f::scale(v)).isIdentity());
    expect$(makeIdentity(Trans2f::translate(v)).isIdentity());
    expect$(makeIdentity(Trans2f::rotate(Math::PI / 2.0).translated(v / 2.0)).isIdentity());
    expect$(makeIdentity(Trans2f::translate(v).rotated(Math::PI / 2.0)).isIdentity());

    return Ok();
}

test$("transform-basics") {
    Vec2<f64> a{1.0, 0.0};
    Vec2<f64> b{0.0, 1.0};
    Vec2<f64> c{1.0, 1.0};

    expectEq$(Trans2f::translate({0.0, 0.0}).apply(a), a);
    expectEq$(Trans2f::translate({1.0, 1.0}).apply(a), (Vec2<f64>{2.0, 1.0}));
    expectEq$(Trans2f::translate({-1.0, -1.0}).apply(a), (Vec2<f64>{0.0, -1.0}));

    expectEq$(Trans2f::translate({0.0, 0.0}).apply(b), b);
    expectEq$(Trans2f::translate({1.0, 1.0}).apply(b), (Vec2<f64>{1.0, 2.0}));
    expectEq$(Trans2f::translate({-1.0, -1.0}).apply(b), (Vec2<f64>{-1.0, 0.0}));

    expectEq$(Trans2f::scale({2.0, 0.0}).apply(a), (Vec2<f64>{2.0, 0.0}));
    expectEq$(Trans2f::scale({0.0, 0.0}).apply(a), (Vec2<f64>{0.0, 0.0}));
    expectEq$(Trans2f::scale({1.0, 1.0}).apply(a), a);

    expectEq$(Trans2f::scale({0.0, 2.0}).apply(b), (Vec2<f64>{0.0, 2.0}));
    expectEq$(Trans2f::scale({0.0, 0.0}).apply(b), (Vec2<f64>{0.0, 0.0}));
    expectEq$(Trans2f::scale({1.0, 1.0}).apply(b), b);

    expectEq$(Trans2f::scale({2.0, 2.0}).apply(c), (Vec2<f64>{2.0, 2.0}));
    expectEq$(Trans2f::scale({1.0, 1.0}).apply(c), c);
    expectEq$(Trans2f::scale({0.0, 0.0}).apply(c), (Vec2<f64>{0.0, 0.0}));
    expectEq$(Trans2f::scale({-1.0, -1.0}).apply(c), (Vec2<f64>{-1.0, -1.0}));

    return Ok();
}

test$("transform-composite-1") {
    Vec2<f64> a{1.0, 0.0};
    Vec2<f64> b{0.0, 1.0};
    Vec2<f64> c{1.0, 1.0};

    auto m1 = Trans2f::translate({1.0, 1.0}).scaled({2.0, 2.0});
    auto m2 = Trans2f::scale({2.0, 2.0}).translated({1.0, 1.0});

    expectEq$(m1.apply(a), (Vec2<f64>{3.0, 1.0}));
    expectEq$(m1.apply(b), (Vec2<f64>{1.0, 3.0}));
    expectEq$(m1.apply(c), (Vec2<f64>{3.0, 3.0}));

    expectEq$(m2.apply(a), (Vec2<f64>{4.0, 2.0}));
    expectEq$(m2.apply(b), (Vec2<f64>{2.0, 4.0}));
    expectEq$(m2.apply(c), (Vec2<f64>{4.0, 4.0}));

    return Ok();
}

test$("transform-composite-2") {
    Vec2<f64> a{1.0, 0.0};
    Vec2<f64> b{0.0, 1.0};
    Vec2<f64> c{1.0, 1.0};

    auto m1 = Trans2f::translate({1.0, 1.0}).rotated(Math::PI / 2.0);
    auto m2 = Trans2f::rotate(Math::PI / 2.0).translated({1.0, 1.0});

    expectEq$(m1.apply(a), (Vec2<f64>{1.0, 2.0}));
    expectEq$(m1.apply(b), (Vec2<f64>{0.0, 1.0}));
    expectEq$(m1.apply(c), (Vec2<f64>{0.0, 2.0}));

    expectEq$(m2.apply(a), (Vec2<f64>{-1.0, 2.0}));
    expectEq$(m2.apply(b), (Vec2<f64>{-2.0, 1.0}));
    expectEq$(m2.apply(c), (Vec2<f64>{-2.0, 2.0}));

    return Ok();
}

} // namespace Karm::Math::Tests
