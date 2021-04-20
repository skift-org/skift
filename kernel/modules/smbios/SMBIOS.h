#pragma once

#include <string.h>

#include "kernel/memory/MemoryRange.h"

namespace Smbios
{

#define SMBIOS_HEADER_TYPE_LIST(__ENTRY)                 \
    __ENTRY(BIOS_INFORMATION, 0)                         \
    __ENTRY(SYSTEM_INFORMATION, 1)                       \
    __ENTRY(BASEBOARD_OR_MODULE_INFORMATION, 2)          \
    __ENTRY(SYSTEM_ENCLOSURE_OR_CHASSIS, 3)              \
    __ENTRY(PROCESSOR_INFORMATION, 4)                    \
    __ENTRY(MEMORY_CONTROLLER_INFORMATION, 5)            \
    __ENTRY(MEMORY_MODULE_INFORMATION, 6)                \
    __ENTRY(CACHE_INFORMATION, 7)                        \
    __ENTRY(PORT_CONNECTOR_INFORMATION, 8)               \
    __ENTRY(SYSTEM_SLOTS, 9)                             \
    __ENTRY(ON_BOARD_DEVICES_INFORMATION, 10)            \
    __ENTRY(OEM_STRINGS, 11)                             \
    __ENTRY(SYSTEM_CONFIGURATION_OPTIONS, 12)            \
    __ENTRY(BIOS_LANGUAGE_INFORMATION, 13)               \
    __ENTRY(GROUP_ASSOCIATIONS, 14)                      \
    __ENTRY(SYSTEM_EVENT_LOG, 15)                        \
    __ENTRY(PHYSICAL_MEMORY_ARRAY, 16)                   \
    __ENTRY(MEMORY_DEVICE, 17)                           \
    __ENTRY(MEMORY_ERROR_INFORMATION_32_BIT, 18)         \
    __ENTRY(MEMORY_ARRAY_MAPPED_ADDRESS, 19)             \
    __ENTRY(MEMORY_DEVICE_MAPPED_ADDRESS, 20)            \
    __ENTRY(BUILT_IN_POINTING_DEVICE, 21)                \
    __ENTRY(PORTABLE_BATTERY, 22)                        \
    __ENTRY(SYSTEM_RESET, 23)                            \
    __ENTRY(HARDWARE_SECURITY, 24)                       \
    __ENTRY(SYSTEM_POWER_CONTROLS, 25)                   \
    __ENTRY(VOLTAGE_PROBE, 26)                           \
    __ENTRY(COOLING_DEVICE, 27)                          \
    __ENTRY(TEMPERATURE_PROBE, 28)                       \
    __ENTRY(ELECTRICAL_CURRENT_PROBE, 29)                \
    __ENTRY(OUT_OF_BAND_REMOTE_ACCESS, 30)               \
    __ENTRY(BOOT_INTEGRITY_SERVICES_BIS_ENTRY_POINT, 31) \
    __ENTRY(SYSTEM_BOOT_INFORMATION, 32)                 \
    __ENTRY(MEMORY_ERROR_INFORMATION_64_BIT, 33)         \
    __ENTRY(MANAGEMENT_DEVICE, 34)                       \
    __ENTRY(MANAGEMENT_DEVICE_COMPONENT, 35)             \
    __ENTRY(MANAGEMENT_DEVICE_THRESHOLD_DATA, 36)        \
    __ENTRY(MEMORY_CHANNEL, 37)                          \
    __ENTRY(IPMI_DEVICE_INFORMATION, 38)                 \
    __ENTRY(SYSTEM_POWER_SUPPLY, 39)                     \
    __ENTRY(ADDITIONAL_INFORMATION, 40)                  \
    __ENTRY(ONBOARD_DEVICES_EXTENDED_INFORMATION, 41)    \
    __ENTRY(MANAGEMENT_CONTROLLER_HOST_INTERFACE, 42)    \
    __ENTRY(TPM_DEVICE, 43)                              \
    __ENTRY(PROCESSOR_ADDITIONAL_INFORMATION, 44)        \
    __ENTRY(INACTIVE, 126)                               \
    __ENTRY(END_OF_TABLE, 127)

enum class HearderType : uint8_t
{

#define __ENTRY(__name, __id) \
    __name = __id,

