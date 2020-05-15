#include <libmath/Face.h>

Vec3f face_normal(Face face)
{
    Vec3f line1, line2;

    line1.x = face.b.x - face.a.x;
    line1.y = face.b.y - face.a.y;
    line1.z = face.b.z - face.a.z;

    line2.x = face.c.x - face.a.x;
    line2.y = face.c.y - face.a.y;
    line2.z = face.c.z - face.a.z;

    return vec3f_cross(line1, line2);
}