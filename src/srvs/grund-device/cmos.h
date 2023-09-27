#pragma once

#include <hal/io.h>

#include "node.h"

namespace Grund::Device::Cmos {

struct Cmos;

struct Device : public Node {
    Cmos &_cmos;

    Cmos &cmos() { return _cmos; }

    Device(Cmos &cmos) : _cmos(cmos) {}
};

/* --- CMOS ----------------------------------------------------------------- */

struct Cmos : public Node {
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

    Strong<Hal::Io> _io;

    Cmos(Strong<Hal::Io> io);

    Res<> init() override;

    Res<u8> read(Addr addr);

    Res<> write(Addr addr, u8 data);

    Res<> waitUpdate();
};

/* --- RTC ------------------------------------------------------------------ */

struct Rtc : public Device {
    using Device::Device;

    Res<> init() override;

    Res<TimeStamp> now();
};

} // namespace Grund::Device::Cmos
