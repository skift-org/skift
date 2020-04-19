#include <libsystem/assert.h>

#include "file-explorer/FileSystemModel.h"

typedef enum
{
    COLUMN_ICON,
    COLUMN_NAME,
    COLUMN_TYPE,
    COLUMN_SIZE,

    __COLUMN_COUNT,
} Column;

static int filesystem_column_count(void)
{
    return __COLUMN_COUNT;
}

static const char *filesystem_column_name(Column column)
{
    switch (column)
    {
    case COLUMN_ICON:
        return "Icon";
    case COLUMN_NAME:
        return "Name";
    case COLUMN_TYPE:
        return "Type";
    case COLUMN_SIZE:
        return "Size";

    default:
        ASSERT_NOT_REACHED();
    }
}

static int filesystem_get_lenght(void *data)
{
    __unused(data);

    return 10;
}

static Variant filesystem_get_data(void *data, int row, Column column)
{
    __unused(data);
    __unused(row);
    __unused(column);

    return vint(424242);
}

static CellStyle filesystem_get_style(void *data, int row, Column column)
{
    __unused(data);
    __unused(row);
    __unused(column);

    return DEFAULT_STYLE;
}

Model filesystem_model_create(void)
{
    Model model = {};

    model.column_count = (ModelColumnCount)filesystem_column_count;
    model.column_name = (ModelColumnName)filesystem_column_name;
    model.get_lenght = (ModelGetLenght)filesystem_get_lenght;
    model.get_data = (ModelGetData)filesystem_get_data;
    model.get_style = (ModelGetStyle)filesystem_get_style;

    return model;
}
