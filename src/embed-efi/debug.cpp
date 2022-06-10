#include <efi/base.h>
#include <efi/spec.h>
#include <embed/debug.h>
#include <karm-base/string.h>

namespace Embed {

void debugHandler(char const *buf) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"DEBUG: ").unwrap();
    auto str = transcode<Utf16>(Str{buf});
    Efi::st()->conOut->outputString(Efi::st()->conOut, str.buf()).unwrap();
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"\n").unwrap();
}

[[noreturn]] void panicHandler(char const *buf) {
    Efi::st()->conOut->outputString(Efi::st()->conOut, (uint16_t const *)L"PANIC: ").unwrap();

    auto str = transcode<Utf16>(Str{buf});

    Efi::st()->conOut->outputString(Efi::st()->conOut, str.buf()).unwrap();
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
