#include <efi/base.h>
#include <efi/spec.h>
#include <embed/debug.h>

namespace Embed {

[[noreturn]] void panicHandler(char const *buf) {
    auto str = String{buf}.transcoded<Utf16>();

    Efi::SystemTable().conOut->outputString(Efi::SystemTable().conOut, str.buf()).unwrap();

    (void)Efi::st()->runtime->resetSystem(
        Efi::ResetType::RESET_SHUTDOWN,
        Efi::ERR_UNSUPPORTED,
        0,
        nullptr);

    while (1)
        ;
}

} // namespace Embed
