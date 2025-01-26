#include <karm-base/time.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("time-duration-builder") {
    auto ts = Duration::fromUSecs(2);
    expectEq$(ts._value, 2uz);

    ts = Duration::fromMSecs(2);
    expectEq$(ts._value, 2uz * 1000);

    ts = Duration::fromSecs(2);
    expectEq$(ts._value, 2uz * 1000 * 1000);

    ts = Duration::fromMinutes(2);
    expectEq$(ts._value, 2uz * 60 * 1000 * 1000);

    ts = Duration::fromHours(2);
    expectEq$(ts._value, 2uz * 60 * 60 * 1000 * 1000);

    ts = Duration::fromDays(2);
    expectEq$(ts._value, 2uz * 24 * 60 * 60 * 1000 * 1000);

    ts = Duration::fromWeeks(2);
    expectEq$(ts._value, 2uz * 7 * 24 * 60 * 60 * 1000 * 1000);

    ts = Duration::fromMonths(2);
    expectEq$(ts._value, 2uz * 4 * 7 * 24 * 60 * 60 * 1000 * 1000);

    ts = Duration::fromYears(2);
    expectEq$(ts._value, 2uz * 12 * 4 * 7 * 24 * 60 * 60 * 1000 * 1000);

    return Ok();
}

test$("time-instant") {
    Instant tst = Instant::epoch();
    expectEq$(tst._value, 0uz);

    tst += Duration::fromUSecs(2);
    expectEq$(tst._value, 2uz);

    tst += Duration::fromMSecs(2);
    expectEq$(tst._value, 2uz * 1000 + 2uz);

    tst += Duration::fromSecs(2);
    expectEq$(tst._value, 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += Duration::fromMinutes(2);
    expectEq$(tst._value, 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += Duration::fromHours(2);
    expectEq$(tst._value, 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += Duration::fromDays(2);
    expectEq$(tst._value, 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += Duration::fromWeeks(2);
    expectEq$(tst._value, 2uz * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += Duration::fromMonths(2);
    expectEq$(tst._value, 2uz * 4 * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += Duration::fromYears(2);
    expectEq$(tst._value, 2uz * 12 * 4 * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 4 * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    return Ok();
}

} // namespace Karm::Base::Tests