    SMBIOS_HEADER_TYPE_LIST(__ENTRY)
#undef __ENTRY

};

struct PACKED Header
{
    HearderType type;
    uint8_t length;
    uint16_t handle;

    const char *name()
    {
        switch (static_cast<int>(type))
        {

#define __ENTRY(__name, __id) \
    case __id:                \
    {                         \
        return #__name;       \
    }

            SMBIOS_HEADER_TYPE_LIST(__ENTRY)
#undef __ENTRY

        default:
            return "unknow";
        }
    }

    const char *string_table() const
    {
        return reinterpret_cast<const char *>(this) + length;
    }

    size_t string_table_lenght() const
    {
        size_t size = 1;
        const char *str = string_table();

        while (str[0] != '\0')
        {
            size++;
            str += strlen(str) + 1;
        }

        return size;
    }

    const char *string(int index) const
    {
        if (index == 0)
        {
            return "";
        }

        const char *str = string_table();
        int current_index = 1;

        while (current_index != index)
        {
            str += strlen(str) + 1;
            current_index++;
        }

        return str;
    }

    size_t lenght_including_string_table() const
    {
        size_t size = length;

        for (size_t i = 0; string_table()[i] != '\0' || string_table()[i + 1] != '\0'; i++)
        {
            size++;
        }

        return size + 2; // We add two for terminators;
    }
};

struct PACKED BIOSInfo
{
    Header header;

    uint8_t _vendor;
    const char *vendor() const { return header.string(_vendor); }

    uint8_t _version;
    const char *version() const { return header.string(_version); }
};

struct PACKED SystemInfo
{
    Header header;

    uint8_t _manufaturer;
    const char *manufaturer() const { return header.string(_manufaturer); }

    uint8_t _product;
    const char *product() const { return header.string(_product); }

    uint8_t _version;
    const char *version() const { return header.string(_version); }

    uint8_t _serial_number;
    const char *serial_number() const { return header.string(_serial_number); }
};

struct PACKED MainboardInfo
{
    Header header;

    uint8_t _manufaturer;
    const char *manufaturer() const { return header.string(_manufaturer); }

    uint8_t _product;
    const char *product() const { return header.string(_product); }

    uint8_t _version;
    const char *version() const { return header.string(_version); }

    uint8_t _serial_number;
    const char *serial_number() const { return header.string(_serial_number); }
};

#define ENCLOSURE_TYPE_LIST(__ENTRY)     \
    __ENTRY(OTHER, 0x01)                 \
    __ENTRY(UNKNOWN, 0x02)               \
    __ENTRY(DESKTOP, 0x03)               \
    __ENTRY(LOW_PROFILE_DESKTOP, 0x04)   \
    __ENTRY(PIZZA_BOX, 0x05)             \
    __ENTRY(MINI_TOWER, 0x06)            \
    __ENTRY(TOWER, 0x07)                 \
    __ENTRY(PORTABLE, 0x08)              \
    __ENTRY(LAPTOP, 0x09)                \
    __ENTRY(NOTEBOOK, 0x0A)              \
    __ENTRY(HAND_HELD, 0x0B)             \
    __ENTRY(DOCKING_STATION, 0x0C)       \
    __ENTRY(ALL_IN_ONE, 0x0D)            \
    __ENTRY(SUB_NOTEBOOK, 0x0E)          \
    __ENTRY(SPACE_SAVING, 0x0F)          \
    __ENTRY(LUNCH_BOX, 0x10)             \
    __ENTRY(MAIN_SERVER_CHASSIS, 0x11)   \
    __ENTRY(EXPANSION_CHASSIS, 0x12)     \
    __ENTRY(SUB_CHASSIS, 0x13)           \
    __ENTRY(BUS_EXPANSION_CHASSIS, 0x14) \
    __ENTRY(PERIPHERAL_CHASSIS, 0x15)    \
    __ENTRY(RAID_CHASSIS, 0x16)          \
    __ENTRY(RACK_MOUNT_CHASSIS, 0x17)    \
    __ENTRY(SEALED_CASE_PC, 0x18)        \
    __ENTRY(MULTI_SYSTEM_CHASSIS, 0x19)  \
    __ENTRY(COMPACT_PCI, 0x1A)           \
    __ENTRY(ADVANCED_TCA, 0x1B)          \
    __ENTRY(BLADE, 0x1C)                 \
    __ENTRY(BLADE_ENCLOSURE, 0x1D)       \
    __ENTRY(TABLET, 0x1E)                \
    __ENTRY(CONVERTIBLE, 0x1F)           \
    __ENTRY(DETACHABLE, 0x20)            \
    __ENTRY(IOT_GATEWAY, 0x21)           \
    __ENTRY(EMBEDDED_PC, 0x22)           \
    __ENTRY(MINI_PC, 0x23)               \
    __ENTRY(STICK_PC, 0x24)

enum class EnclosureType : uint8_t
{
#define __ENTRY(__name, __id) \
    __name = __id,

