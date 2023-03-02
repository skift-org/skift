#pragma once

#include <hal/io.h>

namespace x86_64 {

struct Pic {
    Hal::Io _io;

    static constexpr usize PIC1 = 0x20;
    static constexpr usize PIC2 = 0xA0;
    static constexpr usize CMD = 0;
    static constexpr usize DATA = 1;

    static constexpr usize PIC1_OFFSET = 0x20;
    static constexpr usize PIC2_OFFSET = 0x28;

    static constexpr usize ICW1_ICW4 = 0x01;
    static constexpr usize ICW1_INIT = 0x10;

    static Pic pic1() {
        return {Hal::Io::port({PIC1, 2})};
    }

    static Pic pic2() {
        return {Hal::Io::port({PIC2, 2})};
    }

    void ack() {
        _io.write8(CMD, 0x20);
    }

    void disable() {
        _io.write8(DATA, 0xFF);
    }

    static void wait() {
        asm volatile("jmp 1f; 1: jmp 1f; 1:");
    }

    void cmd(u8 cmd) {
        _io.write8(CMD, cmd);
        wait();
    }

    void data(u8 data) {
        _io.write8(DATA, data);
        wait();
    }
};

struct DualPic {
    Pic _pic1;
    Pic _pic2;

    static DualPic dualPic() {
        return {Pic::pic1(), Pic::pic2()};
    }

    void init() {
        _pic1.cmd(Pic::ICW1_INIT | Pic::ICW1_ICW4);
        _pic2.cmd(Pic::ICW1_INIT | Pic::ICW1_ICW4);

        _pic1.data(Pic::PIC1_OFFSET);
        _pic2.data(Pic::PIC2_OFFSET);

        _pic1.data(4);
        _pic2.data(2);

        _pic1.data(0x01);
        _pic2.data(0x01);

        _pic1.data(0x0);
        _pic2.data(0x0);
    }

    void ack(isize intno) {
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
