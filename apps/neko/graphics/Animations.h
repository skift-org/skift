#include "neko/graphics/Animation.h"

namespace neko
{

namespace Animations
{
static constexpr Animation STOP = {Sprite::MATI1};
static constexpr Animation JARE = {Sprite::JARE, Sprite::MATI1};
static constexpr Animation KAKI = {Sprite::KAKI1, Sprite::KAKI2};
static constexpr Animation AKUBI = {Sprite::MATI2, Sprite::MATI2};
static constexpr Animation SLEEP = {Sprite::SLEEP1, Sprite::SLEEP2, 3};
static constexpr Animation AWAKE = {Sprite::AWAKE, Sprite::AWAKE};
static constexpr Animation MOVE_UP = {Sprite::UP1, Sprite::UP2};
static constexpr Animation MOVE_DOWN = {Sprite::DOWN1, Sprite::DOWN2};
static constexpr Animation MOVE_LEFT = {Sprite::LEFT1, Sprite::LEFT2};
static constexpr Animation MOVE_RIGHT = {Sprite::RIGHT1, Sprite::RIGHT2};
static constexpr Animation MOVE_UP_LEFT = {Sprite::UPLEFT1, Sprite::UPLEFT2};
static constexpr Animation MOVE_UP_RIGHT = {Sprite::UPRIGHT1, Sprite::UPRIGHT2};
static constexpr Animation MOVE_DOWN_LEFT = {Sprite::DWLEFT1, Sprite::DWLEFT2};
static constexpr Animation MOVE_DOWN_RIGHT = {Sprite::DWRIGHT1, Sprite::DWRIGHT2};
static constexpr Animation TOGI_UP = {Sprite::UTOGI1, Sprite::UTOGI2};
static constexpr Animation TOGI_DOWN = {Sprite::DTOGI1, Sprite::DTOGI2};
static constexpr Animation TOGI_LEFT = {Sprite::LTOGI1, Sprite::LTOGI2};
static constexpr Animation TOGI_RIGHT = {Sprite::RTOGI1, Sprite::RTOGI2};

Animation from_vector(Math::Vec2f vec);

}; // namespace Animations

} // namespace neko
