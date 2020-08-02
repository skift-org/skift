#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>

#include "task-manager/TaskModel.h"

enum Column
{
    COLUMN_ID,
    COLUMN_NAME,
    COLUMN_STATE,
    COLUMN_CPU,
    COLUMN_DIRECTORY,

    __COLUMN_COUNT,
};

static void task_model_update(TaskModel *model)
{
    if (model->data)
    {
        json_destroy(model->data);
    }

    model->data = json_parse_file("/System/processes");
}

static Variant task_model_data(TaskModel *model, int row, int column)
{
    JsonValue *task = json_array_get(model->data, (size_t)row);

    switch (column)
    {
    case COLUMN_ID:
    {
        Variant value = Variant(json_integer_value(json_object_get(task, "id")));

        if (json_is(json_object_get(task, "user"), JSON_TRUE))
        {
            return value.with_icon(Icon::get("account"));
        }
        else
        {
            return value.with_icon(Icon::get("cog"));
        }
    }

    case COLUMN_NAME:
        return Variant(json_string_value(json_object_get(task, "name")));

    case COLUMN_STATE:
        return Variant(json_string_value(json_object_get(task, "state")));

    case COLUMN_CPU:
        return Variant("%2d%%", json_integer_value(json_object_get(task, "cpu")));

    case COLUMN_DIRECTORY:
        return Variant(json_string_value(json_object_get(task, "directory")));

    default:
        ASSERT_NOT_REACHED();
    }
}

static int task_model_column_count()
{
    return __COLUMN_COUNT;
}

static int task_model_row_count(TaskModel *model)
{
    return json_array_length(model->data);
}

static const char *task_model_column_name(int column)
{
    switch (column)
    {
    case COLUMN_ID:
        return "ID";

    case COLUMN_NAME:
        return "Name";

    case COLUMN_STATE:
        return "State";

    case COLUMN_CPU:
        return "CPU%";

    case COLUMN_DIRECTORY:
        return "Directory";

    default:
        ASSERT_NOT_REACHED();
    }
}

static void task_model_destroy(TaskModel *model)
{
    json_destroy(model->data);
}

TaskModel *task_model_create()
{
    TaskModel *model = __create(TaskModel);

    MODEL(model)->model_update = (ModelUpdateCallback)task_model_update;
    MODEL(model)->model_data = (ModelDataCallback)task_model_data;
    MODEL(model)->model_row_count = (ModelRowCountCallback)task_model_row_count;
    MODEL(model)->model_column_count = (ModelColumnCountCallback)task_model_column_count;
    MODEL(model)->model_column_name = (ModelColumnNameCallback)task_model_column_name;
    MODEL(model)->model_destroy = (ModelDestroyCallback)task_model_destroy;

    model_initialize((Model *)model);

    return model;
}
