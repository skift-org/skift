#include <assert.h>
#include <libsystem/core/Plugs.h>

#include "archs/Arch.h"
#include "archs/x86/COM.h"
#include "archs/x86/CPUID.h"
#include "archs/x86/FPU.h"
#include "archs/x86/LAPIC.h"
#include "archs/x86/PIC.h"
#include "archs/x86/PIT.h"
#include "archs/x86/Power.h"
#include "archs/x86/RTC.h"
#include "archs/x86_32/GDT.h"
#include "archs/x86_32/IDT.h"
#include "archs/x86_32/Interrupts.h"
#include "archs/x86_32/x86_32.h"

#include "smbios/SMBIOS.h"
#include "system/graphics/EarlyConsole.h"
#include "system/graphics/Graphics.h"
#include "system/system/System.h"
