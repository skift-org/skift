#include <efi/base.h>
#include <karm-base/align.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

struct DebugOut : public Io::TextWriterBase<> {
    Res<usize> write(Bytes bytes) override {
        usize writen{};
        Array<u16, 129> buf{};
        // Some space for the null terminator.
        auto chunkSize = sizeOf(buf) - sizeof(u16);

        while (not isEmpty(bytes)) {
            usize toCopy = alignDown(sizeOf(bytes), sizeof(u16));

            // We need to copy the bytes into to a u16 aligned buffer.
            copy(sub(bytes, 0, toCopy), mutBytes(buf));

            // If bytes.size() is not a multiple of sizeof(u16),
            // then the last byte will be ignored.
            buf[toCopy / sizeof(u16) + 1] = 0;

            try$(Efi::st()->conOut->outputString(Efi::st()->conOut, buf.buf()));
            writen += toCopy;

            bytes = next(bytes, chunkSize);
        }

        return Ok(writen);
    }
};

void __panicHandler(Karm::PanicKind kind, char const *msg) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, kind == Karm::PanicKind::PANIC ? (u16 const *)L"PANIC: " : (u16 const *)L"DEBUG: ").unwrap();

    DebugOut out{};
    (void)out.writeStr(Str{msg});
    Efi::st()->conOut->outputString(Efi::st()->conOut, (u16 const *)EMBED_SYS_LINE_ENDING_L).unwrap();

    if (kind == Karm::PanicKind::PANIC) {
        (void)Efi::st()->runtime->resetSystem(
            Efi::ResetType::RESET_SHUTDOWN,
            Efi::ERR_UNSUPPORTED,
            0,
            nullptr
        );

        while (1)
            ;
    }
}
