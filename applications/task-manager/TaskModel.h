#pragma once

#include <libjson/Json.h>
#include <libsystem/io/Path.h>
#include <libsystem/utils/List.h>
#include <libwidget/Model.h>

typedef struct
{
    Model __super;

    JsonValue *data;
} TaskModel;

TaskModel *task_model_create(void);

void task_model_navigate(TaskModel *model, Path *path);

const char *task_model_filename_by_index(TaskModel *model, int index);
