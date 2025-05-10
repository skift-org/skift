#include <karm-math/trans.h>
#include <karm-test/macros.h>

namespace Karm::Math::Tests {

test$("transform-inverse") {
    Vec2<double> transVec{1.0, 2.0};

    auto makeIdentity = [](Trans2f m) {
        return m.multiply(m.inverse());
    };

    expect$(makeIdentity(Trans2f::makeRotate(10)).isIdentity());
    expect$(makeIdentity(Trans2f::makeSkew(transVec)).isIdentity());
    expect$(makeIdentity(Trans2f::makeScale(transVec)).isIdentity());
    expect$(makeIdentity(Trans2f::makeTranslate(transVec)).isIdentity());

    return Ok();
}

} // namespace Karm::Math::Tests
