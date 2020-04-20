#include <libgraphic/Painter.h>
#include <libwidget/Table.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

#define TABLE_ROW_HEIGHT 24

Rectangle table_cell_bound(Table *widget, int row, int column)
{
    int column_count = model_column_count(widget->model);
    int column_width = widget_content_bound(widget).width / column_count;

    return (Rectangle){{
        widget_content_bound(widget).X + column * column_width,
        widget_content_bound(widget).Y + row * TABLE_ROW_HEIGHT + TABLE_ROW_HEIGHT,
        column_width,
        TABLE_ROW_HEIGHT,
    }};
}

void table_render_cell(Table *widget, Painter *painter, int row, int column)
{
    Rectangle cell_bound = table_cell_bound(widget, row, column);
    painter_draw_string(
        painter,
        widget_font(),
        model_data(widget->model, row, column).as_string,
        (Point){cell_bound.X + 4, cell_bound.Y + 16},
        widget_get_color(widget, THEME_FOREGROUND));
}

void table_paint(Table *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    int column_count = model_column_count(widget->model);
    int column_width = widget_content_bound(widget).width / column_count;

    for (int column = 0; column < column_count; column++)
    {
        Rectangle header_bound = (Rectangle){{
            widget_content_bound(widget).X + column * column_width,
            widget_content_bound(widget).Y,
            column_width,
            TABLE_ROW_HEIGHT,
        }};

        painter_fill_rectangle(painter, header_bound, widget_get_color(widget, THEME_BACKGROUND));
        painter_fill_rectangle(painter, rectangle_right(header_bound, 1), widget_get_color(widget, THEME_BORDER));
        painter_fill_rectangle(painter, rectangle_bottom(header_bound, 1), widget_get_color(widget, THEME_BORDER));

        painter_draw_string(painter, widget_font(), model_column_name(widget->model, column), (Point){header_bound.X + 4, header_bound.Y + 16}, widget_get_color(widget, THEME_FOREGROUND));
        painter_draw_string(painter, widget_font(), model_column_name(widget->model, column), (Point){header_bound.X + 4 + 1, header_bound.Y + 16}, widget_get_color(widget, THEME_FOREGROUND));
    }

    for (int row = 0; row < MIN(model_row_count(widget->model), (widget_content_bound(widget).height / TABLE_ROW_HEIGHT) - 1); row++)
    {
        Rectangle row_bound = (Rectangle){{
            widget_content_bound(widget).X,
            widget_content_bound(widget).Y + row * TABLE_ROW_HEIGHT + TABLE_ROW_HEIGHT,
            widget_content_bound(widget).width,
            TABLE_ROW_HEIGHT,
        }};

        if (widget->selected == row)
        {
            painter_fill_rectangle(painter, row_bound, widget_get_color(widget, THEME_SELECTED));
            painter_draw_rectangle(painter, row_bound, widget_get_color(widget, THEME_SELECTED));
        }

        for (int column = 0; column < column_count; column++)
        {
            table_render_cell(widget, painter, row, column);
        }
    }

    painter_draw_rectangle(painter, widget_content_bound(widget), widget_get_color(widget, THEME_BORDER));
}

Widget *table_create(Widget *parent, Model *model)
{
    Table *table = __create(Table);

    WIDGET(table)->paint = (WidgetPaintCallback)table_paint;
    table->model = model;
    table->selected = 3;

    widget_initialize(WIDGET(table), "Table", parent);

    return WIDGET(table);
}
