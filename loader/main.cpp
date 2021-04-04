#include "loader/EFI.h"

#define SEC_TO_USEC(value) ((value)*1000 * 1000)

extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
    InitializeLib(image_handle, system_table);

    Print((CHAR16 *)L"Hello, world!\n");
    uefi_call_wrapper((void *)BS->Stall, 1, SEC_TO_USEC(5));

    return (EFI_SUCCESS);
}