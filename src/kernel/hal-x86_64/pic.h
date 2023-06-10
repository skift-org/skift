#pragma once

#include <hal/raw.h>

namespace x86_64 {

struct Pic {
    Hal::RawPortIo _io;

    static constexpr usize PIC1 = 0x20;
    static constexpr usize PIC2 = 0xA0;
    static constexpr usize CMD = 0;
    static constexpr usize DATA = 1;

    static constexpr usize PIC1_OFFSET = 0x20;
    static constexpr usize PIC2_OFFSET = 0x28;

    static constexpr usize ICW1_ICW4 = 0x01;
    static constexpr usize ICW1_INIT = 0x10;

    static Pic pic1() {
        return {Hal::RawPortIo({PIC1, 2})};
    }

    static Pic pic2() {
        return {Hal::RawPortIo({PIC2, 2})};
    }

    Res<> ack() {
        try$(_io.out8(CMD, 0x20));
        return Ok();
    }

    Res<> disable() {
        try$(_io.out8(DATA, 0xFF));
        return Ok();
    }

    static void wait() {
        asm volatile("jmp 1f; 1: jmp 1f; 1:");
    }

    Res<> cmd(u8 cmd) {
        try$(_io.out8(CMD, cmd));
        wait();
        return Ok();
    }

    Res<> data(u8 data) {
        try$(_io.out8(DATA, data));
        wait();
        return Ok();
    }
};

struct DualPic {
    Pic _pic1;
    Pic _pic2;

    static DualPic dualPic() {
        return {Pic::pic1(), Pic::pic2()};
    }

    Res<> init() {
        try$(_pic1.cmd(Pic::ICW1_INIT | Pic::ICW1_ICW4));
        try$(_pic2.cmd(Pic::ICW1_INIT | Pic::ICW1_ICW4));

        try$(_pic1.data(Pic::PIC1_OFFSET));
        try$(_pic2.data(Pic::PIC2_OFFSET));

        try$(_pic1.data(4));
        try$(_pic2.data(2));

        try$(_pic1.data(0x01));
        try$(_pic2.data(0x01));

        try$(_pic1.data(0x0));
        try$(_pic2.data(0x0));

        return Ok();
    }

    Res<> ack(isize intno) {
        if (intno >= 40) {
            try$(_pic2.ack());
        }

        try$(_pic1.ack());
        return Ok();
    };

    Res<> disable() {
        try$(_pic1.disable());
        try$(_pic2.disable());
        return Ok();
    }
};

} // namespace x86_64
