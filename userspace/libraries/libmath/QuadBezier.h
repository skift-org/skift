#pragma once

namespace Math
{

template <typename Scalar>
class QuadBezier
{
private:
    Math::Vec2<Scalar> _start;
    Math::Vec2<Scalar> _cp;
    Math::Vec2<Scalar> _end;

public:
    const Math::Vec2<Scalar> &start() const { return _start; }
    const Math::Vec2<Scalar> &cp() const { return _cp2; }
    const Math::Vec2<Scalar> &end() const { return _end; }

    QuadBezier(Math::Vec2<Scalar> start,
               Math::Vec2<Scalar> cp,
               Math::Vec2<Scalar> end)
        : _start{start},
          _cp1{cp},
          _end{end}
    {
    }
};

using QuadBezieri = QuadBezier<int>;
using QuadBezierf = QuadBezier<float>;
using QuadBezierd = QuadBezier<double>;

} // namespace Math
