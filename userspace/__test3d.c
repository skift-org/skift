/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Framebuffer.h>
#include <libgraphic/Painter3D.h>
#include <libmath/Face.h>
#include <libmath/Matrix4.h>
#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

static const Face cude_mesh[] = {
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

    Vector3 camera_position = {0, 0, 0};

    double theta = 0;

    do
    {
        theta += 0.01;

        Matrix4 matRotZ = matrix_create_rotationX(theta);
        Matrix4 matRotX = matrix_create_rotationZ(theta);

        Color background_color = HSV((int)(theta / 10 * 360 + 180) % 360, 0.5, 0.75);

        painter_clear(framebuffer->painter, background_color);

        for (int i = 0; i < 12; i++)
        {
            Face tri = cude_mesh[i];
            Face triProjected;

            tri.a.X -= 0.5;
            tri.b.X -= 0.5;
            tri.c.X -= 0.5;

            tri.a.Y -= 0.5;
            tri.b.Y -= 0.5;
            tri.c.Y -= 0.5;

            tri.a.Z -= 0.5;
            tri.b.Z -= 0.5;
            tri.c.Z -= 0.5;

            tri.a = matrix_apply_tranform(matRotZ, tri.a);
            tri.b = matrix_apply_tranform(matRotZ, tri.b);
            tri.c = matrix_apply_tranform(matRotZ, tri.c);

            tri.a = matrix_apply_tranform(matRotX, tri.a);
            tri.b = matrix_apply_tranform(matRotX, tri.b);
            tri.c = matrix_apply_tranform(matRotX, tri.c);

            tri.a.Z += 3.0;
            tri.b.Z += 3.0;
            tri.c.Z += 3.0;

            if (vector3_dot(face_normal(tri), vector3_sub(tri.a, camera_position)) < 0.0)
            {
                Vector3 light_direction = {0.0, 0, -1.0};
                light_direction = vector3_norm(light_direction);
                float light_force = abs(vector3_dot(face_normal(tri), light_direction));

                triProjected.a = matrix_apply_tranform(projection, tri.a);
                triProjected.b = matrix_apply_tranform(projection, tri.b);
                triProjected.c = matrix_apply_tranform(projection, tri.c);

                triProjected.a.X += 1;
                triProjected.a.Y += 1;
                triProjected.b.X += 1;
                triProjected.b.Y += 1;
                triProjected.c.X += 1;
                triProjected.c.Y += 1;

                triProjected.a.X *= 0.5 * framebuffer->width;
                triProjected.a.Y *= 0.5 * framebuffer->height;
                triProjected.b.X *= 0.5 * framebuffer->width;
                triProjected.b.Y *= 0.5 * framebuffer->height;
                triProjected.c.X *= 0.5 * framebuffer->width;
                triProjected.c.Y *= 0.5 * framebuffer->height;

                Color color = HSV((int)(theta / 10 * 360) % 360, 0.5, light_force);
                painter3D_fill_face(framebuffer->painter, triProjected, color);
                painter3D_draw_face(framebuffer->painter, triProjected, COLOR_BLACK);
            }
        }

        framebuffer_blit(framebuffer);
    } while (true);

    return 0;
}