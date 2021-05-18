#include <libgraphic/Painter.h>

#include "snake/widgets/Board.h"

using namespace Widget;
using namespace Graphic;

namespace Snake
{

BoardView::BoardView()
{
    board = make<Board>(15, 15);

    _timer = own<Async::Timer>(300, [this] {
        board->update();
        should_repaint();
    });

    _timer->start();
}

void BoardView::paint(Painter &painter, const Math::Recti &)
{
    auto cell = [](Math::Vec2i p) {
        return Math::Recti{p * CELL_SIZE, {CELL_SIZE}};
    };

    for (int y = 0; y < board->height(); y++)
    {
        for (int x = 0; x < board->width(); x++)
        {
            painter.fill_rectangle_rounded(cell({x, y}).shrinked({2}), 4, color(THEME_MIDDLEGROUND));
        }
    }

    auto fruit_color = color(THEME_ANSI_BRIGHT_RED);

    for (auto fruit : board->fruits())
    {
        painter.fill_rectangle_rounded(cell(fruit.position).expended(2), 4, fruit_color);
        painter.fill_rectangle_rounded(cell(fruit.position).expended(8), 8, fruit_color.with_alpha(0.5));
    }

    auto &snake = board->snake();

    auto snake_color = color(THEME_ANSI_BRIGHT_GREEN);

    for (auto tail : snake.tail())
    {
        painter.fill_rectangle_rounded(cell(tail).expended(2), 4, snake_color);
    }

    painter.fill_rectangle_rounded(cell(snake.head()).expended(2), 4, snake_color);
    painter.fill_rectangle_rounded(cell(snake.head()).expended(8), 8, snake_color.with_alpha(0.5));
}

void BoardView::event(Event *event)
{
    if (event->type == Event::KEYBOARD_KEY_PRESS)
    {
        auto &keyboard_event = event->keyboard;

        if (keyboard_event.key == KEYBOARD_KEY_UP)
        {
            board->snake().face_to(Facing::UP);
        }

        if (keyboard_event.key == KEYBOARD_KEY_DOWN)
        {
            board->snake().face_to(Facing::DOWN);
        }

        if (keyboard_event.key == KEYBOARD_KEY_LEFT)
        {
            board->snake().face_to(Facing::LEFT);
        }

        if (keyboard_event.key == KEYBOARD_KEY_RIGHT)
        {
            board->snake().face_to(Facing::RIGHT);
        }

        event->accepted = true;
    }
}

Math::Vec2i BoardView::size()
{
    return {
        board->width() * CELL_SIZE,
        board->height() * CELL_SIZE,
    };
}

} // namespace Snake
