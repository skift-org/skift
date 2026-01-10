#pragma once

import Karm.Core;
import Karm.Sys;

#include <abi-ms/abi.h>
#include <vaerk-efi/base.h>

void __panicHandler(Karm::PanicKind kind, char const* msg);

extern "C" Efi::Status efi_main(Efi::Handle handle, Efi::SystemTable* st) {
    Efi::init(handle, st);
    Abi::Ms::init();
    Karm::registerPanicHandler(__panicHandler);

    (void)Efi::st()
        ->conOut->clearScreen(Efi::st()->conOut);

    char const* self = "efi-app";
    char const* argv[] = {self, nullptr};
    Sys::Context ctx;
    ctx.add<Sys::ArgsHook>(1, argv);

    Async::Cancellation cancellation;

    Res<> code = Async::run(entryPointAsync(ctx, cancellation.token()));
    if (not code) {
        Error error = code.none();
        (void)Io::format(Sys::err(), "{}: {}\n", self, error.msg());
        return 1;
    }

    return EFI_SUCCESS;
}
