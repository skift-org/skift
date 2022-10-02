#pragma once

#include <hal/io.h>

namespace x86_64 {

struct Pic {
    Hal::Io _io;

    static constexpr size_t PIC1 = 0x20;
    static constexpr size_t PIC2 = 0xA0;

    static Pic pic1() {
        return {Hal::Io::port({PIC1, 2})};
    }

    static Pic pic2() {
        return {Hal::Io::port({PIC2, 2})};
    }

    void init();

    void ack() {
        _io.write8(0, 0x20);
    }

    void disable() {
        _io.write8(1, 0xFF);
    }
};

struct DualPic {
    Pic _pic1;
    Pic _pic2;

    static DualPic dualPic() {
        return {Pic::pic1(), Pic::pic2()};
    }

    void init() {}

    void ack(int intno) {
        if (intno >= 40) {
            _pic2.ack();
        }
        
        _pic1.ack();
    };

    void disable() {
        _pic1.disable();
        _pic2.disable();
    }
};

} // namespace x86_64
