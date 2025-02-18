#include <karm-math/funcs.h>

#include "object.h"
#include "value.h"

namespace Vaev::Script {

// MARK: Numeric Types ---------------------------------------------------------
// https://tc39.es/ecma262/#sec-ecmascript-language-types-number-type

// https://tc39.es/ecma262/#sec-numeric-types-number-unaryMinus
Number Number::unaryMinus() {
    if (Math::isNan(_val))
        return Math::NAN;
    return -_val;
}

// https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseNOT
// Number Number::bitwiseNot() {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-exponentiate
// Number Number::exponentiate(Number exp) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-multiply
// Number Number::multiply(Number multiplicand) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-divide
// Number Number::divide(Number divisor) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-remainder
// Number Number::remainder(Number divisor) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-add
// Number Number::add(Number augend) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-subtract
// Number Number::subtract(Number subtrahend) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-leftShift
// Number Number::leftShift(Number shiftCount) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-signedRightShift
// Number Number::signedRightShift(Number shiftCount) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-unsignedRightShift
// Number Number::unsignedRightShift(Number shiftCount) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-lessThan
// Boolean Number::lessThan(Number y) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-equal
// Boolean Number::equal(Number y) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-sameValue
Boolean Number::sameValue(Number y) {
    // 1. If x is NaN and y is NaN, return true.
    // 2. If x is +0𝔽 and y is -0𝔽, return false.
    // 3. If x is -0𝔽 and y is +0𝔽, return false.
    // 4. If x is y, return true.
    return _val == y._val;
    // 5. Return false.
}

// https://tc39.es/ecma262/#sec-numeric-types-number-sameValueZero
// Boolean Number::sameValueZero(Number y) {
// }

// https://tc39.es/ecma262/#sec-numberbitwiseop
// Number Number::bitwiseOp(Number y, f64 op) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseAND
// Number Number::bitwiseAnd(Number y) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseXOR
// Number Number::bitwiseXor(Number y) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseOR
// Number Number::bitwiseOr(Number y) {
// }

// https://tc39.es/ecma262/#sec-numeric-types-number-tostring
// String Number::toString() {
// }

void Number::repr(Io::Emit& e) const {
    e("{}", _val);
}

// MARK: Value Type ------------------------------------------------------------
// https://tc39.es/ecma262/#sec-ecmascript-language-types

void Value::repr(Io::Emit& e) const {
    e("{}", store);
}

} // namespace Vaev::Script
