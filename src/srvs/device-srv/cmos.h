#pragma once

#include <hal/io.h>

#include "base.h"

namespace Dev::Cmos {

struct Cmos;

struct Device : public Node {
    Cmos &_cmos;

    Cmos &cmos() {
        return _cmos;
    }

    Device(Cmos &cmos)
        : _cmos(cmos) {}
};

struct Rtc : public Device {
    using Device::Device;
};

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
};

} // namespace Dev::Cmos
