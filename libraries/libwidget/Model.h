#pragma once

#include <libgraphic/Color.h>
#include <libwidget/utils/Variant.h>

struct Model;

struct CellStyle
{
    Position text_position;

    Color foreground;
    Color background;
};

#define DEFAULT_STYLE ((CellStyle){Position::LEFT, Colors::WHITE, Colors::BLACK})

typedef void (*ModelUpdateCallback)(Model *model);
typedef Variant (*ModelDataCallback)(Model *model, int row, int column);
typedef CellStyle (*ModelStyleCallback)(Model *model, int row, int column);
typedef int (*ModelColumnCountCallback)();
typedef int (*ModelRowCountCallback)(Model *model);
typedef const char *(*ModelColumnNameCallback)(int column);
typedef void (*ModelDestroyCallback)(Model *model);

struct Model
{
    ModelUpdateCallback model_update;
    ModelDataCallback model_data;
    ModelStyleCallback model_style;
    ModelRowCountCallback model_row_count;
    ModelColumnCountCallback model_column_count;
    ModelColumnNameCallback model_column_name;
    ModelDestroyCallback model_destroy;
};

#define MODEL(__subclass) ((Model *)__subclass)

void model_initialize(Model *model);

void model_update(Model *model);

Variant model_data(Model *model, int row, int column);

CellStyle model_style(Model *model, int row, int column);

void model_destroy(Model *model);

int model_row_count(Model *model);

int model_column_count(Model *model);

const char *model_column_name(Model *model, int column);
