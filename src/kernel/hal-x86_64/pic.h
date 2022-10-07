#pragma once

#include <hal/io.h>

namespace x86_64 {

struct Pic {
    Hal::Io _io;

    static constexpr size_t PIC1 = 0x20;
    static constexpr size_t PIC2 = 0xA0;
    static constexpr size_t CMD = 0;
    static constexpr size_t DATA = 1;

    static constexpr size_t PIC1_OFFSET = 0x20;
    static constexpr size_t PIC2_OFFSET = 0x28;

    static constexpr size_t ICW1_ICW4 = 0x01;
    static constexpr size_t ICW1_INIT = 0x10;

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

    void cmd(uint8_t cmd) {
        _io.write8(CMD, cmd);
        wait();
    }

    void data(uint8_t data) {
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