    ENCLOSURE_TYPE_LIST(__ENTRY)
#undef __ENTRY
};

struct PACKED EnclosureInfo
{
    Header header;

    uint8_t has_lock : 1;
    EnclosureType type : 7;

    const char *type_string()
    {
        switch (static_cast<int>(type))
        {

#define __ENTRY(__name, __id) \
    case __id:                \
    {                         \
        return #__name;       \
    }

            ENCLOSURE_TYPE_LIST(__ENTRY)
#undef __ENTRY

        default:
            return "UNKNOW";
        }
    }

    uint8_t _manufaturer;
    const char *manufaturer() const { return header.string(_manufaturer); }

    uint8_t _product;
    const char *product() const { return header.string(_product); }

    uint8_t _version;
    const char *version() const { return header.string(_version); }

    uint8_t _serial_number;
    const char *serial_number() const { return header.string(_serial_number); }
};

struct PACKED ProcessorInfo
{
    Header header;
};

struct PACKED CacheInfo
{
    Header header;
};

struct PACKED SlotsInfo
{
    Header header;
};

struct PACKED PhysicalMemoryArray
{
    Header header;
};

struct PACKED MemoryDeviceInfo
{
    Header header;
};

struct PACKED MemoryArrayMappedAddress
{
    Header header;
};

struct PACKED MemoryDeviceMappedAddress
{
    Header header;
};

struct PACKED SystemBootInfo
{
    Header header;
};

struct EntryPoint
{
    char header[4];
    uint8_t checksum_;
    uint8_t length;
    uint8_t major_version;
    uint8_t minor_version;
    uint16_t max_structure_size;
    uint8_t entry_point_revision;
    char formatted_area[5];

    char header2[5];
    uint8_t checksum2;
    uint16_t length2;
    uint32_t table_address;
    uint16_t number_of_structures;
    uint8_t bcd_revision;

    uint8_t checksum()
    {
        uint8_t checksum = 0;

        for (size_t i = 0; i < this->length; i++)
        {
            checksum += (reinterpret_cast<uint8_t *>(this))[i];
        }

        return checksum;
    }

    template <typename IterationCallback>
    void iterate(IterationCallback callback)
    {
        Header *header = reinterpret_cast<Header *>(table_address);

        while (header->type != HearderType::END_OF_TABLE)
        {
            if (callback(header) == Iteration::STOP)
            {
                return;
            }

            header = reinterpret_cast<Header *>(reinterpret_cast<uintptr_t>(header) + header->lenght_including_string_table());
        }
    }
};

static inline EntryPoint *find(MemoryRange range)
{
    for (uintptr_t address = range.base(); address < range.end(); address += 16)
    {
        EntryPoint *entry_point = reinterpret_cast<EntryPoint *>(address);

        if (memcmp(entry_point->header, "_SM_", 4) == 0 &&
            entry_point->checksum() == 0)
        {
            return entry_point;
        }
    }

    return nullptr;
}

} // namespace Smbios
