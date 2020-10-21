#include "device-manager/DeviceModel.h"

enum Column
{
    COLUMN_NAME,
    COLUMN_DESCRIPTION,
    COLUMN_PATH,
    COLUMN_ADDRESS,

    __COLUMN_COUNT,
};

DeviceModel::~DeviceModel()
{
    json::destroy(_data);
}

int DeviceModel::rows()
{
    return json::array_length(_data);
}

int DeviceModel::columns()
{
    return __COLUMN_COUNT;
}

String DeviceModel::header(int column)
{
    switch (column)
    {
    case COLUMN_NAME:
        return "Name";

    case COLUMN_DESCRIPTION:
        return "Description";

    case COLUMN_PATH:
        return "Path";

    case COLUMN_ADDRESS:
        return "Address";

    default:
        ASSERT_NOT_REACHED();
    }
}

Variant DeviceModel::data(int row, int column)
{
    auto device = json::array_get(_data, row);

    switch (column)
    {
    case COLUMN_NAME:
        return json::string_value(json::object_get(device, "name"));

    case COLUMN_DESCRIPTION:
        return json::string_value(json::object_get(device, "description"));

    case COLUMN_PATH:
        return json::string_value(json::object_get(device, "path"));

    case COLUMN_ADDRESS:
        return json::string_value(json::object_get(device, "address"));

    default:
        ASSERT_NOT_REACHED();
    }
}

void DeviceModel::update()
{
    if (_data)
    {
        json::destroy(_data);
    }

    _data = json::parse_file("/System/devices");

    did_update();
}
