#include <libio/File.h>

#include "device-manager/DeviceModel.h"

enum Column
{
    COLUMN_NAME,
    COLUMN_DESCRIPTION,
    COLUMN_PATH,
    COLUMN_ADDRESS,

    __COLUMN_COUNT,
};

int DeviceModel::rows()
{
    return _data.length();
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

Widget::Variant DeviceModel::data(int row, int column)
{
    auto &device = _data.get(row);

    switch (column)
    {
    case COLUMN_NAME:
        return device.get("name").as_string();

    case COLUMN_DESCRIPTION:
        return device.get("description").as_string();

    case COLUMN_PATH:
        return device.get("path").as_string();

    case COLUMN_ADDRESS:
        return device.get("address").as_string();

    default:
        ASSERT_NOT_REACHED();
    }
}

void DeviceModel::update()
{
    IO::File file{"/System/devices", HJ_OPEN_READ};

    if (!file.exist())
    {
        return;
    }

    _data = Json::parse(file);
    did_update();
}
