#include <embed-logger/logger.h>
#include <hjert-api/api.h>
#include <karm-sys/chan.h>

namespace Embed {

void loggerLock() {}

void loggerUnlock() {}

struct LoggerOut : public Io::TextWriterBase<> {
    Io::BufferWriter _buf;

    Res<usize> write(Bytes bytes) override {
        return _buf.write(bytes);
    }

    Res<usize> flush() override {
        try$(Hj::log(_buf.bytes()));
        return _buf.flush();
    }
};

Io::TextWriter &loggerOut() {
    static LoggerOut _loggerOut{};
    return _loggerOut;
}

} // namespace Embed
