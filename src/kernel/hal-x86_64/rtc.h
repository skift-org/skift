#pragma once

#include <hal/io.h>
#include <karm-base/time.h>

namespace x86_64 {

struct Rtc {
    Hal::Io _io;
    bool _hasCentury;

    static constexpr size_t RTC = 0x70;

    static Rtc rtc() {
        return {Hal::Io::port({RTC, 2}), false};
    }

    uint8_t read(uint8_t reg) {
        _io.write8(0, reg);
        return _io.read8(1);
    }

    uint8_t readBcd(uint8_t reg) {
        uint8_t bcd = read(reg);
        return (bcd & 0xf) + ((bcd >> 4) * 10);
    }

    void write(uint8_t reg, uint8_t data) {
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
