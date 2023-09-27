#include <karm-logger/logger.h>

#include "cmos.h"

namespace Grund::Device::Cmos {

/* --- CMOS ----------------------------------------------------------------- */

Cmos::Cmos(Strong<Hal::Io> io)
    : _io(std::move(io)) {}

Res<> Cmos::init() {
    logInfo("cmos: initializing...");

    try$(attach(makeStrong<Rtc>(*this)));

    return Ok();
}

Res<u8> Cmos::read(Addr addr) {
    try$(_io->write<AddrReg>(addr));
    return _io->read<DataReg>();
}

Res<> Cmos::write(Addr addr, u8 data) {
    try$(_io->write<AddrReg>(addr));
    return _io->write<DataReg>(data);
}

Res<> Cmos::waitUpdate() {
    while (true) {
        auto status = try$(read(STATUS_A));
        if (not(status & 0x80))
            break;
    }

    return Ok();
}

/* --- RTC ------------------------------------------------------------------ */

static inline uint8_t fomBcd(uint8_t value) {
    return (value / 16) * 10 + (value & 0xf);
}

Res<> Rtc::init() {
    logInfo("cmos: rtc initializing...");

    // NOTE: The RTC does not need to be initialized but
    //       we can still do some sanity checks
    auto dt = DateTime::fromTimeStamp(try$(this->now()));
    logInfo("cmos: rtc date: {}", dt);
    if (dt.date.year < Year{2023}) {
        logWarn("cmos: rtc out of date, make sure to set the correct date");
    }

    return Ok();
}

Res<TimeStamp> Rtc::now() {
    try$(cmos().waitUpdate());

    DateTime dt;
    dt.time.second = fomBcd(try$(cmos().read(Cmos::SEC)));
    dt.time.minute = fomBcd(try$(cmos().read(Cmos::MIN)));
    dt.time.hour = fomBcd(try$(cmos().read(Cmos::HOUR)));
    dt.date.day = fomBcd(try$(cmos().read(Cmos::DAY))) - 1;
    dt.date.month = fomBcd(try$(cmos().read(Cmos::MONTH))) - 1;
    dt.date.year = fomBcd(try$(cmos().read(Cmos::YEAR))) + 2000;

    return Ok(dt.toTimeStamp());
}

} // namespace Grund::Device::Cmos
