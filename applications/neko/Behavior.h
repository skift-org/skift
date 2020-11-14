#pragma once

#include "neko/Animation.h"

namespace neko
{

class Neko;

class Behavior
{
public:
    virtual ~Behavior(){};

    virtual void update(Neko &) = 0;

    virtual Animation animation(Neko &neko) = 0;
};

} // namespace neko
