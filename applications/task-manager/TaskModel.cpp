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

TaskModel::~TaskModel()
{
    json::destroy(_data);
}

int TaskModel::rows()
{
    return json::array_length(_data);
}

int TaskModel::columns()
{
    return __COLUMN_COUNT;
}

String TaskModel::header(int column)
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

Variant TaskModel::data(int row, int column)
{
    auto task = json::array_get(_data, (size_t)row);

    switch (column)
    {
    case COLUMN_ID:
    {
        Variant value = json::integer_value(json::object_get(task, "id"));

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
        return json::string_value(json::object_get(task, "name"));

    case COLUMN_STATE:
        return json::string_value(json::object_get(task, "state"));

    case COLUMN_CPU:
        return Variant("%2d%%", json::integer_value(json::object_get(task, "cpu")));

    case COLUMN_RAM:
        return Variant("%5d Kio", json::integer_value(json::object_get(task, "ram")) / 1024);

    case COLUMN_DIRECTORY:
        return json::string_value(json::object_get(task, "directory"));

    default:
        ASSERT_NOT_REACHED();
    }
}

void TaskModel::update()
{
    if (_data)
    {
        json::destroy(_data);
    }

    _data = json::parse_file("/System/processes");

    did_update();
}

static String greedy(json::Value *data, const char *field)
{
    size_t most_greedy_index = 0;
    int most_greedy_value = 0;

    for (size_t i = 0; i < json::array_length(data); i++)
    {
        auto task = json::array_get(data, i);

        auto value = json::integer_value(json::object_get(task, field));

        if (value > most_greedy_value)
        {
            most_greedy_index = i;
            most_greedy_value = value;
        }
    }

    auto task = json::array_get(data, most_greedy_index);
    return json::string_value(json::object_get(task, "name"));
}

String TaskModel::ram_greedy()
{
    return greedy(_data, "ram");
}

String TaskModel::cpu_greedy()
{
    return greedy(_data, "cpu");
}
