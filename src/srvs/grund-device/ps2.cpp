#include <karm-app/inputs.h>
#include <karm-logger/logger.h>
#include <karm-sys/rpc.h>

#include "ps2.h"

namespace Grund::Device::Ps2 {

// MARK: Controller ------------------------------------------------------------

Res<> I8042::init() {
    logInfo("ps2: i8042 initializing...");
    try$(attach(makeStrong<Keyboard>(*this)));
    try$(attach(makeStrong<Mouse>(*this)));
    try$(flush());
    return Ok();
}

Res<> I8042::flush() {
    logInfo("ps2: flushing...");
    auto status = try$(readStatus());
    while (status.has(Status::OUT_BUF)) {
        try$(readData());
        status = try$(readStatus());
    }
    return Ok();
}

// MARK: Data and Status

Res<Flags<Status>> I8042::readStatus() {
    auto status = try$(io().read<StatusReg>());
    return Ok(static_cast<Status>(status));
}

Res<> I8042::waitRead() {
    auto timeout = 100000;
    while (timeout--) {
        auto status = try$(readStatus());
        if (status.has(Status::OUT_BUF))
            return Ok();
    }
    return Error::timedOut("waiting for read");
}

Res<> I8042::waitWrite() {
    auto timeout = 100000;
    while (timeout--) {
        auto status = try$(readStatus());
        if (not status.has(Status::IN_BUF))
            return Ok();
    }
    return Error::timedOut("waiting for write");
}

Res<Byte> I8042::readData() {
    try$(waitRead());
    auto data = try$(io().read<DataReg>());
    return Ok(data);
}

Res<> I8042::writeData(Byte data) {
    try$(waitWrite());
    try$(io().write<DataReg>(data));
    return Ok();
}

Res<> I8042::writeCmd(Cmd cmd) {
    try$(waitWrite());
    try$(io().write<CmdReg>(toUnderlyingType(cmd)));
    return Ok();
}

// MARK: Configs

Res<> I8042::writeConfig(Flags<Configs> cfg) {
    try$(writeCmd(Cmd::WRITE_CONFIG));
    try$(writeData(cfg.underlying()));
    return Ok();
}

Res<Flags<Configs>> I8042::readConfig() {
    try$(writeCmd(Cmd::READ_CONFIG));
    auto cfg = try$(readData());
    return Ok(static_cast<Configs>(cfg));
}

// MARK: Keyboard --------------------------------------------------------------

Res<> Keyboard::init() {
    logInfo("ps2: keyboard initializing...");
    try$(ctrl().writeCmd(Cmd::ENABLE_MAIN));

    auto cfgs = try$(ctrl().readConfig());
    cfgs.set(Configs::FIRST_PORT_INTERRUPT_ENABLE);
    try$(ctrl().writeConfig(cfgs));

    logInfo("ps2: keyboard initialized");

    return Ok();
}

Res<> Keyboard::event(App::Event &e) {
    if (auto irq = e.is<IrqEvent>()) {
        if (irq->irq == 1) {
            auto status = try$(ctrl().readStatus());
            while (status.has(Status::OUT_BUF) and
                   not status.has(Status::AUX_BUF)) {
                auto data = try$(ctrl().readData());
                logInfo("ps2: keyboard data {:02x}", data);
                if (_esc) {
                    App::Key key = {App::Key::Code((data & 0x7F) + 0x80)};
                    logInfo("ps2: keyboard key {} {}", key.name(), data & 0x80 ? "pressed" : "released");
                    auto event = App::makeEvent<App::KeyboardEvent>(
                        data & 0x80 ? App::KeyboardEvent::PRESS : App::KeyboardEvent::RELEASE,
                        key,
                        key
                    );
                    try$(bubble(*event));
                    _esc = false;
                } else if (data == 0xE0) {
                    _esc = true;
                } else {
                    App::Key key = {App::Key::Code(data & 0x7F)};
                    logInfo("ps2: keyboard key {} {}", key.name(), data & 0x80 ? "pressed" : "released");
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

// MARK: Mouse -----------------------------------------------------------------

Res<> Mouse::init() {
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

Res<> Mouse::decode() {
    int offx = _buf[1];
    if (_buf[0] & 0x10)
        offx -= 0x100;

    int offy = _buf[2];
    if (_buf[0] & 0x20)
        offy -= 0x100;

    int scroll = 0;
    if (_hasWheel)
        scroll = (i8)_buf[3];

    auto event = App::makeEvent<App::MouseEvent>(App::MouseEvent::MOVE, 0, scroll, Math::Vec2i{offx, offy});
    logInfo("ps2: mouse move {} {} {}", offx, offy, scroll);
    try$(bubble(*event));

    return Ok();
}

Res<> Mouse::event(App::Event &e) {
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
}

} // namespace Grund::Device::Ps2
