#pragma once

#include <libmath/Mat3x2.h>
#include <libmath/Rect.h>
#include <libutils/Array.h>

namespace Graphic
{

struct TransformState
{
    Math::Vec2i origin;
    Math::Recti clip;
    Math::Mat3x2f trans;
};

struct SourceDestination
{
    Math::Recti source;
    Math::Recti destination;

    bool is_empty()
    {
        return destination.is_empty();
    }
};

struct TransformStack
{
private:
    static constexpr auto STATESTACK_SIZE = 32;

    int _top;
    Array<TransformState, STATESTACK_SIZE> _stats{};

    Math::Recti apply_clip(Math::Recti rectangle)
    {
        if (rectangle.colide_with(clip()))
        {
            rectangle = rectangle.clipped_with(clip());

            return rectangle;
        }
        else
        {
            return Math::Recti::empty();
        }
    }

    Math::Recti apply_origin(Math::Recti rectangle)
    {
        return rectangle.offset(origin());
    }

public:
    TransformStack(Math::Recti clip)
    {
        _top = 0;
        _stats[0] = {
            Math::Vec2i::zero(),
            clip,
            Math::Mat3x2f::identity(),
        };
    }

    void push()
    {
        Assert::lower_than(_top, STATESTACK_SIZE);

        _top++;
        _stats[_top] = _stats[_top - 1];
    }

    void pop()
    {
        Assert::greater_than(_top, 0);
        _top--;
    }

    void clip(Math::Recti rect)
    {
        Math::Recti transformed_rect = rect.offset(origin());
        Math::Recti clipped_rect = transformed_rect.clipped_with(clip());

        _stats[_top].clip = clipped_rect;
    }

    Math::Recti clip()
    {
        return _stats[_top].clip;
    }

    void origin(Math::Vec2i vec)
    {
        _stats[_top].origin += vec;
    }

    Math::Vec2i origin()
    {
        return _stats[_top].origin;
    }

    void trans(Math::Mat3x2f trans)
    {
        _stats[_top].trans = _stats[_top].trans * trans;
    }

    Math::Mat3x2f trans()
    {
        return _stats[_top].trans;
    }

    void translate(Math::Vec2f vec)
    {
        trans(Math::Mat3x2f::translation(vec));
    }

    void scale(Math::Vec2f vec)
    {
        trans(Math::Mat3x2f::scale(vec));
    }

    void scale(float scale)
    {
        trans(Math::Mat3x2f::scale(scale));
    }

    void skew_x(float skew)
    {
        trans(Math::Mat3x2f::skewX(skew));
    }

    void skew_y(float skew)
    {
        trans(Math::Mat3x2f::skewY(skew));
    }

    void rotation(float rotation)
    {
        trans(Math::Mat3x2f::rotation(rotation));
    }

    Math::Recti apply(Math::Recti rectangle)
    {
        Math::Recti result = apply_origin(rectangle);
        result = apply_clip(result);
        return result;
    }

    SourceDestination apply(Math::Recti source, Math::Recti destination)
    {
        if (destination.is_empty())
        {
            return {Math::Recti::empty(), Math::Recti::empty()};
        }

        Math::Recti transformed_destination = apply_origin(destination);
        Math::Recti clipped_destination = apply_clip(transformed_destination);

        if (clipped_destination.is_empty())
        {
            return {Math::Recti::empty(), Math::Recti::empty()};
        }

        double scalex = clipped_destination.width() / (double)destination.width();
        double scaley = clipped_destination.height() / (double)destination.height();

        double other_scalex = source.width() / (double)destination.width();
        double other_scaley = source.height() / (double)destination.height();

        Math::Recti clipped_source = source.scaled(scalex, scaley);
        clipped_source = clipped_source.offset((clipped_destination.position() - transformed_destination.position()) * Math::Vec2d{other_scalex, other_scaley});

        return {clipped_source, clipped_destination};
    }
};

} // namespace Graphic
