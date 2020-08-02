#pragma once

#include <libjson/Json.h>
#include <libsystem/io/Path.h>
#include <libsystem/utils/List.h>
#include <libwidget/Model.h>

struct DeviceModel
{
    Model __super;

    JsonValue *data;
} ;

DeviceModel *device_model_create();

void device_model_navigate(DeviceModel *model, Path *path);

const char *device_model_filename_by_index(DeviceModel *model, int index);
