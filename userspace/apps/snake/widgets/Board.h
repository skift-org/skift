#pragma once

#include <libasync/Timer.h>
#include <libwidget/Element.h>

#include "snake/model/Board.h"

namespace Snake
{

struct BoardView :
    public Widget::Element
{
    static constexpr auto CELL_SIZE = 32;

private:
    Ref<Board> board;
    Box<Async::Timer> _timer;

public:
    BoardView();

    void paint(Graphic::Painter &painter, const Math::Recti &) override;

    void event(Widget::Event *event) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(BoardView, board);

} // namespace Snake
