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

        if (vec3f_dot(face_normal(tri), vec3f_sub(tri.a, camera)) < 0.0)
        {
            Vec3f light_direction = {0.0, 0, -1.0};
            light_direction = vec3f_norm(light_direction);
            float light_force = abs(vec3f_dot(face_normal(tri), light_direction));

            triProjected.a = matrix_apply_tranform(projection, tri.a);
            triProjected.b = matrix_apply_tranform(projection, tri.b);
            triProjected.c = matrix_apply_tranform(projection, tri.c);

            triProjected.a.X += 1;
            triProjected.a.Y += 1;
            triProjected.b.X += 1;
            triProjected.b.Y += 1;
            triProjected.c.X += 1;
            triProjected.c.Y += 1;

            triProjected.a.X *= 0.5 * screen.width + screen.X;
            triProjected.a.Y *= 0.5 * screen.height + screen.Y;
            triProjected.b.X *= 0.5 * screen.width + screen.X;
            triProjected.b.Y *= 0.5 * screen.height + screen.Y;
            triProjected.c.X *= 0.5 * screen.width + screen.X;
            triProjected.c.Y *= 0.5 * screen.height + screen.Y;

            //triProjected.a.X += screen.X;
            //triProjected.a.Y += screen.Y;
            //triProjected.b.X += screen.X;
            //triProjected.b.Y += screen.Y;
            //triProjected.c.X += screen.X;
            //triProjected.c.Y += screen.Y;

            Color color = HSV((int)(time / 10 * 360) % 360, 0.5, light_force);
            painter3D_fill_face(painter, triProjected, color);
            painter3D_draw_face(painter, triProjected, COLOR_BLACK);
        }
    }
}
