#include <karm-logger/logger.h>

#include "cmos.h"

namespace Grund::Device::Cmos {

// MARK: CMOS ------------------------------------------------------------------

Cmos::Cmos(Rc<Hal::Io> io)
    : _io(std::move(io)) {}

Res<> Cmos::init() {
    logInfo("cmos: initializing...");

    try$(attach(makeRc<Rtc>(*this)));

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

// MARK: RTC -------------------------------------------------------------------

static inline u8 fomBcd(u8 value) {
    return (value / 16) * 10 + (value & 0xf);
}

Res<> Rtc::init() {
    logInfo("cmos: rtc initializing...");

    // NOTE: The RTC does not need to be initialized but
    //       we can still do some sanity checks
    auto dt = DateTime::fromInstant(try$(this->now()));
    logInfo("cmos: rtc date: {}", dt);
    if (dt.date.year < Year{2024})
        logWarn("cmos: rtc out of date, make sure to set the correct date");
    return Ok();
}

Res<SystemTime> Rtc::now() {
    try$(cmos().waitUpdate());

    DateTime dt;
    dt.time.second = fomBcd(try$(cmos().read(Cmos::SEC)));
    dt.time.minute = fomBcd(try$(cmos().read(Cmos::MIN)));
    dt.time.hour = fomBcd(try$(cmos().read(Cmos::HOUR)));
    dt.date.day = fomBcd(try$(cmos().read(Cmos::DAY))) - 1;
    dt.date.month = fomBcd(try$(cmos().read(Cmos::MONTH))) - 1;
    dt.date.year = fomBcd(try$(cmos().read(Cmos::YEAR))) + 2000;

    return Ok(dt.toInstant());
}

} // namespace Grund::Device::Cmos
