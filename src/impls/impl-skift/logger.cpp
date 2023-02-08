#include <embed-logger/logger.h>
#include <hjert-api/syscalls.h>
#include <karm-sys/chan.h>

namespace Embed {

void loggerLock() {}

void loggerUnlock() {}

struct LoggerOut : public Io::TextWriter<> {
    Result<size_t> write(Bytes bytes) override {
        Hjert::Api::Log::call((char const *)bytes.buf(), bytes.len()).unwrap();
        return bytes.len();
    }
};

Io::TextWriter<> &loggerOut() {
    static LoggerOut _loggerOut{};
    return _loggerOut;
}

} // namespace Embed
