#include <libsystem/Logger.h>
#include <libutils/String.h>

#include "kernel/acpi/ACPI.h"
#include "kernel/acpi/Tables.h"
#include "kernel/memory/MemoryWindow.h"

void acpi_initialize(Multiboot *multiboot)
{
    logger_info("Initializing ACPI sub-system...");

    auto rsdp_window = MemoryWindow<RSDP>(multiboot->acpi_rsdp_address, multiboot->acpi_rsdp_size);

    logger_debug("Cool %s", String(rsdp_window->signature, 8).cstring());

    auto rsdt_window = MemoryWindow<RSDT>(rsdp_window->rsdt_address, sizeof(RSDT));

    logger_debug("Cool %s", String(rsdt_window->header.Signature, 4).cstring());
}
