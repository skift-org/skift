#pragma once

#include <libjson/Json.h>
#include <libsystem/io/Path.h>
#include <libsystem/utils/List.h>
#include <libwidget/Model.h>

struct TaskModel
{
    Model __super;

    JsonValue *data;
};

TaskModel *task_model_create();

const char *task_model_get_greedy_process(TaskModel *model, int ram_cpu);
