#pragma once

#include <libgraphic/Color.h>
#include <libwidget/Variant.h>

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

typedef int (*ModelColumnCount)(void);
typedef const char *(*ModelColumnName)(int column);

typedef Variant (*ModelGetData)(void *data, int row, int column);
typedef int (*ModelGetLenght)(void *data);
typedef CellStyle (*ModelGetStyle)(void *data, int row, int column);

typedef struct Model
{
    ModelColumnCount column_count;
    ModelColumnName column_name;

    ModelGetData get_data;
    ModelGetLenght get_lenght;
    ModelGetStyle get_style;
} Model;
