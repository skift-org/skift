#include "ps2.h"
#include <karm-logger/logger.h>

namespace Dev::Ps2 {

/* --- Controller ----------------------------------------------------------- */

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

/* --- Data and Status --- */

Res<Flags<Status>> I8042::readStatus() {
    auto status = try$(io().read<StatusReg>());
    return Ok(static_cast<Status>(status));
}

Res<> I8042::waitRead() {
    auto timeout = 100000;
    while (timeout--) {
        auto status = try$(readStatus());
        if (status.has(Status::OUT_BUF)) {
            return Ok();
        }
    }
    return Error::timedOut("waiting for read");
}

Res<> I8042::waitWrite() {
    auto timeout = 100000;
    while (timeout--) {
        auto status = try$(readStatus());
        if (not status.has(Status::IN_BUF)) {
            return Ok();
        }
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

/* --- Configs --- */

Res<> I8042::writeConfig(Flags<Configs> cfg) {
    try$(writeCmd(Cmd::WRITE_CONFIG));
    try$(writeData(cfg));
    return Ok();
}

Res<Flags<Configs>> I8042::readConfig() {
    try$(writeCmd(Cmd::READ_CONFIG));
    auto cfg = try$(readData());
    return Ok(static_cast<Configs>(cfg));
}

/* --- Keyboard ------------------------------------------------------------- */

Res<> Keyboard::init() {
    logInfo("ps2: keyboard initializing...");
    try$(ctrl().writeCmd(Cmd::ENABLE_MAIN));
    auto cfgs = try$(ctrl().readConfig());
    cfgs.set(Configs::FIRST_PORT_INTERRUPT_ENABLE);
    try$(ctrl().writeConfig(cfgs));
    return Ok();
}

Res<> Keyboard::event(Events::Event &e) {
    if (auto const *irq = e.is<IrqEvent>()) {
        if (irq->irq == 1) {
            logInfo("ps2: keyboard irq %d", irq->irq);
            auto status = try$(ctrl().readStatus());
            while (status.has(Status::OUT_BUF) and
                   not status.has(Status::AUX_BUF)) {
                auto data = try$(ctrl().readData());
                logInfo("ps2: keyboard data %x", data);
                status = try$(ctrl().readStatus());
            }
        }
    }

    return Device::event(e);
}

/* --- Mouse ---------------------------------------------------------------- */

Res<> Mouse::init() {
    logInfo("ps2: mouse initializing...");
    try$(ctrl().writeCmd(Cmd::ENABLE_AUX));
    auto cfgs = try$(ctrl().readConfig());
    cfgs.set(Configs::SECOND_PORT_INTERRUPT_ENABLE);
    try$(ctrl().writeConfig(cfgs));
    return Ok();
}

Res<> Mouse::event(Events::Event &e) {
    if (auto const *irq = e.is<IrqEvent>()) {
        if (irq->irq == 12) {
            logInfo("ps2: mouse irq {}", irq->irq);
            auto status = try$(ctrl().readStatus());
            while (status.has(Status::OUT_BUF) and
                   status.has(Status::AUX_BUF)) {
                auto data = try$(ctrl().readData());
                logInfo("ps2: mouse data {}", data);
                status = try$(ctrl().readStatus());
            }
        }
    }

    return Device::event(e);
}

} // namespace Dev::Ps2
