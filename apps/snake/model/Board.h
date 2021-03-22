#pragma once

#include <libutils/Random.h>
#include <libutils/Vector.h>

#include "snake/model/Fruit.h"
#include "snake/model/Snake.h"

namespace Snake
{

struct Board : public RefPtr<Board>, public RefCounted<Board>
{
private:
    int _width = 16;
    int _height = 16;

    Snake _snake;
    Vector<Fruit> _fruits;
    Random _random;

public:
    int width() { return _width; }
    int height() { return _height; }

    Snake &snake() { return _snake; }
    Vector<Fruit> &fruits() { return _fruits; }

    Board(int width, int height)
        : _width{width},
          _height{height},
          _snake{},
          _fruits{}
    {
        _snake.head = {_width / 2, _height / 2};
        for (int i = 0; i < 4; i++)
        {
            _snake.tail.push_back({_width / 2 + 1 + i, _height / 2});
        }

        _snake.facing = Facing::LEFT;

        for (size_t i = 0; i < 5; i++)
        {
            _fruits.push_back(Fruit{.position{_random.next_u8() % _width, _random.next_u8() % _height}});
        }
    }

    void update()
    {
        _snake.move();
    }
};

} // namespace Snake
