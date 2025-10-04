module;

#include <hal/io.h>
#include <karm-logger/logger.h>
#include <karm-math/vec.h>

export module Strata.Device:ps2;

import Karm.Core;
import :node;

namespace Strata::Device::Ps2 {

export struct I8042;

struct Device : Node {
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

struct I8042 : Node {
    Rc<Hal::Io> _io;

    using DataReg = Hal::Reg<u8, 0x0>;
    using StatusReg = Hal::Reg<u8, 0x4>;
    using CmdReg = Hal::Reg<u8, 0x4>;

    I8042(Rc<Hal::Io> io)
        : _io(io) {}

    Hal::Io& io() {
        return *_io;
    }

    Res<> init();

    Res<> flush() {
        logInfo("ps2: flushing...");
        auto status = try$(readStatus());
        while (status.has(Status::OUT_BUF)) {
            try$(readData());
            status = try$(readStatus());
        }
        return Ok();
    }

    // MARK: Data and Status

    Res<Flags<Status>> readStatus() {
        auto status = try$(io().read<StatusReg>());
        return Ok(static_cast<Status>(status));
    }

    Res<> waitRead() {
        auto timeout = 100000;
        while (timeout--) {
            auto status = try$(readStatus());
            if (status.has(Status::OUT_BUF))
                return Ok();
        }
        return Error::timedOut("waiting for read");
    }

    Res<> waitWrite() {
        auto timeout = 100000;
        while (timeout--) {
            auto status = try$(readStatus());
            if (not status.has(Status::IN_BUF))
                return Ok();
        }
        return Error::timedOut("waiting for write");
    }

    Res<u8> readData() {
        try$(waitRead());
        auto data = try$(io().read<DataReg>());
        return Ok(data);
    }

    Res<> writeData(u8 data) {
        try$(waitWrite());
        try$(io().write<DataReg>(data));
        return Ok();
    }

    Res<> writeCmd(Cmd cmd) {
        try$(waitWrite());
        try$(io().write<CmdReg>(toUnderlyingType(cmd)));
        return Ok();
    }

    // MARK: Configs

    Res<> writeConfig(Flags<Configs> cfg) {
        try$(writeCmd(Cmd::WRITE_CONFIG));
        try$(writeData(cfg.raw()));
        return Ok();
    }

    Res<Flags<Configs>> readConfig() {
        try$(writeCmd(Cmd::READ_CONFIG));
        auto cfg = try$(readData());
        return Ok(static_cast<Configs>(cfg));
    }
};

struct Keyboard : Device {
    enum _Cmd : u8 {
        GET_SET_SCAN_CODE_SET = 0xF0,
    };

    bool _esc;

    using Device::Device;

    Res<> init() {
        logInfo("ps2: keyboard initializing...");
        try$(ctrl().writeCmd(Cmd::ENABLE_MAIN));

        auto cfgs = try$(ctrl().readConfig());
        cfgs.set(Configs::FIRST_PORT_INTERRUPT_ENABLE);
        try$(ctrl().writeConfig(cfgs));

        logInfo("ps2: keyboard initialized");

        return Ok();
    }

    Res<> event(App::Event& e) {
        if (auto irq = e.is<IrqEvent>()) {
            if (irq->irq == 1) {
                auto status = try$(ctrl().readStatus());
                while (status.has(Status::OUT_BUF) and
                    not status.has(Status::AUX_BUF)) {
                    auto data = try$(ctrl().readData());
                    if (_esc) {
                        App::Key key = {App::Key::Code((data & 0x7F) + 0x80)};
                        auto event = App::makeEvent<App::KeyboardEvent>(
                            data & 0x80 ? App::KeyboardEvent::PRESS : App::KeyboardEvent::RELEASE,
                            key,
                            key,
                            NONE
                        );
                        try$(bubble(*event));
                        _esc = false;
                    } else if (data == 0xE0) {
                        _esc = true;
                    } else {
                        App::Key key = {App::Key::Code(data & 0x7F)};
                        auto event = App::makeEvent<App::KeyboardEvent>(
                            data & 0x80 ? App::KeyboardEvent::PRESS : App::KeyboardEvent::RELEASE,
                            key,
                            key
                        );
                        try$(bubble(*event));
                    }
                    status = try$(ctrl().readStatus());
                }
            }
        }

        return Device::event(e);
    }

