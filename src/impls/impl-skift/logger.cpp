#include <hjert-api/api.h>
#include <karm-logger/_embed.h>

import Karm.Sys;

namespace Karm::Logger::_Embed {

void loggerLock() {}

void loggerUnlock() {}

struct LoggerOut : public Io::TextEncoderBase<> {
    Io::BufferWriter _buf;

    Res<usize> write(Bytes bytes) override {
        return _buf.write(bytes);
    }

    Res<> flush() override {
        try$(Hj::log(_buf.bytes()));
        return _buf.flush();
    }
};

Io::TextWriter& loggerOut() {
    static LoggerOut _loggerOut{};
    return _loggerOut;
}

} // namespace Karm::Logger::_Embed
