module;

#include <hal/io.h>

export module Strata.Device:cmos;

import Karm.Logger;
import Karm.Core;
import :node;

namespace Strata::Device::Cmos {

export struct Cmos;

struct Device : Node {
    Cmos& _cmos;

    Cmos& cmos() { return _cmos; }

    Device(Cmos& cmos) : _cmos(cmos) {}
};

// MARK: CMOS ------------------------------------------------------------------

struct Cmos : Node {
    using AddrReg = Hal::Reg<u8, 0x0>;
    using DataReg = Hal::Reg<u8, 0x1>;

    enum Addr : u8 {
        SEC = 0x0,
        MIN = 0x2,
        HOUR = 0x4,
        DAY = 0x7,
        MONTH = 0x8,
        YEAR = 0x9,
        STATUS_A = 0xA,
        STATUS_B = 0xB,
        STATUS_C = 0xC,
        STATUS_D = 0xD,
    };

    Rc<Hal::Io> _io;

    Cmos(Rc<Hal::Io> io)
        : _io(std::move(io)) {}

    Res<> init();

    Res<u8> read(Addr addr) {
        try$(_io->write<AddrReg>(addr));
        return _io->read<DataReg>();
    }

    Res<> write(Addr addr, u8 data) {
        try$(_io->write<AddrReg>(addr));
        return _io->write<DataReg>(data);
    }

    Res<> waitUpdate() {
        while (true) {
            auto status = try$(read(STATUS_A));
            if (not(status & 0x80))
                break;
        }

        return Ok();
    }
};

// MARK: RTC -------------------------------------------------------------------

static inline u8 fomBcd(u8 value) {
    return (value / 16) * 10 + (value & 0xf);
}

struct Rtc : Device {
    using Device::Device;

    Res<> init() {
        logInfo("cmos: rtc initializing...");

        // NOTE: The RTC does not need to be initialized but
        //       we can still do some sanity checks
        auto dt = DateTime::fromInstant(try$(this->now()));
        logInfo("cmos: rtc date: {}", dt);
        if (dt.date.year < Year{2024})
            logWarn("cmos: rtc out of date, make sure to set the correct date");
        return Ok();
    }

    Res<SystemTime> now() {
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
};

Res<> Cmos::init() {
    logInfo("cmos: initializing...");

    try$(attach(makeRc<Rtc>(*this)));

    return Ok();
}

} // namespace Strata::Device::Cmos
