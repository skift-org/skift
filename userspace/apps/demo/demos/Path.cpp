#include <libgraphic/rast/Rasterizer.h>

#include "demo/demos/Demos.h"

void draw_path(Graphic::Painter &painter, const Graphic::Path &path, Math::Mat3x2f transform, Graphic::Color color)
{
    for (size_t i = 0; i < path.subpaths().count(); i++)
    {
        auto &subpath = path.subpaths()[i];

        for (size_t i = 0; i < subpath.length(); i++)
        {
            Math::CubicBezierf curve = transform.apply(subpath.curves(i));

            painter.draw_line(curve.start(), curve.cp1(), color);
            painter.draw_line(curve.cp1(), curve.cp2(), color);
            painter.draw_line(curve.cp2(), curve.end(), color);

            painter.plot(curve.start(), Graphic::Colors::MAGENTA);
            painter.plot(curve.cp1(), Graphic::Colors::MAGENTA);
            painter.plot(curve.cp2(), Graphic::Colors::MAGENTA);
            painter.plot(curve.end(), Graphic::Colors::MAGENTA);
        }
    }
}

void path_draw(Graphic::Painter &painter, Math::Recti, float)
{
    painter.clear(Graphic::Colors::BLACK);

    Graphic::Rasterizer rast{painter.bitmap()};

    static auto p0 = Graphic::Path::parse("M12,8L10.67,8.09C9.81,7.07 7.4,4.5 5,4.5C5,4.5 3.03,7.46 4.96,11.41C4.41,12.24 4.07,12.67 4,13.66L2.07,13.95L2.28,14.93L4.04,14.67L4.18,15.38L2.61,16.32L3.08,17.21L4.53,16.32C5.68,18.76 8.59,20 12,20C15.41,20 18.32,18.76 19.47,16.32L20.92,17.21L21.39,16.32L19.82,15.38L19.96,14.67L21.72,14.93L21.93,13.95L20,13.66C19.93,12.67 19.59,12.24 19.04,11.41C20.97,7.46 19,4.5 19,4.5C16.6,4.5 14.19,7.07 13.33,8.09L12,8M9,11A1,1 0 0,1 10,12A1,1 0 0,1 9,13A1,1 0 0,1 8,12A1,1 0 0,1 9,11M15,11A1,1 0 0,1 16,12A1,1 0 0,1 15,13A1,1 0 0,1 14,12A1,1 0 0,1 15,11M11,14H13L12.3,15.39C12.5,16.03 13.06,16.5 13.75,16.5A1.5,1.5 0 0,0 15.25,15H15.75A2,2 0 0,1 13.75,17C13,17 12.35,16.59 12,16V16H12C11.65,16.59 11,17 10.25,17A2,2 0 0,1 8.25,15H8.75A1.5,1.5 0 0,0 10.25,16.5C10.94,16.5 11.5,16.03 11.7,15.39L11,14Z");
    static auto p1 = Graphic::Path::parse("m12 8-1.33.09c-.86-1.02-3.27-3.59-5.67-3.59c0 0-1.97 2.96-.04 6.91c-.55.83-.89 1.26-.96 2.25l-1.93.29.21.98 1.76-.26.14.71-1.57.94.47.89 1.45-.89c1.15 2.44 4.06 3.68 7.47 3.68c3.41 0 6.32-1.24 7.47-3.68l1.45.89.47-.89-1.57-.94.14-.71 1.76.26.21-.98-1.93-.29c-.07-.99-.41-1.42-.96-2.25c1.93-3.95-.04-6.91-.04-6.91c-2.4 0-4.81 2.57-5.67 3.59l-1.33-.09m-3 3a1 1 0 011 1a1 1 0 01-1 1a1 1 0 01-1-1a1 1 0 011-1m6 0a1 1 0 011 1a1 1 0 01-1 1a1 1 0 01-1-1a1 1 0 011-1m-4 3h2l-.7 1.39c.2.64.76 1.11 1.45 1.11a1.5 1.5 0 001.5-1.5h.5a2 2 0 01-2 2c-.75 0-1.4-.41-1.75-1v0h0c-.35.59-1 1-1.75 1a2 2 0 01-2-2h.5a1.5 1.5 0 001.5 1.5c.69 0 1.25-.47 1.45-1.11l-.7-1.39z");

    rast.fill(p0, Math::Mat3x2f::scale(10) * Math::Mat3x2f::translation({72, 48}), Graphic::Fill{Graphic::Colors::MAGENTA.with_alpha(0.5)});
    rast.fill(p1, Math::Mat3x2f::scale(10) * Math::Mat3x2f::translation({72, 48}), Graphic::Fill{Graphic::Colors::CYAN.with_alpha(0.5)});

    draw_path(painter, p0, Math::Mat3x2f::scale(10) * Math::Mat3x2f::translation({-5, 48}), Graphic::Colors::BLUE);
    draw_path(painter, p1, Math::Mat3x2f::scale(10) * Math::Mat3x2f::translation({-5, 48}), Graphic::Colors::GREEN);
}