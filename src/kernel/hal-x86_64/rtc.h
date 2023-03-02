#pragma once

#include <hal/io.h>
#include <karm-base/time.h>

namespace x86_64 {

struct Rtc {
    Hal::Io _io;
    bool _hasCentury;

    static constexpr usize RTC = 0x70;

    static Rtc rtc() {
        return {Hal::Io::port({RTC, 2}), false};
    }

    u8 read(u8 reg) {
        _io.write8(0, reg);
        return _io.read8(1);
    }

    u8 readBcd(u8 reg) {
        u8 bcd = read(reg);
        return (bcd & 0xf) + ((bcd >> 4) * 10);
    }

    void write(u8 reg, u8 data) {
        _io.write8(0, reg);
        _io.write8(1, data);
    }

    void wait() {
        while (read(0xA) & 0x80) {
        }
    }

    Karm::DateTime now() {
        wait();

        Karm::DateTime dt = {};
        dt.second = readBcd(0);
        dt.minute = readBcd(2);
        dt.hour = readBcd(4);
        dt.day = readBcd(7);
        dt.month = readBcd(8);
        dt.year = readBcd(9);
        dt.year += _hasCentury ? readBcd(0x32) * 100 : 2000;

        return dt;
    }
};

} // namespace x86_64
