#include <libgraphic/svg/Path.h>
#include <libio/NumberScanner.h>
#include <libio/Streams.h>
#include <libmath/Arc.h>

namespace Graphic
{

static constexpr auto WHITESPACE = "\n\r\t ";
static constexpr auto OPERATIONS = "MmZzLlHhVvCcSsQqTtAa";

static void parse_whitespace(IO::Scanner &scan)
{
    scan.eat_any(WHITESPACE);
}

static void parse_whitespace_or_comma(IO::Scanner &scan)
{
    parse_whitespace(scan);

    if (scan.skip(','))
    {
        parse_whitespace(scan);
    }
}

static float parse_float(IO::Scanner &scan)
{
    return IO::NumberScanner::decimal().scan_float(scan).unwrap_or(0);
}

static Math::Vec2f parse_coordinate(IO::Scanner &scan)
{
    auto x = parse_float(scan);
    parse_whitespace_or_comma(scan);
    auto y = parse_float(scan);
    parse_whitespace_or_comma(scan);

    return Math::Vec2f{(float)x, (float)y};
}

static int parse_arcflags(IO::Scanner &scan)
{
    int flags = 0;

    if (scan.next() == '1')
    {
        flags |= Math::Arcf::LARGE;
    }

    parse_whitespace_or_comma(scan);

    if (scan.next() == '1')
    {
        flags |= Math::Arcf::SWEEP;
    }

    parse_whitespace_or_comma(scan);

    return flags;
}

static void parse_operation(IO::Scanner &scan, Path &path, char operation)
{
    switch (operation)
    {
    case 'M':
        path.begin_subpath(parse_coordinate(scan));

        parse_whitespace(scan);

        // If a moveto is followed by multiple pairs of coordinates,
        // the subsequent pairs are treated as implicit lineto commands.
        while (!scan.ended() &&
               !scan.peek_is_any(OPERATIONS))
        {
            path.line_to(parse_coordinate(scan));
        }
        break;

    case 'm':
        path.begin_subpath_relative(parse_coordinate(scan));

        parse_whitespace(scan);

        // If a moveto is followed by multiple pairs of coordinates,
        // the subsequent pairs are treated as implicit lineto commands.
        while (!scan.ended() &&
               !scan.peek_is_any(OPERATIONS))
        {
            path.line_to_relative(parse_coordinate(scan));
        }
        break;

    case 'Z':
    case 'z':
        path.close_subpath();
        break;

    case 'L':
        path.line_to(parse_coordinate(scan));
        break;

    case 'l':
        path.line_to_relative(parse_coordinate(scan));
        break;

    case 'H':
        path.hline_to(parse_float(scan));
        break;

    case 'h':
        path.hline_to_relative(parse_float(scan));
        break;

    case 'V':
        path.vline_to(parse_float(scan));
        break;

    case 'v':
        path.vline_to_relative(parse_float(scan));
        break;

    case 'C':
    {
        auto cp1 = parse_coordinate(scan);
        auto cp2 = parse_coordinate(scan);
        auto point = parse_coordinate(scan);

        path.cubic_bezier_to(cp1, cp2, point);
        break;
    }

    case 'c':
    {
        auto cp1 = parse_coordinate(scan);
        auto cp2 = parse_coordinate(scan);
        auto point = parse_coordinate(scan);

        path.cubic_bezier_to_relative(cp1, cp2, point);
        break;
    }

    case 'S':
    {
        auto cp = parse_coordinate(scan);
        auto point = parse_coordinate(scan);

        path.smooth_cubic_bezier_to(cp, point);
        break;
    }

    case 's':
    {
        auto cp = parse_coordinate(scan);
        auto point = parse_coordinate(scan);
        path.smooth_cubic_bezier_to_relative(cp, point);

        break;
    }

    case 'Q':
    {
        auto cp = parse_coordinate(scan);
        auto point = parse_coordinate(scan);
        path.quad_bezier_to(cp, point);

        break;
    }

    case 'q':
    {
        auto cp = parse_coordinate(scan);
        auto point = parse_coordinate(scan);
        path.quad_bezier_to_relative(cp, point);

        break;
    }

    case 'T':
    {
        path.smooth_quad_bezier_to(parse_coordinate(scan));
        break;
    }

    case 't':
    {
        path.smooth_quad_bezier_to_relative(parse_coordinate(scan));
        break;
    }

    case 'A':
    {
        auto rx = parse_float(scan);
        parse_whitespace_or_comma(scan);

        auto ry = parse_float(scan);
        parse_whitespace_or_comma(scan);

        auto a = parse_float(scan);
        parse_whitespace_or_comma(scan);

        auto flags = parse_arcflags(scan);

        auto point = parse_coordinate(scan);

        path.arc_to(rx, ry, a, flags, point);
        break;
    }

    case 'a':
    {
        auto rx = parse_float(scan);
        parse_whitespace_or_comma(scan);

        auto ry = parse_float(scan);
        parse_whitespace_or_comma(scan);

        auto a = parse_float(scan);
        parse_whitespace_or_comma(scan);

        auto flags = parse_arcflags(scan);

        auto point = parse_coordinate(scan);
        path.arc_to_relative(rx, ry, a, flags, point);

        break;
    }

    default:
        break;
    }
}

Path Path::parse(const char *str)
{
    IO::MemoryReader memory{str};
    IO::Scanner scan{memory};
    return parse(scan);
}

Path Path::parse(IO::Scanner &scan)
{
    Path path;

    parse_whitespace(scan);

    if (scan.skip_word("none"))
    {
        // None indicates that there is no path data for the element
        return path;
    }

    while (!scan.ended() &&
           scan.peek_is_any(OPERATIONS))
    {
        char op = scan.next();

        do
        {
            parse_whitespace(scan);
            parse_operation(scan, path, op);
            parse_whitespace_or_comma(scan);
        } while (!scan.ended() && !scan.peek_is_any(OPERATIONS));
    }

    return path;
}

} // namespace Graphic
