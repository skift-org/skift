#include <libutils/Vector.h>

#include "kernel/devices/LegacyDevice.h"
#include "kernel/devices/PCIDevice.h"
#include "kernel/devices/UNIXDevice.h"

#include "kernel/drivers/BGA.h"
#include "kernel/drivers/E1000.h"
#include "kernel/drivers/LegacyKeyboard.h"
#include "kernel/drivers/LegacyMouse.h"
#include "kernel/drivers/LegacySerial.h"
#include "kernel/drivers/UnixNull.h"
#include "kernel/drivers/UnixRandom.h"
#include "kernel/drivers/UnixZero.h"
#include "kernel/drivers/VirtioBlock.h"
#include "kernel/drivers/VirtioConsole.h"
#include "kernel/drivers/VirtioEntropy.h"
#include "kernel/drivers/VirtioGraphic.h"
#include "kernel/drivers/VirtioNetwork.h"

static Vector<DeviceDriver *> *_drivers;

void driver_initialize()
{
    logger_info("Installing drivers...");

    _drivers = new Vector<DeviceDriver *>();

    _drivers->push_back(new PCIDeviceDriver<BGA>{"QEMU Graphics Adaptator", 0x1234, 0x1111});
    _drivers->push_back(new PCIDeviceDriver<BGA>{"Virtual Box Graphics Adaptator", 0x80ee, 0xbeef});

    _drivers->push_back(new PCIDeviceDriver<E1000>{"Intel 82577LM Ethernet Adaptator", 0x8086, 0x10EA});
    _drivers->push_back(new PCIDeviceDriver<E1000>{"Intel I217 Ethernet Adaptator", 0x8086, 0x153A});
    _drivers->push_back(new PCIDeviceDriver<E1000>{"Virtual Ethernet Adaptator", 0x8086, 0x100E});

    _drivers->push_back(new VirtioDeviceDriver<VirtioBlock>{"VirtI/O Block Device", VIRTIO_DEVICE_BLOCK});
    _drivers->push_back(new VirtioDeviceDriver<VirtioConsole>{"VirtI/O Console Device", VIRTIO_DEVICE_CONSOLE});
    _drivers->push_back(new VirtioDeviceDriver<VirtioEntropy>{"VirtI/O Entropy Device", VIRTIO_DEVICE_ENTROPY});
    _drivers->push_back(new VirtioDeviceDriver<VirtioGraphic>{"VirtI/O Graphic Device", VIRTIO_DEVICE_GRAPHICS});
    _drivers->push_back(new VirtioDeviceDriver<VirtioNetwork>{"VirtI/O Network Device", VIRTIO_DEVICE_NETWORK});

    _drivers->push_back(new LegacyDeviceDriver<LegacyKeyboard>{"Legacy Keyboard", LEGACY_KEYBOARD});
    _drivers->push_back(new LegacyDeviceDriver<LegacyMouse>{"Legacy Mouse", LEGACY_MOUSE});
    _drivers->push_back(new LegacyDeviceDriver<LegacySerial>{"Legacy Serial Port (COM1)", LEGACY_COM1});
    _drivers->push_back(new LegacyDeviceDriver<LegacySerial>{"Legacy Serial Port (COM2)", LEGACY_COM2});
    _drivers->push_back(new LegacyDeviceDriver<LegacySerial>{"Legacy Serial Port (COM3)", LEGACY_COM3});
    _drivers->push_back(new LegacyDeviceDriver<LegacySerial>{"Legacy Serial Port (COM4)", LEGACY_COM4});

    _drivers->push_back(new UNIXDeviceDriver<UnixNull>{"Unix Null Device", UNIX_NULL});
    _drivers->push_back(new UNIXDeviceDriver<UnixRandom>{"Unix Random Device", UNIX_RANDOM});
    _drivers->push_back(new UNIXDeviceDriver<UnixZero>{"Unix Zero Device", UNIX_ZERO});

    for (size_t i = 0; i < _drivers->count(); i++)
    {
        logger_info("Driver: %s", (*_drivers)[i]->name());
    }
}

DeviceDriver *driver_for(DeviceAddress address)
{
    for (size_t i = 0; i < _drivers->count(); i++)
    {
        if ((*_drivers)[i]->bus() == address.bus() &&
            (*_drivers)[i]->match(address))
        {
            return (*_drivers)[i];
        }
    }

    return nullptr;
}
