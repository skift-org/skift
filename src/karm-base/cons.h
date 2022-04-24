#pragma once


namespace Karm::Base
{

template <typename TCar, typename TCdr>
struct Cons
{
    using Car = TCar;
    using Cdr = TCdr;

    Car car;
    Cdr cdr;
};


} // namespace Karm::Base
