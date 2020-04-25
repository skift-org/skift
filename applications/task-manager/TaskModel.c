#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Directory.h>
#include <libsystem/logger.h>

#include "task-manager/TaskModel.h"

typedef enum
{
    COLUMN_ID,
    COLUMN_NAME,
    COLUMN_CPU,

    __COLUMN_COUNT,
} Column;

static void task_model_update(TaskModel *model)
{
    if (model->data)
    {
        json_destroy(model->data);
    }

    model->data = json_parse_file("/sys/processes");
}

static Variant task_model_data(TaskModel *model, int row, int column)
{
    JsonValue *task = json_array_get(model->data, (size_t)row);

    switch (column)
    {
    case COLUMN_ID:
        return vint(json_integer_value(json_object_get(task, "id")));
    case COLUMN_NAME:
        return vstring(json_string_value(json_object_get(task, "name")));
    case COLUMN_CPU:
        return vstringf("%2d%%", json_integer_value(json_object_get(task, "cpu")));

    default:
        ASSERT_NOT_REACHED();
    }
}

static int task_model_column_count(void)
{
    return __COLUMN_COUNT;
}

static int task_model_row_count(TaskModel *model)
{
    return json_array_lenght(model->data);
}

static const char *task_model_column_name(int column)
{
    switch (column)
    {
    case COLUMN_ID:
        return "ID";
    case COLUMN_NAME:
        return "Name";
    case COLUMN_CPU:
        return "CPU%";

    default:
        ASSERT_NOT_REACHED();
    }
}

static void task_model_destroy(TaskModel *model)
{
    json_destroy(model->data);
}

TaskModel *task_model_create(void)
{
    TaskModel *model = __create(TaskModel);

    model->model_update = (ModelUpdateCallback)task_model_update;
    model->model_data = (ModelDataCallback)task_model_data;
    model->model_row_count = (ModelRowCountCallback)task_model_row_count;
    model->model_column_count = (ModelColumnCountCallback)task_model_column_count;
    model->model_column_name = (ModelColumnNameCallback)task_model_column_name;
    model->model_destroy = (ModelDestroyCallback)task_model_destroy;

    model_initialize((Model *)model);

    return model;
}
