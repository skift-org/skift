module;

#include <hal/io.h>

export module Strata.Device:pci;

import :node;

namespace Strata::Device::Pci {

struct Id {
    u16 vendor;
    u16 device;
};

struct Addr {
    u16 seg;
    u8 bus;
    u8 slot;
    u8 func;
};

struct Device : public Node {
};

// MARK: Group, Bus, Slot, Func ------------------------------------------------

struct Group : public Device {
};

struct Bus : public Device {
};

struct Slot : public Device {
};

struct Func : public Device {
};

} // namespace Strata::Device::Pci
