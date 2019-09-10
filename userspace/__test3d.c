/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libgraphic/bitmap.h>
#include <libgraphic/painter.h>
#include <libsystem/logger.h>
#include <libsystem/assert.h>

#include <libdevice/framebuffer.h>

typedef struct
{
    double X;
    double Y;
    double Z;
} vector3_t;

vector3_t vector3_normalized(vector3_t v)
{
    float l = sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);

    v.X /= l;
    v.Y /= l;
    v.Z /= l;

    return v;
}

vector3_t vector3_substract(vector3_t a, vector3_t b)
{
    return (vector3_t){a.X - b.X, a.Y - b.Y, a.Z - b.Z};
}

vector3_t vector3_cross_product(vector3_t a, vector3_t b)
{
    vector3_t r;

    r.X = a.Y * b.Z - a.Z * b.Y;
    r.Y = a.Z * b.X - a.X * b.Z;
    r.Z = a.X * b.Y - a.Y * b.X;

    r = vector3_normalized(r);

    return r;
}

double vector3_dot_product(vector3_t a, vector3_t b)
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

typedef struct
{
    vector3_t p[3];
} triangle_t;

vector3_t triangle_normal(triangle_t tri)
{
    vector3_t line1, line2;

    line1.X = tri.p[1].X - tri.p[0].X;
    line1.Y = tri.p[1].Y - tri.p[0].Y;
    line1.Z = tri.p[1].Z - tri.p[0].Z;

    line2.X = tri.p[2].X - tri.p[0].X;
    line2.Y = tri.p[2].Y - tri.p[0].Y;
    line2.Z = tri.p[2].Z - tri.p[0].Z;

    return vector3_cross_product(line1, line2);
}

typedef struct
{
    double m[4][4];
} matrix4_t;

