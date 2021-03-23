#include <libgraphic/svg/Path.h>
#include <libutils/ScannerUtils.h>

namespace Graphic
{
static constexpr auto WHITESPACE = "\n\r\t ";
static constexpr auto OPERATIONS = "MmZzLlHhVvCcSsQqTtAa";

static void whitespace(Scanner &scan)
{
    scan.eat(WHITESPACE);
}

static void whitespace_or_comma(Scanner &scan)
{
    whitespace(scan);

    if (scan.skip(','))
    {
        whitespace(scan);
    }
}

static Vec2f coordinate(Scanner &scan)
{
    auto x = scan_float(scan);
    whitespace_or_comma(scan);
    auto y = scan_float(scan);
    whitespace_or_comma(scan);

    return Vec2f{(float)x, (float)y};
}

static int arcflags(Scanner &scan)
{
    int flags = 0;

    if (scan.current_is("1"))
    {
        flags |= Arc::LARGE;
    }
    scan.foreward();

    whitespace_or_comma(scan);

    if (scan.current_is("1"))
    {
        flags |= Arc::SWEEP;
    }

    scan.foreward();

    whitespace_or_comma(scan);

    return flags;
}

static void operation(Scanner &scan, Path &path, char operation)
{
    switch (operation)
    {
    case 'M':
        path.begin_subpath(coordinate(scan));

        whitespace(scan);

        // If a moveto is followed by multiple pairs of coordinates,
        // the subsequent pairs are treated as implicit lineto commands.
        while (scan.do_continue() &&
               !scan.current_is(OPERATIONS))
        {
            path.line_to(coordinate(scan));
        }
        break;

    case 'm':
        path.begin_subpath_relative(coordinate(scan));

        whitespace(scan);

        // If a moveto is followed by multiple pairs of coordinates,
        // the subsequent pairs are treated as implicit lineto commands.
        while (scan.do_continue() &&
               !scan.current_is(OPERATIONS))
        {
            path.line_to_relative(coordinate(scan));
        }
        break;

    case 'Z':
    case 'z':
        path.close_subpath();
        break;

    case 'L':
        path.line_to(coordinate(scan));
        break;

    case 'l':
        path.line_to_relative(coordinate(scan));
        break;

    case 'H':
        path.hline_to(scan_float(scan));
        break;

    case 'h':
        path.hline_to_relative(scan_float(scan));
        break;

    case 'V':
        path.vline_to(scan_float(scan));
        break;

    case 'v':
        path.vline_to_relative(scan_float(scan));
        break;

    case 'C':
    {
        auto cp1 = coordinate(scan);
        auto cp2 = coordinate(scan);
        auto point = coordinate(scan);

        path.cubic_bezier_to(cp1, cp2, point);

        break;
    }

    case 'c':
    {
        auto cp1 = coordinate(scan);
        auto cp2 = coordinate(scan);
        auto point = coordinate(scan);

        path.cubic_bezier_to_relative(cp1, cp2, point);

        break;
    }

    case 'S':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.smooth_cubic_bezier_to(cp, point);

        break;
    }

    case 's':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.smooth_cubic_bezier_to_relative(cp, point);

        break;
    }

    case 'Q':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.quad_bezier_to(cp, point);

        break;
    }

    case 'q':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.quad_bezier_to_relative(cp, point);

        break;
    }

    case 'T':
    {
        path.smooth_quad_bezier_to(coordinate(scan));
        break;
    }

    case 't':
    {
        path.smooth_quad_bezier_to_relative(coordinate(scan));
        break;
    }

    case 'A':
    {
        auto rx = scan_float(scan);
        whitespace_or_comma(scan);

        auto ry = scan_float(scan);
        whitespace_or_comma(scan);

        auto a = scan_float(scan);
        whitespace_or_comma(scan);

        auto flags = arcflags(scan);

        auto point = coordinate(scan);

        path.arc_to(rx, ry, a, flags, point);
        break;
    }

    case 'a':
    {
        auto rx = scan_float(scan);
        whitespace_or_comma(scan);

        auto ry = scan_float(scan);
        whitespace_or_comma(scan);

        auto a = scan_float(scan);
        whitespace_or_comma(scan);

        auto flags = arcflags(scan);

        auto point = coordinate(scan);
        path.arc_to_relative(rx, ry, a, flags, point);

        break;
    }

    default:
        break;
    }
}

Path Path::parse(Scanner &scan)
{
    Path path;

    whitespace(scan);

    if (scan.skip("none"))
    {
        // None indicates that there is no path data for the element
        return path;
    }

    while (scan.do_continue() &&
           scan.current_is(OPERATIONS))
    {
        char op = scan.current();
        scan.foreward();

        do
        {
            whitespace(scan);
            operation(scan, path, op);
            whitespace_or_comma(scan);
        } while (scan.do_continue() && !scan.current_is(OPERATIONS));
    }

    return path;
}

} // namespace Graphic
