#include "system/Streams.h"
#include <libutils/Vec.h>

#include "pci/PCIDevice.h"
#include "ps2/LegacyDevice.h"
#include "unix/UNIXDevice.h"

#include "bga/BGA.h"
#include "com/LegacySerial.h"
#include "ps2/LegacyKeyboard.h"
#include "ps2/LegacyMouse.h"
#include "unix/UnixNull.h"
#include "unix/UnixRandom.h"
#include "unix/UnixZero.h"

static Vec<DeviceMatcher *> *_matchers;

void driver_initialize()
{
    Kernel::logln("Installing drivers...");

    _matchers = new Vec<DeviceMatcher *>();

    _matchers->push_back(new PCIDeviceMatcher<BGA>{"QEMU Graphics Adaptator", 0x1234, 0x1111});
    _matchers->push_back(new PCIDeviceMatcher<BGA>{"Virtual Box Graphics Adaptator", 0x80ee, 0xbeef});

    _matchers->push_back(new LegacyDeviceMatcher<LegacyKeyboard>{"Legacy Keyboard", LEGACY_KEYBOARD});
    _matchers->push_back(new LegacyDeviceMatcher<LegacyMouse>{"Legacy Mouse", LEGACY_MOUSE});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM1)", LEGACY_COM1});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM2)", LEGACY_COM2});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM3)", LEGACY_COM3});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM4)", LEGACY_COM4});

    _matchers->push_back(new UNIXDeviceMatcher<UnixNull>{"Unix Null Device", UNIX_NULL});
    _matchers->push_back(new UNIXDeviceMatcher<UnixRandom>{"Unix Random Device", UNIX_RANDOM});
    _matchers->push_back(new UNIXDeviceMatcher<UnixZero>{"Unix Zero Device", UNIX_ZERO});

    for (size_t i = 0; i < _matchers->count(); i++)
    {
        Kernel::logln("Driver: {}", (*_matchers)[i]->name());
    }
}

DeviceMatcher *driver_for(DeviceAddress address)
{
    for (size_t i = 0; i < _matchers->count(); i++)
    {
        if ((*_matchers)[i]->bus() == address.bus() &&
            (*_matchers)[i]->match(address))
        {
            return (*_matchers)[i];
        }
    }

    return nullptr;
}
