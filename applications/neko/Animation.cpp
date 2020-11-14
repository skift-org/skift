#include <libsystem/Logger.h>
#include <libsystem/math/MinMax.h>

#include "neko/Animation.h"

namespace neko
{

static Animation direction_to_animation[3][3] = {
    {Animation::MOVE_UP_LEFT, Animation::MOVE_UP, Animation::MOVE_UP_RIGHT},
    {Animation::MOVE_LEFT, Animation::STOP, Animation::MOVE_RIGHT},
    {Animation::MOVE_DOWN_LEFT, Animation::MOVE_DOWN, Animation::MOVE_DOWN_RIGHT},
};

Animation vector_to_move_animation(Vec2f vec)
{
    auto normalized = vec.normalized();

    int x = normalized.x() * 2;
    int y = normalized.y() * 2;

    return direction_to_animation[clamp(y + 1, 0, 2)][clamp(x + 1, 0, 2)];
}

} // namespace neko
