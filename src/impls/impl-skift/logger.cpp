#include <embed-logger/logger.h>
#include <hjert-api/api.h>
#include <karm-sys/chan.h>

namespace Embed {

void loggerLock() {}

void loggerUnlock() {}

struct LoggerOut : public Io::TextWriter<> {
    Res<size_t> write(Bytes bytes) override {
        Hj::log((char const *)bytes.buf(), bytes.len()).unwrap();
        return Ok(bytes.len());
    }
};

Io::TextWriter<> &loggerOut() {
    static LoggerOut _loggerOut{};
    return _loggerOut;
}

} // namespace Embed
