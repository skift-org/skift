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
    COLUMN_RAM,
    COLUMN_DIRECTORY,

    __COLUMN_COUNT,
};

static void task_model_update(TaskModel *model)
{
    if (model->data)
    {
        json::destroy(model->data);
    }

    model->data = json::parse_file("/System/processes");
}

static Variant task_model_data(TaskModel *model, int row, int column)
{
    auto task = json::array_get(model->data, (size_t)row);

    switch (column)
    {
    case COLUMN_ID:
    {
        Variant value = Variant(json::integer_value(json::object_get(task, "id")));

        if (json::is(json::object_get(task, "user"), json::TRUE))
        {
            return value.with_icon(Icon::get("account"));
        }
        else
        {
            return value.with_icon(Icon::get("cog"));
        }
    }

    case COLUMN_NAME:
        return Variant(json::string_value(json::object_get(task, "name")));

    case COLUMN_STATE:
        return Variant(json::string_value(json::object_get(task, "state")));

    case COLUMN_CPU:
        return Variant("%2d%%", json::integer_value(json::object_get(task, "cpu")));

    case COLUMN_RAM:
        return Variant("%5d Kio", json::integer_value(json::object_get(task, "ram")) / 1024);

    case COLUMN_DIRECTORY:
        return Variant(json::string_value(json::object_get(task, "directory")));

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
    return json::array_length(model->data);
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

    case COLUMN_RAM:
        return "RAM(Kio)";

    case COLUMN_DIRECTORY:
        return "Directory";

    default:
        ASSERT_NOT_REACHED();
    }
}

static void task_model_destroy(TaskModel *model)
{
    json::destroy(model->data);
}

TaskModel *task_model_create()
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

const char *task_model_get_greedy_process(TaskModel *model, int ram_cpu)
{
    const char *greedy = "";

    int row_count = json::array_length(model->data);
    int list[row_count];

    for (int row = 0; row < row_count; row++)
    {
        // 0 means memory. 1 means processor
        if (ram_cpu == 0)
        {
            auto task = json::array_get(model->data, (size_t)row);
            list[row] = json::integer_value(json::object_get(task, "ram"));
        }
        else if (ram_cpu == 1)
        {
            auto task = json::array_get(model->data, (size_t)row);
            list[row] = json::integer_value(json::object_get(task, "cpu"));
        }
    }

    int greedy_index = 0;
    for (int i = 0; i < row_count; i++)
    {
        if (list[greedy_index] < list[i])
        {
            greedy_index = i;
        }
    }

    auto task = json::array_get(model->data, (size_t)greedy_index);
    greedy = json::string_value(json::object_get(task, "name"));

    return greedy;
}