static const triangle_t cude_mesh[] = {
    {{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}},
    {{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}},
    {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}},
    {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}}},
    {{{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}},
    {{{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}},
    {{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}},
    {{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}},
    {{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}},
    {{{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}},
    {{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}},
    {{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}},
};

framebuffer_mode_info_t mode_info = {true, 800, 600};

vector3_t painter_apply_tranform(vector3_t position, matrix4_t transform)
{
    vector3_t out = {0};

    out.X = position.X * transform.m[0][0] +
            position.Y * transform.m[1][0] +
            position.Z * transform.m[2][0] +
            transform.m[3][0];

    out.Y = position.X * transform.m[0][1] +
            position.Y * transform.m[1][1] +
            position.Z * transform.m[2][1] +
            transform.m[3][1];

    out.Z = position.X * transform.m[0][2] +
            position.Y * transform.m[1][2] +
            position.Z * transform.m[2][2] +
            transform.m[3][2];

    double w = position.X * transform.m[0][3] +
               position.Y * transform.m[1][3] +
               position.Z * transform.m[2][3] +
               transform.m[3][3];

    if (w != 0.0f)
    {
        out.X /= w;
        out.Y /= w;
        out.Z /= w;
    }

    return out;
}

matrix4_t painter_create_projection(double near, double far, double fov, double aspect_ratio)
{
    double fov_rad = 1.0 / tan(fov * 0.5 / 180.0 * PI);
    matrix4_t projection = {0};

    projection.m[0][0] = aspect_ratio * fov_rad;
    projection.m[1][1] = fov_rad;
    projection.m[2][2] = far / (far - near);
    projection.m[3][2] = (-far * near) / (far - near);
    projection.m[2][3] = 1.0;
    projection.m[3][3] = 0.0;

    return projection;
}

void painter_draw_triangle(painter_t *paint, triangle_t triangle, color_t color)
{
    painter_draw_line(paint, (point_t){triangle.p[0].X, triangle.p[0].Y}, (point_t){triangle.p[1].X, triangle.p[1].Y}, color);
    painter_draw_line(paint, (point_t){triangle.p[1].X, triangle.p[1].Y}, (point_t){triangle.p[2].X, triangle.p[2].Y}, color);
    painter_draw_line(paint, (point_t){triangle.p[2].X, triangle.p[2].Y}, (point_t){triangle.p[0].X, triangle.p[0].Y}, color);
}

void painter_fill_triangle(painter_t *paint, triangle_t triangle, color_t color)
{
    point_t a = (point_t){triangle.p[0].X, triangle.p[0].Y};
    point_t b = (point_t){triangle.p[1].X, triangle.p[1].Y};
    point_t c = (point_t){triangle.p[2].X, triangle.p[2].Y};

    int dx = abs(b.X - a.X), sx = a.X < b.X ? 1 : -1;
    int dy = abs(b.Y - a.Y), sy = a.Y < b.Y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        painter_draw_line(paint, a, c, color);

        if (a.X == b.X && a.Y == b.Y)
            break;

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            a.X += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            a.Y += sy;
        }
    }
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    iostream_t *framebuffer_device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (framebuffer_device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);
        return -1;
    }

    if (iostream_call(framebuffer_device, FRAMEBUFFER_CALL_SET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return -1;
    }

    bitmap_t *framebuffer = bitmap(800, 600);
    painter_t *paint = painter(framebuffer);

    matrix4_t projection = painter_create_projection(0.1, 1000.0f, 45.0f, framebuffer->height / (double)framebuffer->width);

    vector3_t camera_position = {0, 0, 0};

    double theta = 0;

    do
    {
        theta += 0.01;

        matrix4_t matRotZ = {0}, matRotX = {0};

        // Rotation Z
        matRotZ.m[0][0] = cos(theta);
        matRotZ.m[0][1] = sin(theta);
        matRotZ.m[1][0] = -sin(theta);
        matRotZ.m[1][1] = cos(theta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cos(theta * 0.5);
        matRotX.m[1][2] = sin(theta * 0.5);
        matRotX.m[2][1] = -sin(theta * 0.5);
        matRotX.m[2][2] = cos(theta * 0.5);
        matRotX.m[3][3] = 1;

        painter_clear_rect(paint, bitmap_bound(framebuffer), COLOR_BLACK);

        for (int i = 0; i < 12; i++)
        {
            triangle_t tri = cude_mesh[i];
            triangle_t triProjected;

            tri.p[0].X -= 0.5;
            tri.p[1].X -= 0.5;
            tri.p[2].X -= 0.5;
            tri.p[0].Y -= 0.5;
            tri.p[1].Y -= 0.5;
            tri.p[2].Y -= 0.5;
            tri.p[0].Z -= 0.5;
            tri.p[1].Z -= 0.5;
            tri.p[2].Z -= 0.5;

            UNUSED(matRotZ);
            tri.p[0] = painter_apply_tranform(tri.p[0], matRotZ);
            tri.p[1] = painter_apply_tranform(tri.p[1], matRotZ);
            tri.p[2] = painter_apply_tranform(tri.p[2], matRotZ);

            UNUSED(matRotX);
            tri.p[0] = painter_apply_tranform(tri.p[0], matRotX);
            tri.p[1] = painter_apply_tranform(tri.p[1], matRotX);
            tri.p[2] = painter_apply_tranform(tri.p[2], matRotX);

            tri.p[0].Z += 3.0;
            tri.p[1].Z += 3.0;
            tri.p[2].Z += 3.0;

            if (true || vector3_dot_product(triangle_normal(tri), vector3_substract(tri.p[0], camera_position)) < 0.0)
            {
                // vector3_t light_direction = { 0.0, 0, -1.0};
                // light_direction = vector3_normalized(light_direction);
                // float light_force = vector3_dot_product(triangle_normal(tri), light_direction);

                triProjected.p[0] = painter_apply_tranform(tri.p[0], projection);
                triProjected.p[1] = painter_apply_tranform(tri.p[1], projection);
                triProjected.p[2] = painter_apply_tranform(tri.p[2], projection);

                triProjected.p[0].X += 1.0;
                triProjected.p[0].Y += 1.0;
                triProjected.p[1].X += 1.0;
                triProjected.p[1].Y += 1.0;
                triProjected.p[2].X += 1.0;
                triProjected.p[2].Y += 1.0;

                triProjected.p[0].X *= 0.5 * framebuffer->width;
                triProjected.p[0].Y *= 0.5 * framebuffer->height;
                triProjected.p[1].X *= 0.5 * framebuffer->width;
                triProjected.p[1].Y *= 0.5 * framebuffer->height;
                triProjected.p[2].X *= 0.5 * framebuffer->width;
                triProjected.p[2].Y *= 0.5 * framebuffer->height;

                color_t color = COLOR(0x0066ff); //HSV(0.0, 1.0, 1.0);
                color.A = 7;
                painter_fill_triangle(paint, triProjected, color);
            }
        }

        iostream_call(framebuffer_device, FRAMEBUFFER_CALL_BLIT, framebuffer->buffer);
    } while (true);

    return 0;
}