    Res<> sendCmd(_Cmd cmd) {
        try$(ctrl().writeData(cmd));
        return Ok();
    }

    Res<> sendCmd(_Cmd cmd, u8 data) {
        try$(ctrl().writeData(cmd));
        try$(ctrl().writeData(data));
        return Ok();
    }
};

struct Mouse : Device {
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

    Res<> init() {
        logInfo("ps2: mouse initializing...");
        try$(ctrl().writeCmd(Cmd::ENABLE_AUX));

        auto cfgs = try$(ctrl().readConfig());
        cfgs.set(Configs::SECOND_PORT_INTERRUPT_ENABLE);
        try$(ctrl().writeConfig(cfgs));

        try$(sendCmd(SET_DEFAULT));
        try$(sendCmd(ENABLE_REPORT));

        // enable scroll wheel
        logInfo("ps2: mouse enabling scroll wheel...");
        try$(getDeiceId());

        try$(sendCmd(SET_SAMPLE_RATE, 200));
        try$(sendCmd(SET_SAMPLE_RATE, 100));
        try$(sendCmd(SET_SAMPLE_RATE, 80));

        auto status = try$(getDeiceId());
        if (status == 0x03) {
            logInfo("ps2: mouse scroll wheel enabled");
            _hasWheel = true;
        } else {
            logInfo("ps2: mouse scroll wheel not supported");
            _hasWheel = false;
        }

        logInfo("ps2: mouse initialized");

        return Ok();
    }

    Res<> event(App::Event& e) {
        if (auto irq = e.is<IrqEvent>()) {
            if (irq->irq == 12) {
                auto status = try$(ctrl().readStatus());
                while (status.has(Status::OUT_BUF) and
                    status.has(Status::AUX_BUF)) {

                    auto data = try$(ctrl().readData());

                    switch (_cycle) {
                    case 0:
                        _buf[0] = data;
                        if (data & 0x08)
                            _cycle = 1;
                        break;

                    case 1:
                        _buf[1] = data;
                        _cycle = 2;
                        break;

                    case 2:
                        _buf[2] = data;
                        if (_hasWheel)
                            _cycle++;
                        else {
                            _cycle = 0;
                            try$(decode());
                        }
                        break;

                    case 3:
                        _buf[3] = data;
                        _cycle = 0;
                        try$(decode());
                        break;
                    }

                    status = try$(ctrl().readStatus());
                }
            }
        }

        return Device::event(e);
    };

    Res<> decode() {
        int offx = _buf[1];
        if (_buf[0] & 0x10)
            offx -= 0x100;

        int offy = _buf[2];
        if (_buf[0] & 0x20)
            offy -= 0x100;

        Flags buttons = App::MouseButton::NONE;

        buttons |= ((_buf[0] >> 0) & 1) ? App::MouseButton::LEFT : App::MouseButton::NONE;
        buttons |= ((_buf[0] >> 1) & 1) ? App::MouseButton::RIGHT : App::MouseButton::NONE;
        buttons |= ((_buf[0] >> 2) & 1) ? App::MouseButton::MIDDLE : App::MouseButton::NONE;

        i8 scroll = 0;
        if (_hasWheel)
            scroll = static_cast<i8>(_buf[3]);

        auto event = App::makeEvent<App::MouseEvent>(
            App::MouseEvent::MOVE,
            0,
            scroll,
            Math::Vec2i{offx, -offy},
            buttons
        );
        try$(bubble(*event));

        return Ok();
    }

    Res<u8> sendCmd(_Cmd cmd) {
        try$(ctrl().writeCmd(Cmd::WRITE_IN_AUX));
        try$(ctrl().writeData(cmd));
        return Ok(try$(ctrl().readData()));
    }

    Res<u8> sendCmd(_Cmd cmd, u8 data) {
        try$(sendCmd(cmd));
        try$(ctrl().writeCmd(Cmd::WRITE_IN_AUX));
        try$(ctrl().writeData(data));
        return Ok(try$(ctrl().readData()));
    }

    Res<u8> getDeiceId() {
        try$(sendCmd(GET_DEVICE_ID));
        return ctrl().readData();
    }
};

Res<> I8042::init() {
    logInfo("ps2: i8042 initializing...");
    try$(attach(makeRc<Keyboard>(*this)));
    try$(attach(makeRc<Mouse>(*this)));
    try$(flush());
    return Ok();
}

} // namespace Strata::Device::Ps2
