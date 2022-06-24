#include <efi/base.h>
#include <efi/spec.h>
#include <embed/debug.h>
#include <karm-base/align.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Embed {

struct DebugOut : public Io::TextWriter<Utf16> {
    Result<size_t> write(Bytes bytes) override {
        size_t writen{};
        Array<uint16_t, 129> buf{};
        // Some space for the null terminator.
        auto chunkSize = buf.size() - sizeof(uint16_t);

        while (!bytes.empty()) {
            size_t toCopy = alignDown(bytes.size(), sizeof(uint16_t));

            // We need to copy the bytes into to a uint16_t aligned buffer.
            bytes
                .sub<Bytes>(0, toCopy)
                .copyTo(buf.mutBytes());

            // If bytes.size() is not a multiple of sizeof(uint16_t),
            // then the last byte will be ignored.
            buf[toCopy / sizeof(uint16_t) + 1] = 0;

            writen += try$(Efi::st()->conOut->outputString(Efi::st()->conOut, buf));

            bytes = bytes.sub(chunkSize);
        }

        return writen;
    }
};

void debugHandler(char const *buf) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"DEBUG: ").unwrap();
    DebugOut out{};
    (void)out.writeStr(buf);
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"\n").unwrap();
}

[[noreturn]] void panicHandler(char const *buf) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"PANIC: ").unwrap();

    DebugOut out{};
    (void)out.writeStr(buf);

    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"\n").unwrap();

    (void)Efi::st()->runtime->resetSystem(
        Efi::ResetType::RESET_SHUTDOWN,
        Efi::ERR_UNSUPPORTED,
        0,
        nullptr);

    while (1)
        ;
}

} // namespace Embed
