#include <libgraphic/Painter.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/ScrollBar.h>
#include <libwidget/widgets/Table.h>

#define TABLE_ROW_HEIGHT 32

Rectangle table_bound(Table *widget)
{
    return widget_get_content_bound(widget).shrinked(Insets(0, 0, 0, 16));
}

Rectangle table_scrollbar_bound(Table *widget)
{
    return widget_get_content_bound(widget).take_right(16);
}

Rectangle table_header_bound(Table *widget)
{
    return table_bound(widget).take_top(TABLE_ROW_HEIGHT);
}

Rectangle table_body_bound(Table *widget)
{
    return table_bound(widget).shrinked(Insets(TABLE_ROW_HEIGHT, 0, 0));
}

Rectangle table_row_bound(Table *widget, int row)
{
    return Rectangle(
        table_body_bound(widget).x(),
        table_body_bound(widget).y() + row * TABLE_ROW_HEIGHT - widget->scroll_offset,
        table_body_bound(widget).width(),
        TABLE_ROW_HEIGHT);
}

Rectangle table_cell_bound(Table *widget, int row, int column)
{
    int column_count = model_column_count(widget->model);
    int column_width = table_body_bound(widget).width() / column_count;
    Rectangle row_bound = table_row_bound(widget, row);

    return Rectangle(
        row_bound.x() + column * column_width,
        row_bound.y(),
        column_width,
        TABLE_ROW_HEIGHT);
}

void table_render_cell(Table *widget, Painter &painter, int row, int column)
{
    Rectangle cell_bound = table_cell_bound(widget, row, column);
    Variant data = model_data(widget->model, row, column);

    painter.push();
    painter.clip(cell_bound);

    if (row % 2 == 0)
    {
        painter.fill_rectangle(cell_bound, ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.05));
    }

    if (data.has_icon())
    {
        painter.blit_icon(
            *data.icon(),
            ICON_18PX,
            Rectangle(cell_bound.x() + 7, cell_bound.y() + 7, 18, 18),
            widget_get_color(widget, THEME_FOREGROUND));

        painter.draw_string(
            *widget_font(),
            data.as_string(),
            Vec2i(cell_bound.x() + 7 + 18 + 7, cell_bound.y() + 20),
            widget_get_color(widget, THEME_FOREGROUND));
    }
    else
    {
        painter.draw_string(
            *widget_font(),
            data.as_string(),
            Vec2i(cell_bound.x() + 7, cell_bound.y() + 20),
            widget_get_color(widget, THEME_FOREGROUND));
    }

    painter.pop();
}

int table_row_at(Table *widget, Vec2i position)
{
    position = position - table_body_bound(widget).position();

    int row = (position.y() + widget->scroll_offset) / TABLE_ROW_HEIGHT;

    if (row < 0 || row >= model_row_count(widget->model))
    {
        row = -1;
    }

    return row;
}

void table_paint(Table *widget, Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.push();
    painter.clip(widget_get_bound(widget));

    int column_count = model_column_count(widget->model);
    int column_width = table_bound(widget).width() / column_count;

    for (int row = MAX(0, widget->scroll_offset / TABLE_ROW_HEIGHT - 1);
         row < MIN(model_row_count(widget->model), ((widget->scroll_offset + table_body_bound(widget).height()) / TABLE_ROW_HEIGHT) + 1);
         row++)
    {
        Rectangle row_bound = table_row_bound(widget, row);

        if (widget->selected == row)
        {
            painter.fill_rectangle(row_bound, widget_get_color(widget, THEME_SELECTION));
            painter.draw_rectangle(row_bound, widget_get_color(widget, THEME_SELECTION));
        }

        for (int column = 0; column < column_count; column++)
        {
            table_render_cell(widget, painter, row, column);
        }
    }

    painter.blur_rectangle(table_header_bound(widget), 8);
    painter.fill_rectangle(table_header_bound(widget), ALPHA(widget_get_color(widget, THEME_BACKGROUND), 0.9));

    for (int column = 0; column < column_count; column++)
    {
        Rectangle header_bound = Rectangle(
            table_header_bound(widget).x() + column * column_width,
            table_header_bound(widget).y(),
            column_width,
            TABLE_ROW_HEIGHT);

        if (column < column_count - 1)
        {
            painter.fill_rectangle(header_bound.take_right(1), widget_get_color(widget, THEME_BORDER));
        }
        painter.fill_rectangle(header_bound.take_right(1), widget_get_color(widget, THEME_BORDER));

        painter.draw_string(*widget_font(), model_column_name(widget->model, column), Vec2i(header_bound.x() + 7, header_bound.y() + 20), widget_get_color(widget, THEME_FOREGROUND));
        painter.draw_string(*widget_font(), model_column_name(widget->model, column), Vec2i(header_bound.x() + 7 + 1, header_bound.y() + 20), widget_get_color(widget, THEME_FOREGROUND));
    }

    painter.pop();
}

void table_event(Table *widget, Event *event)
{
    if (event->type == Event::MOUSE_BUTTON_PRESS)
    {
        widget->selected = table_row_at(widget, event->mouse.position);
        widget->should_repaint();
    }
}

void table_layout(Table *widget)
{
    widget->scrollbar->bound = table_scrollbar_bound(widget);

    ((ScrollBar *)widget->scrollbar)->track = TABLE_ROW_HEIGHT * model_row_count(widget->model);
    ((ScrollBar *)widget->scrollbar)->thumb = table_body_bound(widget).height();
    ((ScrollBar *)widget->scrollbar)->value = 0;
}

static const WidgetClass table_class = {
    .name = "Table",

    .paint = (WidgetPaintCallback)table_paint,
    .event = (WidgetEventCallback)table_event,
    .layout = (WidgetLayoutCallback)table_layout,
};

Table *table_create(Widget *parent, Model *model)
{
    auto table = __create(Table);

    table->model = model;
    table->selected = -1;
    table->scroll_offset = 0;

    widget_initialize(table, &table_class, parent);

    table->scrollbar = scrollbar_create(table);

    table->scrollbar->on(Event::VALUE_CHANGE, [table](auto) {
        table->scroll_offset = table->scrollbar->value;
        table->should_repaint();
    });

    return table;
}
