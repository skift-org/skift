#include <efi/base.h>
#include <karm-sys/defs.h>

import Karm.Core;

struct DebugOut : Io::TextWriter {
    Res<> writeRune(Rune rune) override {
        Utf16::One one;
        if (not Utf16::encodeUnit(rune, one))
            return Error::invalidInput("encoding error");

        // NOTE: outputString() wants a null terminated string bruh
        Array<u16, 3> cstr = {};
        copy(sub(one), mutSub(cstr));

        try$(Efi::st()->conOut->outputString(Efi::st()->conOut, cstr.buf()));

        return Ok();
    }
};

void __panicHandler(Karm::PanicKind kind, char const* msg) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, kind == Karm::PanicKind::PANIC ? (u16 const*)L"PANIC: " : (u16 const*)L"DEBUG: ").unwrap();
    DebugOut out{};
    (void)out.writeStr(Str{msg});
    Efi::st()->conOut->outputString(Efi::st()->conOut, (u16 const*)EMBED_SYS_LINE_ENDING_L).unwrap();

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
