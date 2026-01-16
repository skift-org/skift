module;

#include <hjert-api/api.h>

module Karm.Logger;

import Karm.Sys;

namespace Karm::Logger::_Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter& loggerOut() {
    struct LoggerOut : Io::TextWriter {
        StringBuilder _sb;

        Res<> writeRune(Rune rune) override {
            Utf8::One one;
            if (not Utf8::encodeUnit(rune, one))
                return Error::invalidInput("encoding error");

            _sb.append(rune);
            if (rune == '\n') {
                try$(Hj::log(_sb.bytes()));
                _sb.clear();
            }

            return Ok();
        }
    };

    static LoggerOut out;
    return out;
}

} // namespace Karm::Logger::_Embed
