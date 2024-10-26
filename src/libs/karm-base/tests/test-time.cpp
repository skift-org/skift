#include <karm-base/time.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("timespan-builders") {
    auto ts = TimeSpan::fromUSecs(2);
    expectEq$(ts._value, 2uz);

    ts = TimeSpan::fromMSecs(2);
    expectEq$(ts._value, 2uz * 1000);

    ts = TimeSpan::fromSecs(2);
    expectEq$(ts._value, 2uz * 1000 * 1000);

    ts = TimeSpan::fromMinutes(2);
    expectEq$(ts._value, 2uz * 60 * 1000 * 1000);

    ts = TimeSpan::fromHours(2);
    expectEq$(ts._value, 2uz * 60 * 60 * 1000 * 1000);

    ts = TimeSpan::fromDays(2);
    expectEq$(ts._value, 2uz * 24 * 60 * 60 * 1000 * 1000);

    ts = TimeSpan::fromWeeks(2);
    expectEq$(ts._value, 2uz * 7 * 24 * 60 * 60 * 1000 * 1000);

    ts = TimeSpan::fromMonths(2);
    expectEq$(ts._value, 2uz * 4 * 7 * 24 * 60 * 60 * 1000 * 1000);

    ts = TimeSpan::fromYears(2);
    expectEq$(ts._value, 2uz * 12 * 4 * 7 * 24 * 60 * 60 * 1000 * 1000);

    return Ok();
}

test$("timestamp") {
    TimeStamp tst = TimeStamp::epoch();
    expectEq$(tst._value, 0uz);

    tst += TimeSpan::fromUSecs(2);
    expectEq$(tst._value, 2uz);

    tst += TimeSpan::fromMSecs(2);
    expectEq$(tst._value, 2uz * 1000 + 2uz);

    tst += TimeSpan::fromSecs(2);
    expectEq$(tst._value, 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += TimeSpan::fromMinutes(2);
    expectEq$(tst._value, 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += TimeSpan::fromHours(2);
    expectEq$(tst._value, 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += TimeSpan::fromDays(2);
    expectEq$(tst._value, 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += TimeSpan::fromWeeks(2);
    expectEq$(tst._value, 2uz * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += TimeSpan::fromMonths(2);
    expectEq$(tst._value, 2uz * 4 * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    tst += TimeSpan::fromYears(2);
    expectEq$(tst._value, 2uz * 12 * 4 * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 4 * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 7 * 24 * 60 * 60 * 1000 * 1000 + 2uz * 24 * 60 * 60 * 1000 * 1000 + 2uz * 60 * 60 * 1000 * 1000 + 2uz * 60 * 1000 * 1000 + 2uz * 1000 * 1000 + 2uz * 1000 + 2uz);

    return Ok();
}

} // namespace Karm::Base::Tests
