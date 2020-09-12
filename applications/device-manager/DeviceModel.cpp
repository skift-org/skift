#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>

#include "device-manager/DeviceModel.h"

enum Column
{
    COLUMN_NAME,
    COLUMN_DESCRIPTION,
    COLUMN_PATH,
    COLUMN_ADDRESS,

    __COLUMN_COUNT,
};

static void device_model_update(DeviceModel *model)
{
    if (model->data)
    {
        json::destroy(model->data);
    }

    model->data = json::parse_file("/System/devices");
}

static Variant device_model_data(DeviceModel *model, int row, int column)
{
    auto device = json::array_get(model->data, (size_t)row);

    switch (column)
    {
    case COLUMN_NAME:
        return Variant(json::string_value(json::object_get(device, "name")));

    case COLUMN_DESCRIPTION:
        return Variant(json::string_value(json::object_get(device, "description")));

    case COLUMN_PATH:
        return Variant(json::string_value(json::object_get(device, "path")));

    case COLUMN_ADDRESS:
        return Variant(json::string_value(json::object_get(device, "address")));

    default:
        ASSERT_NOT_REACHED();
    }
}

static int device_model_column_count()
{
    return __COLUMN_COUNT;
}

static int device_model_row_count(DeviceModel *model)
{
    return json::array_length(model->data);
}

static const char *device_model_column_name(int column)
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

static void device_model_destroy(DeviceModel *model)
{
    json::destroy(model->data);
}

DeviceModel *device_model_create()
{
    DeviceModel *model = __create(DeviceModel);

    model->model_update = (ModelUpdateCallback)device_model_update;
    model->model_data = (ModelDataCallback)device_model_data;
    model->model_row_count = (ModelRowCountCallback)device_model_row_count;
    model->model_column_count = (ModelColumnCountCallback)device_model_column_count;
    model->model_column_name = (ModelColumnNameCallback)device_model_column_name;
    model->model_destroy = (ModelDestroyCallback)device_model_destroy;

    model_initialize((Model *)model);

    return model;
}
