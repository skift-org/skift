#include <karm-math/trans.h>
#include <karm-test/macros.h>

namespace Karm::Math::Tests {

test$("transform-inverse") {
    Vec2<double> transVec{1.0, 2.0};

    auto makeIdentity = [](Trans2f m) {
        return m.multiply(m.inverse());
    };

    expect$(makeIdentity(Trans2f::rotate(10)).isIdentity());
    expect$(makeIdentity(Trans2f::skew(transVec)).isIdentity());
    expect$(makeIdentity(Trans2f::scale(transVec)).isIdentity());
    expect$(makeIdentity(Trans2f::translate(transVec)).isIdentity());

    return Ok();
}

} // namespace Karm::Math::Tests
