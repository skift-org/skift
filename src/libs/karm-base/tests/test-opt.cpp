#include <karm-base/opt.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("opt-default-constructor") {
    Opt<int> opt{};

    expect$(not opt.has());

    return Ok();
}

test$("opt-constructed") {
    Opt<int> opt{420};

    expect$(opt.has());
    expectEq$(opt.unwrap(), 420);

    return Ok();
}

test$("opt-assign") {
    Opt<int> opt{};

    opt = 420;

    expect$(opt.has());
    expectEq$(opt.unwrap(), 420);

    return Ok();
}

test$("opt-assign-none") {
    Opt<int> opt{420};

    opt = NONE;

    expect$(not opt.has());

    return Ok();
}

test$("opt-unwrap") {
    Opt<int> opt{420};

    expectEq$(opt.unwrap(), 420);

    return Ok();
}

test$("opt-take") {
    Opt<int> opt{420};

    expectEq$(opt.take(), 420);
    expect$(not opt.has());

    return Ok();
}

} // namespace Karm::Base::Tests
