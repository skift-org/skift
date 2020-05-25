#pragma once

#include <libgraphic/Color.h>
#include <libwidget/utils/Variant.h>

struct Model;

typedef enum
{
    POSITION_TOP_LEFT,
    POSITION_TOP_CENTER,
    POSITION_TOP_RIGHT,

    POSITION_LEFT,
    POSITION_CENTER,
    POSITION_RIGHT,

    POSITION_BOTTOM_LEFT,
    POSITION_BOTTOM_CENTER,
    POSITION_BOTTOM_RIGHT,
} Position;

typedef struct
{
    Position text_position;

    Color foreground;
    Color background;
} CellStyle;

#define DEFAULT_STYLE ((CellStyle){POSITION_LEFT, COLOR_WHITE, COLOR_BLACK})

typedef void (*ModelUpdateCallback)(struct Model *model);
typedef Variant (*ModelDataCallback)(struct Model *model, int row, int column);
typedef CellStyle (*ModelStyleCallback)(struct Model *model, int row, int column);
typedef int (*ModelColumnCountCallback)(void);
typedef int (*ModelRowCountCallback)(struct Model *model);
typedef const char *(*ModelColumnNameCallback)(int column);
typedef void (*ModelDestroyCallback)(struct Model *model);

typedef struct Model
{
    ModelUpdateCallback model_update;
    ModelDataCallback model_data;
    ModelStyleCallback model_style;
    ModelRowCountCallback model_row_count;
    ModelColumnCountCallback model_column_count;
    ModelColumnNameCallback model_column_name;
    ModelDestroyCallback model_destroy;
} Model;

#define MODEL(__subclass) ((Model *)__subclass)

void model_initialize(Model *model);

void model_update(Model *model);

Variant model_data(Model *model, int row, int column);

CellStyle model_style(Model *model, int row, int column);

void model_destroy(Model *model);

int model_row_count(Model *model);

int model_column_count(Model *model);

const char *model_column_name(Model *model, int column);
