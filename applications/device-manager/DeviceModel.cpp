#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>

#include "device-manager/DeviceModel.h"

typedef enum
{
    COLUMN_DEVICE,
    COLUMN_DESCRIPTION,

    __COLUMN_COUNT,
} Column;

static void device_model_update(DeviceModel *model)
{
    if (model->data)
    {
        json_destroy(model->data);
    }

    model->data = json_parse_file("/System/devices");
}

static Variant device_model_data(DeviceModel *model, int row, int column)
{
    JsonValue *device = json_array_get(model->data, (size_t)row);

    switch (column)
    {
    case COLUMN_DEVICE:
        return Variant(json_string_value(json_object_get(device, "device")));

    case COLUMN_DESCRIPTION:
        return Variant(json_string_value(json_object_get(device, "description")));

    default:
        ASSERT_NOT_REACHED();
    }
}

static int device_model_column_count(void)
{
    return __COLUMN_COUNT;
}

static int device_model_row_count(DeviceModel *model)
{
    return json_array_length(model->data);
}

static const char *device_model_column_name(int column)
{
    switch (column)
    {
    case COLUMN_DEVICE:
        return "Device";

    case COLUMN_DESCRIPTION:
        return "Description";

    default:
        ASSERT_NOT_REACHED();
    }
}

static void device_model_destroy(DeviceModel *model)
{
    json_destroy(model->data);
}

DeviceModel *device_model_create(void)
{
    DeviceModel *model = __create(DeviceModel);

    MODEL(model)->model_update = (ModelUpdateCallback)device_model_update;
    MODEL(model)->model_data = (ModelDataCallback)device_model_data;
    MODEL(model)->model_row_count = (ModelRowCountCallback)device_model_row_count;
    MODEL(model)->model_column_count = (ModelColumnCountCallback)device_model_column_count;
    MODEL(model)->model_column_name = (ModelColumnNameCallback)device_model_column_name;
    MODEL(model)->model_destroy = (ModelDestroyCallback)device_model_destroy;

    model_initialize((Model *)model);

    return model;
}
