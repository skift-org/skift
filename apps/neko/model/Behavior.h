#pragma once

#include "neko/graphics/Animation.h"

namespace neko
{

class Neko;

class Behavior
{
public:
    virtual const char *name() { return "undefined"; }

    virtual ~Behavior(){};

    virtual void update(Neko &) = 0;

    virtual Animation animation(Neko &neko) = 0;
};

} // namespace neko
