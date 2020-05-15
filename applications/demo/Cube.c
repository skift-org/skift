/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Painter3D.h>
#include <libmath/Face.h>
#include <libmath/Matrix4.h>

#include "demo/Demos.h"

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

static Vec3f camera = {0, 0, 0};

void cube_draw(Painter *painter, Rectangle screen, double time)
{
    Matrix4 projection = matrix_create_projection(0.1, 1000.0f, 45.0f, screen.height / (double)screen.width);

    Matrix4 matRotZ = matrix_create_rotationX(time);
    Matrix4 matRotX = matrix_create_rotationZ(time);

    Color background_color = HSV((int)(time / 10 * 360 + 180) % 360, 0.5, 0.75);

    painter_clear_rectangle(painter, screen, background_color);

    for (int i = 0; i < 12; i++)
    {
        Face tri = cude_mesh[i];
        Face triProjected;

        tri.a.x -= 0.5;
        tri.b.x -= 0.5;
        tri.c.x -= 0.5;

        tri.a.y -= 0.5;
        tri.b.y -= 0.5;
        tri.c.y -= 0.5;

        tri.a.z -= 0.5;
        tri.b.z -= 0.5;
        tri.c.z -= 0.5;

        tri.a = matrix_apply_tranform(matRotZ, tri.a);
        tri.b = matrix_apply_tranform(matRotZ, tri.b);
        tri.c = matrix_apply_tranform(matRotZ, tri.c);

        tri.a = matrix_apply_tranform(matRotX, tri.a);
        tri.b = matrix_apply_tranform(matRotX, tri.b);
        tri.c = matrix_apply_tranform(matRotX, tri.c);

        tri.a.z += 3.0;
        tri.b.z += 3.0;
        tri.c.z += 3.0;

        if (vec3f_dot(face_normal(tri), vec3f_sub(tri.a, camera)) < 0.0)
        {
            Vec3f light_direction = {0.0, 0, -1.0};
            light_direction = vec3f_norm(light_direction);
            float light_force = abs(vec3f_dot(face_normal(tri), light_direction));

            triProjected.a = matrix_apply_tranform(projection, tri.a);
            triProjected.b = matrix_apply_tranform(projection, tri.b);
            triProjected.c = matrix_apply_tranform(projection, tri.c);

            triProjected.a.x += 1;
            triProjected.a.y += 1;
            triProjected.b.x += 1;
            triProjected.b.y += 1;
            triProjected.c.x += 1;
            triProjected.c.y += 1;

            triProjected.a.x *= 0.5 * screen.width + screen.x;
            triProjected.a.y *= 0.5 * screen.height + screen.y;
            triProjected.b.x *= 0.5 * screen.width + screen.x;
            triProjected.b.y *= 0.5 * screen.height + screen.y;
            triProjected.c.x *= 0.5 * screen.width + screen.x;
            triProjected.c.y *= 0.5 * screen.height + screen.y;

            //triProjected.a.x += screen.x;
            //triProjected.a.y += screen.y;
            //triProjected.b.x += screen.x;
            //triProjected.b.y += screen.y;
            //triProjected.c.x += screen.x;
            //triProjected.c.y += screen.y;

            Color color = HSV((int)(time / 10 * 360) % 360, 0.5, light_force);
            painter3D_fill_face(painter, triProjected, color);
            painter3D_draw_face(painter, triProjected, COLOR_BLACK);
        }
    }
}
