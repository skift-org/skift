#pragma once

#include <abi-ms/abi.h>
#include <efi/base.h>
#include <karm-fmt/fmt.h>
#include <karm-main/base.h>
#include <karm-sys/chan.h>

#ifdef EMBED_EFI_MAIN_IMPL

extern "C" Efi::Status efi_main(Efi::Handle handle, Efi::SystemTable *st) {
    Efi::init(handle, st);
    Abi::Ms::init();

    char const *argv[] = {"efi-app", nullptr};
    CliArgs args{1, argv};

    ExitCode code = entryPoint(args);

    if (!code) {
        Error error = code.none();
        (void)Fmt::format(Sys::err(), "{}: {}\n", args.self(), error.msg());
        return 1;
    }

    return EFI_SUCCESS;
}

#endif // EMBED_EFI_MAIN_IMPL
