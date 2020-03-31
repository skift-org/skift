#include <libmath/Face.h>

Vector3 face_normal(Face face)
{
    Vector3 line1, line2;

    line1.X = face.b.X - face.a.X;
    line1.Y = face.b.Y - face.a.Y;
    line1.Z = face.b.Z - face.a.Z;

    line2.X = face.c.X - face.a.X;
    line2.Y = face.c.Y - face.a.Y;
    line2.Z = face.c.Z - face.a.Z;

    return vector3_cross(line1, line2);
}