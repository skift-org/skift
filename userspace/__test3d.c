/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

#include <libgraphic/Framebuffer.h>
#include <libgraphic/Matrix.h>

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
    vector3_t a;
    vector3_t b;
    vector3_t c;
} face_t;

vector3_t face_normal(face_t tri)
{
    vector3_t line1, line2;

    line1.X = tri.b.X - tri.a.X;
    line1.Y = tri.b.Y - tri.a.Y;
    line1.Z = tri.b.Z - tri.a.Z;

    line2.X = tri.c.X - tri.a.X;
    line2.Y = tri.c.Y - tri.a.Y;
    line2.Z = tri.c.Z - tri.a.Z;

    return vector3_cross_product(line1, line2);
}

static const face_t cude_mesh[] = {
    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
    {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
    {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
    {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
    {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
    {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
    {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
    {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
};

vector3_t matrix_apply_tranform(vector3_t position, Matrix4 transform)
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

void painter3D_draw_line(Painter *painter, vector3_t va, vector3_t vb, Color color)
{
    painter_draw_line(painter, (Point){va.X, va.Y}, (Point){vb.X, vb.Y}, color);
}

void painter3D_draw_face(Painter *painter, face_t face, Color color)
{
    painter3D_draw_line(painter, face.a, face.b, color);
    painter3D_draw_line(painter, face.b, face.c, color);
    painter3D_draw_line(painter, face.c, face.a, color);
}

void painter3D_fill_face(Painter *painter, face_t face, Color color)
{
    vector3_t a = face.a;
    vector3_t b = face.b;
    vector3_t c = face.c;

    if (a.Y > b.Y)
        SWAP(a, b);
    if (a.Y > c.Y)
        SWAP(a, c);
    if (b.Y > c.Y)
        SWAP(b, c);

    vector3_t s = a;
    vector3_t e = a;

    double dx1 = 0;
    double dx2 = 0;
    double dx3 = 0;

    if (b.Y - a.Y > 0)
    {
        dx1 = (b.X - a.X) / (b.Y - a.Y);
    }

    if (c.Y - a.Y > 0)
    {
        dx2 = (c.X - a.X) / (c.Y - a.Y);
    }

    if (c.Y - b.Y > 0)
    {
        dx3 = (c.X - b.X) / (c.Y - b.Y);
    }

    if (dx1 > dx2)
    {
        for (; s.Y <= b.Y; s.Y++, e.Y++, s.X += dx2, e.X += dx1)
        {
            painter3D_draw_line(painter, (vector3_t){s.X - 1, s.Y, 0}, (vector3_t){e.X + 1, s.Y, 0}, color);
        }

        e = b;

        for (; s.Y <= c.Y; s.Y++, e.Y++, s.X += dx2, e.X += dx3)
        {
            painter3D_draw_line(painter, (vector3_t){s.X - 1, s.Y, 0}, (vector3_t){e.X + 1, s.Y, 0}, color);
        }
    }
    else
    {
        for (; s.Y <= b.Y; s.Y++, e.Y++, s.X += dx1, e.X += dx2)
        {
            painter3D_draw_line(painter, (vector3_t){s.X - 1, s.Y, 0}, (vector3_t){e.X + 1, s.Y, 0}, color);
        }

        s = b;

        for (; s.Y <= c.Y; s.Y++, e.Y++, s.X += dx3, e.X += dx2)
        {
            painter3D_draw_line(painter, (vector3_t){s.X - 1, s.Y, 0}, (vector3_t){e.X + 1, s.Y, 0}, color);
        }
    }
}

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    Framebuffer *framebuffer = framebuffer_open();

    if (handle_has_error(framebuffer))
    {
        handle_printf_error(framebuffer, "failled to open /dev/framebuffer");
        framebuffer_close(framebuffer);

        return -1;
    }

    Matrix4 projection = matrix_create_projection(0.1, 1000.0f, 45.0f, framebuffer->height / (double)framebuffer->width);

    vector3_t camera_position = {0, 0, 0};

    double theta = 0;

    do
    {
        theta += 0.01;

        Matrix4 matRotZ = matrix_create_rotationX(theta);
        Matrix4 matRotX = matrix_create_rotationZ(theta);

        Color background_color = HSV(abs(sin(theta / 10 + PI / 2)) * 360, 0.5, 0.75);

        painter_clear(framebuffer->painter, background_color);

        for (int i = 0; i < 12; i++)
        {
            face_t tri = cude_mesh[i];
            face_t triProjected;

            tri.a.X -= 0.5;
            tri.b.X -= 0.5;
            tri.c.X -= 0.5;
            tri.a.Y -= 0.5;
            tri.b.Y -= 0.5;
            tri.c.Y -= 0.5;
            tri.a.Z -= 0.5;
            tri.b.Z -= 0.5;
            tri.c.Z -= 0.5;

            tri.a = matrix_apply_tranform(tri.a, matRotZ);
            tri.b = matrix_apply_tranform(tri.b, matRotZ);
            tri.c = matrix_apply_tranform(tri.c, matRotZ);

            tri.a = matrix_apply_tranform(tri.a, matRotX);
            tri.b = matrix_apply_tranform(tri.b, matRotX);
            tri.c = matrix_apply_tranform(tri.c, matRotX);

            tri.a.Z += 3.0;
            tri.b.Z += 3.0;
            tri.c.Z += 3.0;

            if (vector3_dot_product(face_normal(tri), vector3_substract(tri.a, camera_position)) < 0.0)
            {
                vector3_t light_direction = {0.0, 0, -1.0};
                light_direction = vector3_normalized(light_direction);
                float light_force = abs(vector3_dot_product(face_normal(tri), light_direction));

                triProjected.a = matrix_apply_tranform(tri.a, projection);
                triProjected.b = matrix_apply_tranform(tri.b, projection);
                triProjected.c = matrix_apply_tranform(tri.c, projection);

                triProjected.a.X += 1.0;
                triProjected.a.Y += 1.0;
                triProjected.b.X += 1.0;
                triProjected.b.Y += 1.0;
                triProjected.c.X += 1.0;
                triProjected.c.Y += 1.0;

                triProjected.a.X *= 0.5 * framebuffer->width;
                triProjected.a.Y *= 0.5 * framebuffer->height;
                triProjected.b.X *= 0.5 * framebuffer->width;
                triProjected.b.Y *= 0.5 * framebuffer->height;
                triProjected.c.X *= 0.5 * framebuffer->width;
                triProjected.c.Y *= 0.5 * framebuffer->height;

                Color color = HSV(abs(sin(theta / 10)) * 360, 0.5, light_force);
                painter3D_fill_face(framebuffer->painter, triProjected, color);
                painter3D_draw_face(framebuffer->painter, triProjected, COLOR_BLACK);
            }
        }

        framebuffer_blit(framebuffer);
    } while (true);

    return 0;
}