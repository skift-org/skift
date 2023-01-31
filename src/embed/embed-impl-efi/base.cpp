#include <efi/base.h>
#include <efi/spec.h>
#include <embed/base.h>
#include <karm-base/align.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Embed {

struct DebugOut : public Io::TextWriter<> {
    Result<size_t> write(Bytes bytes) override {
        size_t writen{};
        Array<uint16_t, 129> buf{};
        // Some space for the null terminator.
        auto chunkSize = sizeOf(buf) - sizeof(uint16_t);

        while (not isEmpty(bytes)) {
            size_t toCopy = alignDown(sizeOf(bytes), sizeof(uint16_t));

            // We need to copy the bytes into to a uint16_t aligned buffer.
            copy(sub(bytes, 0, toCopy), mutBytes(buf));

            // If bytes.size() is not a multiple of sizeof(uint16_t),
            // then the last byte will be ignored.
            buf[toCopy / sizeof(uint16_t) + 1] = 0;

            writen += try$(Efi::st()->conOut->outputString(Efi::st()->conOut, buf.buf()));

            bytes = next(bytes, chunkSize);
        }

        return writen;
    }
};

void debug(char const *buf) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"DEBUG: ").unwrap();
    DebugOut out{};
    (void)out.writeStr(buf);
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)EMBED_SYS_LINE_ENDING_L).unwrap();
}

[[noreturn]] void panic(char const *buf) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"PANIC: ").unwrap();

    DebugOut out{};
    (void)out.writeStr(buf);

    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)EMBED_SYS_LINE_ENDING_L).unwrap();

    (void)Efi::st()->runtime->resetSystem(
        Efi::ResetType::RESET_SHUTDOWN,
        Efi::ERR_UNSUPPORTED,
        0,
        nullptr);

    while (1)
        ;
}

} // namespace Embed
