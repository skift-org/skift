#include <libsystem/Logger.h>
#include <libutils/Vector.h>

#include "pci/PCIDevice.h"
#include "ps2/LegacyDevice.h"
#include "unix/UNIXDevice.h"

#include "ac97/AC97.h"
#include "ata/LegacyATA.h"
#include "bga/BGA.h"
#include "com/LegacySerial.h"
#include "e1000/E1000.h"
#include "pcspkr/PCSpeaker.h"
#include "ps2/LegacyKeyboard.h"
#include "ps2/LegacyMouse.h"
#include "unix/UnixNull.h"
#include "unix/UnixRandom.h"
#include "unix/UnixZero.h"
#include "virtio/VirtioBlock.h"
#include "virtio/VirtioConsole.h"
#include "virtio/VirtioEntropy.h"
#include "virtio/VirtioGraphic.h"
#include "virtio/VirtioNetwork.h"

static Vector<DeviceMatcher *> *_matchers;

void driver_initialize()
{
    logger_info("Installing drivers...");

    _matchers = new Vector<DeviceMatcher *>();

    _matchers->push_back(new PCIDeviceMatcher<BGA>{"QEMU Graphics Adaptator", 0x1234, 0x1111});
    _matchers->push_back(new PCIDeviceMatcher<BGA>{"Virtual Box Graphics Adaptator", 0x80ee, 0xbeef});

    _matchers->push_back(new PCIDeviceMatcher<E1000>{"Intel 82577LM Ethernet Adaptator", 0x8086, 0x10EA});
    _matchers->push_back(new PCIDeviceMatcher<E1000>{"Intel I217 Ethernet Adaptator", 0x8086, 0x153A});
    _matchers->push_back(new PCIDeviceMatcher<E1000>{"Virtual Ethernet Adaptator", 0x8086, 0x100E});

    _matchers->push_back(new PCIDeviceMatcher<AC97>{"Intel ICH", 0x8086, 0x2415});

    _matchers->push_back(new VirtioDeviceMatcher<VirtioBlock>{"VirtI/O Block Device", VIRTIO_DEVICE_BLOCK});
    _matchers->push_back(new VirtioDeviceMatcher<VirtioConsole>{"VirtI/O Console Device", VIRTIO_DEVICE_CONSOLE});
    _matchers->push_back(new VirtioDeviceMatcher<VirtioEntropy>{"VirtI/O Entropy Device", VIRTIO_DEVICE_ENTROPY});
    _matchers->push_back(new VirtioDeviceMatcher<VirtioGraphic>{"VirtI/O Graphic Device", VIRTIO_DEVICE_GRAPHICS});
    _matchers->push_back(new VirtioDeviceMatcher<VirtioNetwork>{"VirtI/O Network Device", VIRTIO_DEVICE_NETWORK});

    _matchers->push_back(new LegacyDeviceMatcher<LegacyKeyboard>{"Legacy Keyboard", LEGACY_KEYBOARD});
    _matchers->push_back(new LegacyDeviceMatcher<LegacyMouse>{"Legacy Mouse", LEGACY_MOUSE});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM1)", LEGACY_COM1});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM2)", LEGACY_COM2});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM3)", LEGACY_COM3});
    _matchers->push_back(new LegacyDeviceMatcher<LegacySerial>{"Legacy Serial Port (COM4)", LEGACY_COM4});
    _matchers->push_back(new LegacyDeviceMatcher<PCSpeaker>{"PCSpeaker", LEGACY_SPEAKER});

    _matchers->push_back(new LegacyDeviceMatcher<LegacyATA>{"ATA0 Disk", LEGACY_ATA0});
    _matchers->push_back(new LegacyDeviceMatcher<LegacyATA>{"ATA1 Disk", LEGACY_ATA1});
    _matchers->push_back(new LegacyDeviceMatcher<LegacyATA>{"ATA2 Disk", LEGACY_ATA2});
    _matchers->push_back(new LegacyDeviceMatcher<LegacyATA>{"ATA3 Disk", LEGACY_ATA3});

    _matchers->push_back(new UNIXDeviceMatcher<UnixNull>{"Unix Null Device", UNIX_NULL});
    _matchers->push_back(new UNIXDeviceMatcher<UnixRandom>{"Unix Random Device", UNIX_RANDOM});
    _matchers->push_back(new UNIXDeviceMatcher<UnixZero>{"Unix Zero Device", UNIX_ZERO});

    for (size_t i = 0; i < _matchers->count(); i++)
    {
        logger_info("Driver: %s", (*_matchers)[i]->name());
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
