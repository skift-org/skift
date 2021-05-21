#pragma once

namespace Arch::x86
{

void reboot_8042();

void reboot_triple_fault();

void shutdown_virtual_machines();

void shutdown_acpi();

} // namespace Arch::x86
