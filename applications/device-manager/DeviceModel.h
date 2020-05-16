#pragma once

#include <libjson/Json.h>
#include <libsystem/path.h>
#include <libsystem/utils/List.h>
#include <libwidget/Model.h>

typedef struct
{
    Model __super;

    JsonValue *data;
} DeviceModel;

DeviceModel *device_model_create(void);

void device_model_navigate(DeviceModel *model, Path *path);

const char *device_model_filename_by_index(DeviceModel *model, int index);
