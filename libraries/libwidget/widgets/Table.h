#pragma once

#include <libutils/String.h>

#include <libwidget/Model.h>
#include <libwidget/Widget.h>
#include <libwidget/widgets/ScrollBar.h>

class Table : public Widget
{
private:
    static constexpr int TABLE_ROW_HEIGHT = 32;

    Model *_model;
    int _selected = -1;
    int _scroll_offset = 0;
    ScrollBar *_scrollbar;

    String _empty_message{"No data to display"};

    Rectangle body_bound() const;
    Rectangle scrollbar_bound() const;
    Rectangle header_bound() const;
    Rectangle list_bound() const;
    Rectangle row_bound(int row) const;
    Rectangle column_bound(int column) const;
    Rectangle cell_bound(int row, int column) const;
    int row_at(Vec2i position) const;
    void paint_cell(Painter &painter, int row, int column);

public:
    void empty_message(String message)
    {
        _empty_message = message;
    }

    int selected() { return _selected; }

    void select(int index)
    {
        if (index == _selected)
        {
            return;
        }

        if (index < 0 || index >= model_row_count(_model))
        {
            return;
        }

        _selected = index;
        should_repaint();
    }

    void scroll_to_top()
    {
        _scroll_offset = 0;

        should_repaint();
        should_relayout();
    }

    Table(Widget *parent, Model *model);

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void do_layout();
};
