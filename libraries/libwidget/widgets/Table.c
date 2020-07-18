#include <libgraphic/Painter.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/ScrollBar.h>
#include <libwidget/widgets/Table.h>

#define TABLE_ROW_HEIGHT 24

Rectangle table_bound(Table *widget)
{
    return rectangle_shrink(widget_get_content_bound(widget), INSETS(0, 0, 0, 16));
}

Rectangle table_scrollbar_bound(Table *widget)
{
    return rectangle_right(widget_get_content_bound(widget), 16);
}

Rectangle table_header_bound(Table *widget)
{
    return rectangle_top(table_bound(widget), TABLE_ROW_HEIGHT);
}

Rectangle table_body_bound(Table *widget)
{
    return rectangle_shrink(table_bound(widget), INSETS(TABLE_ROW_HEIGHT, 0, 0));
}

Rectangle table_row_bound(Table *widget, int row)
{
    return (Rectangle){{
        table_body_bound(widget).x,
        table_body_bound(widget).y + row * TABLE_ROW_HEIGHT - widget->scroll_offset,
        table_body_bound(widget).width,
        TABLE_ROW_HEIGHT,
    }};
}

Rectangle table_cell_bound(Table *widget, int row, int column)
{
    int column_count = model_column_count(widget->model);
    int column_width = table_body_bound(widget).width / column_count;
    Rectangle row_bound = table_row_bound(widget, row);

    return (Rectangle){{
        row_bound.x + column * column_width,
        row_bound.y,
        column_width,
        TABLE_ROW_HEIGHT,
    }};
}

void table_render_cell(Table *widget, Painter *painter, int row, int column)
{
    Rectangle cell_bound = table_cell_bound(widget, row, column);
    Variant data = model_data(widget->model, row, column);

    painter_push_clip(painter, cell_bound);

    if (row % 2 == 0)
    {
        painter_fill_rectangle(painter, cell_bound, ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.05));
    }

    if (data.icon)
    {
        painter_blit_icon(
            painter,
            data.icon,
            ICON_18PX,
            (Rectangle){{cell_bound.x + 4, cell_bound.y + 3, 18, 18}},
            widget_get_color(widget, THEME_FOREGROUND));

        painter_draw_string(
            painter,
            widget_font(),
            data.as_string,
            vec2i(cell_bound.x + 4 + 18 + 4, cell_bound.y + 16),
            widget_get_color(widget, THEME_FOREGROUND));
    }
    else
    {
        painter_draw_string(
            painter,
            widget_font(),
            data.as_string,
            vec2i(cell_bound.x + 4, cell_bound.y + 16),
            widget_get_color(widget, THEME_FOREGROUND));
    }

    painter_pop_clip(painter);
}

int table_row_at(Table *widget, Vec2i position)
{
    position = vec2i_sub(position, table_body_bound(widget).position);

    int row = (position.y + widget->scroll_offset) / TABLE_ROW_HEIGHT;

    if (row < 0 || row >= model_row_count(widget->model))
    {
        row = -1;
    }

    return row;
}

void table_paint(Table *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter_push_clip(painter, widget_get_bound(widget));

    int column_count = model_column_count(widget->model);
    int column_width = table_bound(widget).width / column_count;

    for (int row = MAX(0, widget->scroll_offset / TABLE_ROW_HEIGHT - 1);
         row < MIN(model_row_count(widget->model), ((widget->scroll_offset + table_body_bound(widget).height) / TABLE_ROW_HEIGHT) + 1);
         row++)
    {
        Rectangle row_bound = table_row_bound(widget, row);

        if (widget->selected == row)
        {
            painter_fill_rectangle(painter, row_bound, widget_get_color(widget, THEME_SELECTION));
            painter_draw_rectangle(painter, row_bound, widget_get_color(widget, THEME_SELECTION));
        }

        for (int column = 0; column < column_count; column++)
        {
            table_render_cell(widget, painter, row, column);
        }
    }

    painter_blur_rectangle(painter, table_header_bound(widget), 16);
    painter_fill_rectangle(painter, table_header_bound(widget), ALPHA(widget_get_color(widget, THEME_BACKGROUND), 0.5));

    for (int column = 0; column < column_count; column++)
    {
        Rectangle header_bound = (Rectangle){{
            table_header_bound(widget).x + column * column_width,
            table_header_bound(widget).y,
            column_width,
            TABLE_ROW_HEIGHT,
        }};

        if (column < column_count - 1)
        {
            painter_fill_rectangle(painter, rectangle_right(header_bound, 1), widget_get_color(widget, THEME_BORDER));
        }
        painter_fill_rectangle(painter, rectangle_bottom(header_bound, 1), widget_get_color(widget, THEME_BORDER));

        painter_draw_string(painter, widget_font(), model_column_name(widget->model, column), vec2i(header_bound.x + 4, header_bound.y + 16), widget_get_color(widget, THEME_FOREGROUND));
        painter_draw_string(painter, widget_font(), model_column_name(widget->model, column), vec2i(header_bound.x + 4 + 1, header_bound.y + 16), widget_get_color(widget, THEME_FOREGROUND));
    }

    painter_pop_clip(painter);
}

void table_event(Table *widget, Event *event)
{
    if (event->type == EVENT_MOUSE_BUTTON_PRESS)
    {
        widget->selected = table_row_at(widget, event->mouse.position);
        widget_update(WIDGET(widget));
    }
}

void table_layout(Table *widget)
{
    widget->scrollbar->bound = table_scrollbar_bound(widget);

    ((ScrollBar *)widget->scrollbar)->track = TABLE_ROW_HEIGHT * model_row_count(widget->model);
    ((ScrollBar *)widget->scrollbar)->thumb = table_body_bound(widget).height;
    ((ScrollBar *)widget->scrollbar)->value = 0;
}

void table_on_scrollbar_scroll(Table *table, ScrollBar *sender, Event *event)
{
    __unused(event);

    table->scroll_offset = sender->value;
    widget_update(WIDGET(table));
}

static const WidgetClass table_class = {
    .name = "Table",

    .paint = (WidgetPaintCallback)table_paint,
    .event = (WidgetEventCallback)table_event,
    .layout = (WidgetLayoutCallback)table_layout,
};

Widget *table_create(Widget *parent, Model *model)
{
    Table *table = __create(Table);

    table->model = model;
    table->selected = -1;
    table->scroll_offset = 0;

    widget_initialize(WIDGET(table), &table_class, parent);

    table->scrollbar = scrollbar_create(WIDGET(table));
    widget_set_event_handler(table->scrollbar, EVENT_VALUE_CHANGE, EVENT_HANDLER(table, table_on_scrollbar_scroll));

    return WIDGET(table);
}
