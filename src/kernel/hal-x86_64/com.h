#pragma once

#include <hal/io.h>
#include <karm-io/traits.h>

#include "asm.h"

namespace x86_64 {

struct Com : public Io::TextWriterBase<> {
    Hal::Io _io;

    Com(Hal::Io io) : _io(io) {
    }

    static Com com1() {
        return {Hal::Io::port(0x3F8, 8)};
    }

    static Com com2() {
        return {Hal::Io::port({0x2F8, 8})};
    }

    static Com com3() {
        return {Hal::Io::port({0x3E8, 8})};
    }

    static Com com4() {
        return {Hal::Io::port({0x2E8, 8})};
    }

    enum Regs {
        DATA = 0,
        INTERRUPT = 1,
        BAUD_RATE_LOW = 0,
        BAUD_RATE_HIGH = 1,
        INTERRUPT_IDENTIFICATOR = 2,
        FIFO_CONTROLLER = 2,
        LINE_CONTROL = 3,
        MODEM_CONTROL = 4,
        LINE_STATUS = 5,
        MODEM_STATUS = 6,
        SCRATCH_REGISTER = 7,
    };

    enum LineCtrls {
        DATA_SIZE_5 = 0,
        DATA_SIZE_6 = 1,
        DATA_SIZE_7 = 2,
        DATA_SIZE_8 = 3,
        DLAB_STATUS = 1 << 7,
    };

    enum ModemCtrl {
        MODEM_DTR = 1 << 0,
        MODEM_RTS = 1 << 1,
        MODEM_OUT1 = 1 << 2,
        MODEM_OUT2 = 1 << 3,
        MODEM_LOOPBACK = 1 << 4,
    };

    enum Int {
        WHEN_DATA_AVAILABLE = 1 << 0,
        WHEN_TRANSMITTER_EMPTY = 1 << 1,
        WHEN_BREAK_ERROR = 1 << 2,
        WHEN_STATUS_UPDATE = 1 << 3,
    };

    enum LineStatus {
        DATA_READY = 1 << 0,
        OVERRUN_ERROR = 1 << 1,
        PARITY_ERROR = 1 << 2,
        FRAMING_ERROR = 1 << 3,
        BREAK_INDICATOR = 1 << 4,
        TRANSMITTER_BUF_EMPTY = 1 << 5,
        TRANSMITTER_EMPTY = 1 << 6,
        IMPENDING_ERROR = 1 << 7,
    };

    void init() {
        writeReg(INTERRUPT_IDENTIFICATOR, 0);

        // Turn on dlab for setting baud rate
        writeReg(LINE_CONTROL, DLAB_STATUS);

        // Set bauds
        writeReg(BAUD_RATE_LOW, 115200 / 9600);
        writeReg(BAUD_RATE_HIGH, 0);

        // we want 8bit caracters + clear dlab
        writeReg(LINE_CONTROL, DATA_SIZE_8);

        // turn on communication + redirect UART interrupt into ICU
        writeReg(MODEM_CONTROL, MODEM_DTR | MODEM_RTS | MODEM_OUT2);
        writeReg(INTERRUPT, WHEN_DATA_AVAILABLE);
    }

    void writeReg(Regs reg, u8 value) {
        _io.write8(reg, value);
    }

    u8 readReg(Regs reg) {
        return _io.read8(reg);
    }

    bool canRead() {
        return readReg(LINE_STATUS) & DATA_READY;
    }

    bool canWrite() {
        return readReg(LINE_STATUS) & TRANSMITTER_BUF_EMPTY;
    }

    void waitWrite() {
        while (not canWrite()) {
        }
    }

    void waitRead() {
        while (not canRead()) {
        }
    }

    Res<usize> putByte(Byte c) {
        waitWrite();
        writeReg(DATA, static_cast<u8>(c));

        return Ok(1uz);
    }

    u8 getByte() {
        waitRead();
        return readReg(DATA);
    }

    Res<usize> write(Bytes bytes) override {
        for (auto b : iter(bytes)) {
            try$(putByte(b));
        }

        return Ok(sizeOf(bytes));
    }
};

} // namespace x86_64
