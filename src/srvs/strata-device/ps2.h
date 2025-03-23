#pragma once

#include <hal/io.h>
#include <karm-base/enum.h>

#include "node.h"

namespace Strata::Device::Ps2 {

struct I8042;

struct Device : public Node {
    I8042& _ctrl;

    I8042& ctrl() {
        return _ctrl;
    }

    Device(I8042& ctrl)
        : _ctrl(ctrl) {}

    virtual ~Device() = default;
};

enum struct Status : u8 {
    OUT_BUF = 1 << 0,
    IN_BUF = 1 << 1,
    SYS = 1 << 2,
    CMD_DATA = 1 << 3,
    KEY_LOCK = 1 << 4,
    AUX_BUF = 1 << 5,
    TIMEOUT = 1 << 6,
    PARITY = 1 << 7,
};

FlagsEnum$(Status);

enum struct Cmd : u8 {
    READ_CONFIG = 0x20,
    WRITE_CONFIG = 0x60,
    DISABLE_AUX = 0xA7,
    ENABLE_AUX = 0xA8,
    TEST_AUX = 0xA9,
    TEST_CONTROLLER = 0xAA,
    TEST_MAIN = 0xAB,
    DISABLE_MAIN = 0xAD,
    ENABLE_MAIN = 0xAE,
    WRITE_OUT_MAIN = 0xD2,
    WRITE_OUT_AUX = 0xD3,
    WRITE_IN_AUX = 0xD4,
};

enum struct Configs : u8 {
    FIRST_PORT_INTERRUPT_ENABLE = (1 << 0),
    SECOND_PORT_INTERRUPT_ENABLE = (1 << 1),
    FIRST_PORT_CLOCK_DISABLE = (1 << 4),
    SECOND_PORT_CLOCK_DISABLE = (1 << 5),
    FIRST_TRANSLATION = (1 << 6),
};

FlagsEnum$(Configs);

struct I8042 : public Node {
    Rc<Hal::Io> _io;

    using DataReg = Hal::Reg<u8, 0x0>;
    using StatusReg = Hal::Reg<u8, 0x4>;
    using CmdReg = Hal::Reg<u8, 0x4>;

    I8042(Rc<Hal::Io> io)
        : _io(io) {}

    Hal::Io& io() {
        return *_io;
    }

    Res<> init() override;

    Res<> flush();

    // MARK: Data and Status

    Res<Flags<Status>> readStatus();

    Res<> waitRead();

    Res<> waitWrite();

    Res<Byte> readData();

    Res<> writeData(Byte data);

    Res<> writeCmd(Cmd cmd);

    // MARK: Configs

    Res<> writeConfig(Flags<Configs> cfg);

    Res<Flags<Configs>> readConfig();
};

struct Keyboard : public Device {
    enum _Cmd : u8 {
        GET_SET_SCAN_CODE_SET = 0xF0,
    };

    bool _esc;

    using Device::Device;

    Res<> init() override;

    Res<> event(App::Event& e) override;

    Res<> sendCmd(_Cmd cmd) {
        try$(ctrl().writeData(cmd));
        return Ok();
    }

    Res<> sendCmd(_Cmd cmd, Byte data) {
        try$(ctrl().writeData(cmd));
        try$(ctrl().writeData(data));
        return Ok();
    }
};

struct Mouse : public Device {
    enum _Cmd : u8 {
        GET_DEVICE_ID = 0xF2,
        SET_SAMPLE_RATE = 0xF3,
        ENABLE_REPORT = 0xF4,
        SET_DEFAULT = 0xF6,
    };

    u8 _cycle;
    Array<u8, 4> _buf;
    bool _hasWheel;

    using Device::Device;

    Res<> init() override;

    Res<> event(App::Event& e) override;

    Res<> decode();

    Res<Byte> sendCmd(_Cmd cmd) {
        try$(ctrl().writeCmd(Cmd::WRITE_IN_AUX));
        try$(ctrl().writeData(cmd));
        return Ok(try$(ctrl().readData()));
    }

    Res<Byte> sendCmd(_Cmd cmd, Byte data) {
        try$(sendCmd(cmd));
        try$(ctrl().writeCmd(Cmd::WRITE_IN_AUX));
        try$(ctrl().writeData(data));
        return Ok(try$(ctrl().readData()));
    }

    Res<Byte> getDeiceId() {
        try$(sendCmd(GET_DEVICE_ID));
        return ctrl().readData();
    }
};

} // namespace Strata::Device::Ps2
