#include <libsystem/assert.h>
#include <libwidget/Model.h>

void model_initialize(Model *model)
{
    model_update(model);
}

void model_update(Model *model)
{
    if (model->model_update)
    {
        model->model_update(model);
    }
}

Variant model_data(Model *model, int row, int column)
{
    assert(row >= 0 && row < model_row_count(model));
    assert(column >= 0 && column < model_column_count(model));

    if (model->model_data)
    {
        return model->model_data(model, row, column);
    }

    ASSERT_NOT_REACHED();
}

CellStyle model_style(Model *model, int row, int column)
{
    assert(row >= 0 && row < model_row_count(model));
    assert(column >= 0 && column < model_column_count(model));

    if (model->model_style)
    {
        return model->model_style(model, row, column);
    }

    return DEFAULT_STYLE;
}

void model_destroy(Model *model)
{
    if (model->model_destroy)
    {
        model->model_destroy(model);
    }

    free(model);
}

int model_row_count(Model *model)
{
    if (model->model_row_count)
    {
        return model->model_row_count(model);
    }

    return 0;
}

int model_column_count(Model *model)
{
    if (model->model_column_count)
    {
        return model->model_column_count();
    }

    return 0;
}

const char *model_column_name(Model *model, int column)
{
    assert(column >= 0 && column < model_column_count(model));

    if (model->model_column_name)
    {
        return model->model_column_name(column);
    }

    return "<null>";
}
