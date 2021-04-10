#include "neko/graphics/Animations.h"

namespace neko::Animations
{

static Animation direction_to_animation[3][3] = {
    {Animations::MOVE_UP_LEFT, Animations::MOVE_UP, Animations::MOVE_UP_RIGHT},
    {Animations::MOVE_LEFT, Animations::STOP, Animations::MOVE_RIGHT},
    {Animations::MOVE_DOWN_LEFT, Animations::MOVE_DOWN, Animations::MOVE_DOWN_RIGHT},
};

Animation from_vector(Math::Vec2f vec)
{
    auto normalized = vec.normalized();

    int x = normalized.x() * 2;
    int y = normalized.y() * 2;

    return direction_to_animation[clamp(y + 1, 0, 2)][clamp(x + 1, 0, 2)];
}

} // namespace neko::Animations
